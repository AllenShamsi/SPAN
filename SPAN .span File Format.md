# SPAN `.span` File Format

This document describes the structure of `.span` files produced by SPAN (Speech Production Annotation and Navigation). The format combines a human-readable text header with a binary payload containing synchronized audio and electromagnetic articulography (EMA) positional data.

The `.span` format is used by SPAN as an analysis format, but its structure is documented here so that the stored data can be independently interpreted or accessed from other software environments.

## File structure

A `.span` file consists of:

1. an 11-line text header;
2. a binary audio block;
3. one binary positional-data block for each stored EMA channel.

The binary payload begins immediately after the newline terminating header line 11.

Conceptually:

```text
.span file
│
├── 11-line text header
│
├── audio samples
│
├── positional channel 1
│   ├── sample 1: x y z phi theta rms
│   ├── sample 2: x y z phi theta rms
│   └── ...
├── positional channel 2
│   └── ...
└── positional channel N
    └── ...
```

## Text header

The header always contains 11 newline-terminated lines.

### Lines 1–6

SPAN begins with the first six text lines of the original Carstens `.pos` file. Lines 3–5 are replaced with SPAN-specific information:

```text
Line 1: copied from the original .pos header
Line 2: copied from the original .pos header
Line 3: channelsNum: <number>
Line 4: wavSR: <number>
Line 5: posSR: <number>
Line 6: copied from the original .pos header
```

`channelsNum` is the total number of channels represented by SPAN, including the audio channel. Thus:

```text
number of positional channels = channelsNum - 1
```

`wavSR` is the audio sampling rate in Hz.

`posSR` is the original EMA positional sampling rate in Hz.

### Line 7: channel labels

Line 7 contains the audio channel and positional-channel labels:

```text
0:audio 1:<label1> 2:<label2> ... N:<labelN>
```

For example:

```text
0:audio 1:REF-L 2:REF-R 3:REF-F 4:TT 5:TD 6:LL
```

Channel `0` always represents audio.

Positional channels are numbered consecutively from `1` in the same order in which their binary data blocks occur in the file.

The labels are those assigned by the user during SPAN preprocessing.

### Line 8: plot configuration

Line 8 begins with:

```text
Config:
```

followed by the currently stored channel configurations.

A newly generated file initially contains the selected positional-channel labels, for example:

```text
Config: REF-L REF-R REF-F TT TD LL
```

SPAN may subsequently update this line when visualization configurations are changed. Configuration names may encode displayed dimensions or motion types such as displacement, velocity, or acceleration.

The configuration line is metadata and does not alter the stored positional samples.

### Line 9: derived-channel recipe

Line 9 begins with:

```text
Recipe:
```

A newly generated file contains:

```text
Recipe: None
```

When derived channels are defined in SPAN, this line may contain one or more definitions of the form:

```text
<derived_name>: <signal1> <signal2> <operation>;
```

Currently supported operations include:

```text
SUB
```

for component-wise subtraction and:

```text
DIST
```

for Euclidean distance.

Derived channels are reconstructed by SPAN when the file is loaded. They are not stored as additional binary positional blocks.

### Line 10: number of audio samples

```text
wavSamples: <number>
```

This is the number of 32-bit floating-point samples stored in the binary audio block.

### Line 11: number of positional samples

```text
trialSamples: <number>
```

This is the number of EMA samples stored for each positional channel.

All positional channels are expected to contain the same number of samples.

## Binary representation

All numerical values in the binary payload are written directly from C++ `float` values.

In current SPAN builds, these values are 32-bit IEEE-754 floating-point numbers. Current supported Windows and macOS hardware uses little-endian byte order; no separate byte-order marker is stored in the file.

Each numerical value therefore occupies 4 bytes.

## Audio block

Immediately after header line 11, SPAN stores:

```text
wavSamples
```

consecutive 32-bit floating-point audio samples.

The audio data are mono and normalized to the range `[-1, 1]` relative to the largest absolute sample value in the recording.

For mono WAV input, the original mono samples are used.

For stereo WAV input, the left and right channels are averaged to mono before normalization.

The audio block therefore has a size of:

```text
wavSamples × 4 bytes
```

## Positional-data blocks

The audio block is followed by one positional block for each EMA channel.

The number of positional channels is:

```text
channelsNum - 1
```

Channels are stored sequentially in the same order as the positional channels listed on header line 7.

The layout is **channel-major**, not sample-major. That is, all samples for positional channel 1 are written first, followed by all samples for positional channel 2, and so forth.

For each positional sample, six 32-bit floating-point values are stored in this order:

```text
x
y
z
phi
theta
rms
```

Each sample therefore occupies:

```text
6 × 4 = 24 bytes
```

Each positional-channel block occupies:

```text
trialSamples × 24 bytes
```

The full positional payload occupies:

```text
(channelsNum - 1) × trialSamples × 24 bytes
```

## Position preprocessing represented in the file

The stored `x`, `y`, and `z` coordinates are the spatially corrected positional trajectories produced during SPAN preprocessing.

Before they are written to the `.span` file, SPAN applies a first-order low-pass Butterworth filter with a 20-Hz cutoff to each stored `x`, `y`, and `z` trajectory.

The `phi`, `theta`, and `rms` values are stored without this positional filtering and are currently preserved from the input data.

The binary positional data remain stored at `posSR`, the original EMA positional sampling rate.

When SPAN reads a `.span` file for analysis, the positional trajectories are interpolated in memory from `posSR` to `wavSR` for synchronized display and analysis. This interpolation does not modify the positional samples stored in the `.span` file itself.

## Reading a `.span` file independently

An external reader can reconstruct the contents of a `.span` file using the following procedure:

1. Read exactly 11 newline-terminated text header lines.
2. Parse:
   - `channelsNum`
   - `wavSR`
   - `posSR`
   - positional channel labels
   - `wavSamples`
   - `trialSamples`
3. Beginning immediately after the newline ending line 11, read `wavSamples` 32-bit floating-point values as the mono audio signal.
4. Let:

```text
N = channelsNum - 1
```

5. For each of the `N` positional channels, read `trialSamples` records.
6. Each positional record consists of six consecutive 32-bit floating-point values:

```text
x, y, z, phi, theta, rms
```

7. Associate positional block 1 with channel 1 on header line 7, positional block 2 with channel 2, and so forth.

## Payload-size check

Ignoring the variable-length text header, the expected binary payload size in bytes is:

```text
(wavSamples × 4)
+
((channelsNum - 1) × trialSamples × 6 × 4)
```

Equivalently:

```text
4 × wavSamples
+
24 × (channelsNum - 1) × trialSamples
```

This provides a simple integrity check when implementing an independent reader.

## Derived channels and configurations

The binary payload contains only the audio signal and the positional channels exported during preprocessing.

Plot configurations and derived-channel definitions are represented in the text header. SPAN can update the `Config:` and `Recipe:` lines later without rewriting the binary payload.

Derived signals such as articulator subtraction or Euclidean distance are therefore reconstructed from the stored positional trajectories when the file is opened.

## Scope

`.span` is SPAN's working analysis format rather than a standardized EMA exchange format. The specification is provided so that the contents are transparent and so that researchers can implement readers, converters, or other interoperability tools in languages and environments of their choice.

The implementation itself is open source; the authoritative reader and writer are contained in SPAN's source code.
