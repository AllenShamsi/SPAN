# SPAN

Version 0.3.0.

SPAN is a desktop application for preprocessing, visualizing, annotating, and exporting electromagnetic articulography (EMA) data together with synchronized audio.

It is intended for workflows that begin with raw AG50x `.pos` and `.wav` recordings and continue through spatial correction, visualization, landmark annotation, template-based labeling, and CSV export.

## Windows portable use

Extract the complete Windows portable archive for SPAN 0.3.0 to a writable
directory and run SPAN.exe. Keep its DLLs and plugin subdirectories together. Qt or MSYS2
installation is not required for the portable package; supported Windows x64,
working graphics drivers, and an audio output device are needed for the
corresponding features. Paths containing spaces are supported. Templates are
located relative to the working directory, so start SPAN from its extracted
folder. Do not copy SPAN.exe alone or merge it with an older distribution.

Release notes and available downloads are provided on the GitHub Releases page:
https://github.com/AllenShamsi/SPAN/releases

See LICENSE.txt for the SPAN license. Third-party licensing information appears
below, in the preserved source notices and in the respective dependency
documentation. Verify portable builds on a clean Windows machine and
representative hardware before distribution. User recordings and local templates
are not included.

## Main workflow

A typical SPAN workflow is:

1. Convert raw bite-plane and trial recordings into `.span` files
2. Open `.span` files in the main window
3. Configure kinematic channels for plotting
4. Create derived channels if needed
5. Place, adjust, and export landmarks
6. Optionally save and apply templates across similar files

---

## Quick start

### 1. Preprocess raw AG50x data into `.span`

Open the **Pre-processing Window**.

1. Click **Select BP File** and choose the bite-plane file.
2. Click **Select Trial Files** and choose matching trial `.pos` and `.wav` files.
3. In **Sensor Layout**, check the channels you want to use.
4. Label the three reference channels exactly as:
   - `REF-L`
   - `REF-R`
   - `REF-F`
5. Label all other checked channels as needed.
6. In **Bite Plane Channels**, assign three non-reference channels to:
   - `BP-L`
   - `BP-F`
   - `BP-R`
7. In **Origin**, keep **BP centroid** or select a raw sensor whose mean position
   in the bite-plane recording will define the coordinate origin.
8. Click **Start** to compute and preview the corrected bite-plane view.
9. Click **Proceed** once to preview corrected trial data.
10. Click **Proceed** again to choose an output folder and export `.span` files.

#### Notes
- Trial `.pos` and `.wav` files must have matching base names.
- The reference labels must appear exactly as `REF-L`, `REF-R`, and `REF-F`.
- The bite-plane mapping must use three non-reference channels.

---

### 2. Open and inspect `.span` files

1. In the main window, click **Open**.
2. Select one or more `.span` files.
3. Double-click a file or click **View** to load it.
4. Inspect the file using the waveform, spectrogram, kinematic plots, landmark table, and 3D sensor view.

---

### 3. Configure channels for plotting

1. Choose a sensor or derived signal.
2. Select the motion type:
   - displacement
   - velocity
   - acceleration
3. Choose the dimensions to display:
   - X
   - Y
   - Z
4. Click **Add** to add the channel to the plot stack.
5. Use the following controls to manage the configuration list:
   - **Up**
   - **Down**
   - **Update**
   - **Remove**
   - **Apply to All**

Configuration settings are stored in the `.span` file without changing the underlying positional data.

---

### 4. Create derived channels

1. Choose two input channels.
2. Enter a name for the derived channel.
3. Select an operation, such as:
   - subtract component
   - Euclidean distance
4. Click **Derive**.
5. Click **Clear** in the derived-channel section to remove all derived channels and reset the recipe line.

Derived channels are useful for inter-articulator relationships, constriction proxies, and other custom kinematic measures.

---

### 5. Landmark annotation

#### Basic actions
- **Right-click** on a plot to place a plain landmark.
- **Shift + right-click** on a one-dimensional displacement trace to snap to a nearby displacement extremum and place `MaxC` with surrounding landmarks when velocity information is available.
- For two-dimensional displacement configurations (XY, XZ or YZ), assisted placement uses tangential-speed minima and surrounding speed peaks while the visible traces remain positions.
- Drag a landmark handle to move it.
- Select landmarks in the landmark table to review or remove them.

#### Derived landmarks
When the appropriate velocity information is available, SPAN can derive additional landmarks such as:
- `GONS`
- `NONS`
- `PVEL1`
- `PVEL2`
- `NOFFS`
- `GOFFS`

#### Independent velocity thresholds

In Settings > Landmark Detection, set GONS, NONS, NOFFS, and GOFFS separately
(1-50%, default 20% each). Click Apply to activate the settings for subsequent
manual assisted, DTW-assisted, and batch annotation. Existing landmarks are not
recalculated. Reset restores the displayed defaults; click Apply to activate them.
Settings last for the current application session.

For 1D gestures, each threshold is its percentage of the corresponding signed,
sign-normalized velocity peak magnitude: closing for GONS/NONS, opening for
NOFFS/GOFFS. For 2D displacement gestures, the level is speed at MaxC plus the
selected percentage of the rise from that speed to the corresponding speed peak.
The separate 2D peak-qualification fraction stays fixed at 20%. These settings
do not alter MaxC or PVEL selection, search windows, interpolation, or fallbacks.
The reopening deadband remains separate and applies to 1D excursion searches.

#### DTW search context

DTW-assisted 2D annotation uses a fixed 300-ms search radius on each side of
the preserved MaxC anchor, bounded by the available full-resolution data.
Zoom, viewport position, and display downsampling do not select this radius.
The 300-ms implementation default preserves existing wide-view behavior; it
is not a universally validated gestural duration. Manual 2D surrounding-landmark
searches retain their viewport-dependent radius (120-300 ms). The 1D signed
velocity algorithm is unchanged.

#### Review of automatic landmarks

DTW preserves the manager-refined MaxC time without a second snap. Automatically
generated landmarks require analyst review: small local fluctuations may be
eligible extrema, and a fixed window may include neighboring movement or exclude
a later crossing. Existing fallback rules can supply an anchor, peak or boundary
when its intended criterion is not found. Temporal ordering does not prove
scientific validity. No automatic annotation-quality warning system is included.
Coincident landmarks retain separate editing identities within the session,
including after file switches. IDs are not added to CSV or .span files.

#### Landmark table and export
The landmark table stores:
- landmark name
- channel
- time offset
- Y value at the landmark location

Click **Export Landmarks** to save the landmark table as CSV for downstream analysis.
For 2D displacement gestures, the companion tangential-values CSV provides speed
values separately from the positional Y values in the regular landmark table.

---

### 6. Templates and DTW-assisted annotation

1. Load a `.span` exemplar.
2. Place `MaxC` landmarks on the channels of interest.
3. Click **Save template** to save a template `.json` file.
4. Select the template and click **Apply** in the template section to transfer candidate landmarks to the current file.
5. Click **Apply to ...** in the template section to process compatible files in the exemplar directory.

Templates are intended for repeated annotation across structurally similar files. Candidate landmarks remain editable after transfer.

---

### 7. Spectrogram controls

Use the spectrogram controls to adjust:
- analysis window size
- maximum displayed frequency
- dynamic range

Click **Apply** to rebuild the spectrogram for the current visible range.
Click **Reset** to restore and apply the default spectrogram settings.

---

## Keyboard shortcuts

- `Ctrl+O` — Open `.span` files
- `Ctrl+Shift+P` — Open the preprocessing window
- `Ctrl+I` — File info
- `Space` — Start / stop audio playback (a new playback starts from the beginning)
- `Shift+Space` — Play current selection
- `Esc` — Stop playback
- `Ctrl++` / `Ctrl+-` — Zoom in / out
- `Ctrl+0` — Reset zoom
- `Ctrl+E` — Export landmark CSV
- `Ctrl+L` — Place landmarks from a selected CSV file
- `Ctrl+Shift+L` — Clear all landmarks
- `Ctrl+Shift+Up` / `Ctrl+Shift+Down` — Move config up / down
- `Delete` — Remove the selected entry when the Files, landmarks or configured-channel table has focus
- `Ctrl+Shift+I` — Invert selected configuration
- `Ctrl+Shift+A` — Apply configuration to all files
- `F1` — Help

---

## File formats

### Input
SPAN works with:
- raw AG50x `.pos` files
- `.wav` audio files
- previously created `.span` files
- template `.json` files

### Output
SPAN generates:
- `.span` files
- landmark `.csv` files
- template `.json` files

---

## Help

Help is available through the **Help** menu, `F1`, and widget tooltips.

---

## Third-party code

SPAN includes third-party code from **QCustomPlot**, which is used for 2D plotting and interactive visualization.

- **Component:** QCustomPlot
- **Author:** Emanuel Eichhammer
- **Website:** https://www.qcustomplot.com/
- **Version:** 2.1.1
- **License:** GNU General Public License v3 or later

The original copyright and license notice are preserved in qcustomplot.cpp and
qcustomplot.h. SPAN also bundles **dr_wav v0.13.16** by **David Reid**, used to read
WAV audio. Its public-domain / MIT No Attribution license alternatives and
copyright notice are preserved in dr_wav.h.

Additional build/runtime dependencies retain their own licenses:

| Dependency | Licensing reference |
|---|---|
| Qt, including Qt Data Visualization | Module-specific Qt licenses; Qt Data Visualization is available under GPLv3 or a commercial license: https://doc.qt.io/qt-6.10/qtdatavisualization-index.html |
| Eigen | Primarily MPL-2.0, with per-file notices for additional code: https://libeigen.gitlab.io/ |
| FFTW | GPLv2 or later for the open-source distribution: https://fftw.org/doc/License-and-Copyright.html |
| PortAudio, by Ross Bencina and Phil Burk | MIT license: https://portaudio.com/license.html |

These references summarize the dependencies and do not replace their full license
texts or required notices. Retain the applicable dependency documentation and
notices with distributed components, including the Windows runtime libraries.

---

## License

SPAN is distributed under the **GNU General Public License v3.0 or later**.

The full SPAN license is in LICENSE.txt. Preserve the original license headers and
notices in the bundled QCustomPlot and dr_wav source files; third-party components
retain their respective licenses.

---

## Contact

For bug reports, suggestions, or questions related to SPAN, contact:

`allen.shamsi@gmail.com`

## Automatic kinematic Y-axis titles

Axis titles describe the calculated quantity separately from the channel legend.
Recognized AG50xDATA_V002/V003 recordings preserve millimetre XYZ coordinates:
preprocessing applies rigid transformations, without a length-unit conversion.
Signed SUB differences and DIST Euclidean distances inherit these units. DIST
stores its scalar result in X; its other components are padding, not distances.
Velocity and acceleration use seconds (mm/s and mm/s squared). Multi-axis velocity
is tangential speed; multi-axis acceleration is (a dot v)/speed, not |a|.
Derivatives of distance are labeled Distance rate and Distance second derivative.
Position plots remain Position even when annotation uses hidden tangential speed.
A leading minus denotes display inversion. Plotted components may have vertical
offsets for readability; full-resolution cursor values retain the original data.
Runtime quantity metadata comes from the recognized source signature and SUB/DIST
recipes. Unrecognized provenance, unsupported components and arbitrary numeric
series receive Value (unit unspecified), rather than an assumed physical unit.
Orientation (phi/theta) and RMS fields are retained but cannot currently be selected
as kinematic plots; SPAN does not assign angular or RMS units to XYZ configurations.
The audio waveform Y-axis is labeled **Amplitude**: WAV input is
decoded to floats, stereo is averaged to mono, and samples are divided by the
recording's maximum absolute amplitude (silence remains zero). Loaded .span audio
is displayed as stored; externally edited files must preserve this convention.
Audio retains its Time (s) axis and spectrogram behavior is unchanged. Titles wrap,
and shared left margins keep audio and kinematic time axes aligned. Axis titles
are presentation metadata and do not change signal values or file formats.

Unit provenance: AG501 binary position coordinates are described as millimetres in
Marinelli et al. (2023), https://doi.org/10.3390/s23239511 (section 3.2).
External or edited files retaining an AG50x signature must preserve that convention;
the legacy .span format has no field for declaring an alternative physical unit.
