# SPAN

SPAN is a desktop application for preprocessing, visualizing, annotating, and exporting electromagnetic articulography (EMA) data together with synchronized audio.

It is intended for workflows that begin with raw AG50x `.pos` and `.wav` recordings and continue through spatial correction, visualization, landmark annotation, template-based labeling, and CSV export.

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

1. Click **Select BP File (.pos)** and choose the bite-plane file.
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
7. Click **Start** to compute and preview the corrected bite-plane view.
8. Click **Proceed** once to preview corrected trial data.
9. Click **Proceed** again to export `.span` files.

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
4. Click **Add Config** to add the channel to the plot stack.
5. Use the following controls to manage the configuration list:
   - **Move Up**
   - **Move Down**
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
5. Use **Clear All Derived** to remove all derived channels and reset the recipe line.

Derived channels are useful for inter-articulator relationships, constriction proxies, and other custom kinematic measures.

---

### 5. Landmark annotation

#### Basic actions
- **Right-click** on a plot to place a plain landmark.
- **Shift + right-click** on a single-channel displacement trace to snap to the nearest local extremum and place `MaxC`.
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

#### Landmark table and export
The landmark table stores:
- landmark name
- channel
- time offset
- Y value at the landmark location

Click **Export CSV** to save the landmark table for downstream analysis.

---

### 6. Templates and DTW-assisted annotation

1. Load a `.span` exemplar.
2. Place `MaxC` landmarks on the channels of interest.
3. Click **Save Template** to save a template `.json` file.
4. Use **Apply Template** to transfer candidate landmarks to the current file.
5. Use **Batch Apply Template** to process multiple files.

Templates are intended for repeated annotation across structurally similar files. Candidate landmarks remain editable after transfer.

---

### 7. Spectrogram controls

Use the spectrogram controls to adjust:
- analysis window size
- maximum displayed frequency
- dynamic range

Click **Apply** to rebuild the spectrogram for the current visible range.  
Click **Defaults** to restore the recommended settings.

---

## Keyboard shortcuts

- `Ctrl+O` — Open `.span` files
- `Ctrl+I` — File info
- `Space` — Play / pause audio
- `Shift+Space` — Play current selection
- `Esc` — Stop playback
- `Ctrl++` / `Ctrl+-` — Zoom in / out
- `Ctrl+0` — Reset zoom
- `Ctrl+E` — Export CSV
- `Ctrl+Shift+L` — Clear all landmarks
- `Ctrl+Shift+Up` / `Ctrl+Shift+Down` — Move config up / down
- `Ctrl+Delete` — Remove config
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

The original copyright and license notice are preserved in the bundled source files.

---

## License

SPAN is distributed under the **GNU General Public License v3.0 or later**.

This repository also includes bundled third-party code from QCustomPlot. Please preserve the original license headers in the bundled source files.

---

## Contact

For bug reports, suggestions, or questions related to SPAN, contact:

`allen.shamsi@gmail.com`
