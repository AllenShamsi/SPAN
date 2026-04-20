# SPAN

SPAN is a desktop application for preprocessing, viewing, annotating, and exporting electromagnetic articulography (EMA) data together with synchronized audio.

It is designed for workflows that begin with raw AG50x `.pos` and `.wav` recordings and continue through correction, visualization, landmark annotation, template-based labeling, and CSV export.

## What SPAN does

SPAN supports the following main tasks:

- preprocessing raw AG50x bite-plane and trial recordings into `.span` files
- loading and inspecting `.span` files
- plotting kinematic channels and derived channels
- visualizing audio, kinematic traces, and 3D sensor layouts
- placing, editing, and exporting gestural landmarks
- saving and applying annotation templates
- rebuilding spectrograms with user-controlled display settings

## Main workflow

A typical SPAN workflow looks like this:

1. preprocess raw AG50x recordings into `.span`
2. open `.span` files in the main window
3. configure channels for plotting
4. create derived channels if needed
5. place or adjust landmarks
6. export landmark tables to CSV
7. optionally save and apply templates for repeated annotation workflows

---

## Quick start

## 1. Pre-process raw AG50x data into `.span`

Open the **Pre-processing Window**.

1. Click **Select BP File (.pos)** and choose the bite-plane POS file.
2. Click **Select Trial Files** and choose matching trial `.pos` and `.wav` files.
3. In **Sensor Layout**, check each channel you want to use.
4. Name the three reference channels exactly:
   - `REF-L`
   - `REF-R`
   - `REF-F`
5. Name all other checked channels as desired.
6. In **Bite Plane Channels**, assign three non-reference channels to:
   - `BP-L`
   - `BP-F`
   - `BP-R`
7. Click **Start** to compute and preview the corrected bite-plane view.
8. Click **Proceed** once to preview corrected trial data.
9. Click **Proceed** again to export `.span` files.

### Notes
- Trial `.pos` and `.wav` files must have matching base names.
- The three reference labels must appear exactly as `REF-L`, `REF-R`, and `REF-F`.
- The bite-plane mapping must use three different non-reference channels.

## 2. Open and inspect `.span` files

1. In the main SPAN window, click **Open**.
2. Select one or more `.span` files.
3. Double-click a file or click **View** to load it.
4. Use the audio panel, sensor plots, landmark table, and 3D sensor view to inspect the file.

## 3. Configure channels for plotting

1. Choose a sensor or derived signal.
2. Select the motion type:
   - displacement
   - velocity
   - acceleration
3. Choose the dimensions to include:
   - X
   - Y
   - Z
4. Click **Add Config** to add the channel to the plot stack.
5. Use:
   - **Move Up**
   - **Move Down**
   - **Update**
   - **Remove**
   - **Apply to All**

to manage the configuration list.

## 4. Create derived channels

1. Choose two input channels.
2. Enter a new derived-channel name.
3. Choose an operation, such as:
   - subtract component
   - Euclidean distance
4. Click **Derive**.
5. Use **Clear All Derived** to remove all derived channels and reset the recipe line.

Derived channels are useful for inter-articulator relationships, constriction proxies, and other custom kinematic measures.

## 5. Landmark annotation

### Basic actions
- **Right-click** on a plot to place a plain label.
- **Shift + right-click** on a single-channel displacement trace to snap to the nearest local extremum and place `MaxC`.
- When precomputed velocity is available, SPAN can also derive additional landmarks such as:
  - `GONS`
  - `NONS`
  - `PVEL1`
  - `PVEL2`
  - `NOFFS`
  - `GOFFS`
- Drag a landmark handle to move it.
- Select landmarks in the landmark table to review or remove them.

### Landmark table and export
The landmark table stores:
- landmark name
- channel
- time offset
- Y value at the landmark location

Click **Export CSV** to save the landmark table for downstream analysis.

## 6. Templates / DTW

1. Load a `.span` exemplar.
2. Place `MaxC` landmarks on the channels of interest.
3. Click **Save Template** to save a template JSON.
4. Use **Apply Template** for the current file.
5. Use **Batch Apply Template** to process multiple files.

Templates are intended to speed up repeated annotation across similar files.

## 7. Spectrogram controls

Use the spectrogram controls to change:
- analysis window size
- view max frequency
- dynamic range

Click **Apply** to rebuild the spectrogram for the current visible range.  
Click **Defaults** to restore recommended defaults.

---

## Useful shortcuts

- `Ctrl+O` — Open `.span` files
- `Ctrl+I` — File info
- `Space` — Play / pause audio
- `Shift+Space` — Play current selection
- `Esc` — Stop playback
- `Ctrl++` / `Ctrl+-` — Zoom in / out
- `Ctrl+0` — Reset zoom
- `Ctrl+E` — Export CSV
- `Ctrl+Shift+L` — Clear all landmarks
- `Ctrl+Shift+Up / Down` — Move config up / down
- `Ctrl+Delete` — Remove config
- `Ctrl+Shift+I` — Invert selected configuration
- `Ctrl+Shift+A` — Apply configuration to all files
- `F1` — Help

---

## Main interface overview

### Pre-processing window
The pre-processing window is used to convert raw AG50x recordings into `.span` files. It includes:
- bite-plane file selection
- trial file selection
- sensor layout assignment
- bite-plane channel mapping
- 3D preview of corrected sensor positions
- export of corrected `.span` files

### Main window
The main window is used for:
- file loading
- audio playback
- waveform viewing
- kinematic plotting
- 3D sensor inspection
- landmark placement and editing
- template creation and application
- CSV export

---

## Input and output formats

### Input
SPAN works with:
- raw AG50x `.pos` files
- `.wav` audio files
- previously created `.span` files
- template `.json` files

### Output
SPAN can generate:
- `.span` files
- landmark `.csv` files
- template `.json` files

---

## Help inside the app

SPAN includes built-in help and tooltips in both the main window and the preprocessing window.

Use:
- the **Help** menu
- **F1**
- widget tooltips

for quick reminders while working.

---

## Third-party code

SPAN includes third-party code from **QCustomPlot**, which is used for 2D plotting and interactive plotting functionality in the application. This is part of the current plotting stack used by the visualizer classes and plot-based annotation workflow.  

- Component: QCustomPlot
- Author: Emanuel Eichhammer
- Website: https://www.qcustomplot.com/
- Version: 2.1.1
- License: GNU General Public License v3 or later

The original copyright and license notice are preserved in the bundled QCustomPlot source files. 

---

## License

SPAN is distributed under the GNU General Public License v3.0 or later.

This repository also includes bundled third-party code from QCustomPlot.
See the Third-party code section above, and preserve the original license
headers in the bundled source files.

---

## Contact

For bug reports, suggestions, or questions related to SPAN, contact:

`allen.shamsi@gmail.com`
