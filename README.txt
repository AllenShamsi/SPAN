# SPAN

SPAN is a desktop application for preprocessing, visualizing, annotating, and exporting electromagnetic articulography (EMA) data together with synchronized audio.

It is designed for workflows that begin with raw AG50x `.pos` and `.wav` recordings and continue through spatial correction, visualization, channel configuration, landmark annotation, and CSV export.

---

## Main workflow

A typical SPAN workflow is:

1. Convert raw bite-plane and trial recordings into `.span` files
2. Open `.span` files in the main window
3. Configure kinematic channels for plotting
4. Create derived channels if needed
5. Place, inspect, adjust, and export landmarks
6. Optionally load landmark positions from a `.csv` file onto the current `.span` file

---

## Quick start

## 1. Preprocess raw AG50x data into `.span`

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

### Notes
- Trial `.pos` and `.wav` files must have matching base names.
- The reference labels must appear exactly as `REF-L`, `REF-R`, and `REF-F`.
- The bite-plane mapping must use three non-reference channels.

---

## 2. Open and inspect files

In the main window, click **Add Files** to load one or more `.span` and/or `.csv` files.

- Double-click a `.span` file, or select it and click **View**, to load it
- Double-click a `.csv` file to place labels on the currently loaded `.span` file
- Use **File Info** to inspect metadata for the currently loaded `.span` file

When a `.span` file is loaded, SPAN displays:

- the audio waveform
- a spectrogram
- configured kinematic plots
- a landmark table
- a 3D sensor view

---

## 3. Configure channels for plotting

Use the configuration panel to choose which channels to display.

1. Select a sensor from the channel list.
2. Choose a motion type:
   - displacement
   - velocity
   - acceleration
3. Choose the displayed dimensions:
   - X
   - Y
   - Z
4. Click **Add Config** to add the channel to the plot stack.

You can then manage the configuration list with:

- **Move Up**
- **Move Down**
- **Update**
- **Remove**
- **Apply to All**
- **Invert**

### Notes
- Configuration settings are stored in the `.span` file.
- Updating a configuration changes how the signal is displayed, not the underlying stored positional data.
- **Apply to All** writes the current configuration list to all loaded `.span` files.

---

## 4. Create derived channels

SPAN supports simple derived channels for custom measurements.

1. Choose two input channels.
2. Enter a name for the derived channel.
3. Select an operation:
   - **Subtract a component**
   - **Compute Euclidean distance**
4. Click **Derive**.

Use **Clear All Derived** to remove all derived channels and reset the stored recipe.

Derived channels can be useful for inter-articulator relationships, constriction proxies, and custom kinematic measures.

---

## 5. Landmark annotation

Landmarks can be placed directly on the plots and reviewed in the landmark table.

### Basic actions
- Place landmarks directly on plots
- Select landmarks in the landmark table to highlight them in the plots
- Drag landmarks to adjust their timing
- Rename landmarks in the landmark table
- Remove selected landmarks with **Remove Landmark**
- Remove all landmarks with **Clear All Landmarks**

### Landmark table
The landmark table stores:

- landmark name
- channel
- time offset
- Y value at the landmark location

### CSV export
Click **Export CSV** to save the current landmark table as a `.csv` file.

---

## 6. Load landmarks from CSV

SPAN can place landmarks from a `.csv` file onto the currently loaded `.span` file.

Expected CSV header:

```text
Name,Channel,Offset
```

Each valid row is matched against the currently displayed channel names. Rows that do not match a current channel, or that duplicate an existing landmark at the same location, are ignored.

---

## 7. Playback and navigation

SPAN supports synchronized audio playback and plot navigation.

### Audio
- **Play Sound** plays the full audio signal
- **Play Selection** plays the current selected time region
- **Stop** stops playback

While audio is playing, the 3D sensor view updates over time.

### Navigation
Use the plot controls to:

- zoom in
- zoom out
- zoom to selection
- reset the full visible range
- scroll horizontally through the signal

You can also choose which plotted parameter is tracked in the display:
- X
- Y
- Z

---

## File formats

## Input
SPAN works with:

- raw AG50x `.pos` files
- `.wav` audio files
- previously created `.span` files
- landmark `.csv` files

## Output
SPAN generates:

- `.span` files
- landmark `.csv` files

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
