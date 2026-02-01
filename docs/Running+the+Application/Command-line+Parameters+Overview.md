# Running the Application – Command-line Parameters Overview

This section explains how to launch the Win32 MIDI sampler with custom settings via command-line arguments. Each positional argument configures a global variable before the GUI and audio engine initialize. Values outside documented ranges are **clamped** where appropriate.

```bash
spitonicmidiinstrumentpolysamplerswin32.exe \
  [MIDI_IN_DEVICE] [MIDI_CHANNEL] [AUDIO_OUT_DEVICE] \
  [ASIO_CH0] [ASIO_CH1] \
  [X] [Y] [WIDTH] [HEIGHT] [ALPHA] \
  [TITLE_BAR] [MENU_BAR] [ACCEL] \
  [FONT_HEIGHT] [FONT_R] [FONT_G] [FONT_B] \
  [ALIGN] [WIN_CLASS] [WIN_TITLE] \
  [BEGIN_CMD] [END_CMD] [SAMPLES_FOLDER] [FILE_FILTER]
```

---

## 1. MIDI Configuration 🎹

Configure which MIDI port and channel the sampler listens to.

- **Arg [1]: MIDI Input Device**

The name of the MIDI port (e.g., `"Q49"`, `"In From MIDI Yoke: 1"`).

Assigned to `global_inputmididevicename`.

- **Arg [2]: MIDI Channel**

The MIDI channel to listen on (0–15).

Special values:

- `-1`: all channels with existing sampler modules
- `-2`: map all channels to modules in round-robin
- `-3`: map all channels to a single module (togglable at runtime)

Out-of-range values are clamped to –3…15.

---

## 2. Audio Configuration 🔊

Select the audio output device and ASIO channel mapping.

- **Arg [3]: Audio Output Device Name**

Name of the PortAudio/ASIO device (e.g., `"E-MU ASIO"`).

Stored in `mySPIAudioDevice.global_audiooutputdevicename`.

- **Arg [4] & [5]: ASIO Channel Selectors**

Two integers selecting left/right ASIO channels.

Mapped to

```cpp
  global_outputAudioChannelSelectors[0] = atoi(argv[4]);
  global_outputAudioChannelSelectors[1] = atoi(argv[5]);
```

Defaults: `0, 1`.

---

## 3. Window Position & Size 🖥️

Define the initial placement and dimensions of the main window.

| Arg | Variable | Description | Default |
| --- | --- | --- | --- |
| 6 | `global_x` | Window X position (pixels) | 100 |
| 7 | `global_y` | Window Y position (pixels) | 200 |
| 8 | `global_xwidth` | Window width (pixels) | 400 |
| 9 | `global_yheight` | Window height (pixels) | 400 |


All are parsed with `atoi()` and assigned when `nArgs > 5`.

---

## 4. Window Appearance ⚙️

Toggle transparency and UI elements.

- **Arg [10]: Window Alpha**

Opacity (0–255) set to `global_alpha`.

- **Arg [11]: Title Bar Display**

`0` = hidden, `1` = shown (`global_titlebardisplay`).

- **Arg [12]: Menu Bar Display**

`0` = hidden, `1` = shown (`global_menubardisplay`).

- **Arg [13]: Accelerators Active**

`0` = disabled, `1` = enabled (`global_acceleratoractive`).

---

## 5. Font & Color 🎨

Customize sample name text appearance.

| Arg | Variable | Description | Default |
| --- | --- | --- | --- |
| 14 | `global_fontheight` | Font height (pixels) | 24 |
| 15 | `global_fontcolor_r` | Font color red (0–255) | 255 |
| 16 | `global_fontcolor_g` | Font color green (0–255) | 255 |
| 17 | `global_fontcolor_b` | Font color blue (0–255) | 255 |


Parsed when `nArgs > 13`.

---

## 6. Text Alignment & Window Class Overrides

Fine-tune static text alignment and override Win32 class/title.

- **Arg [18]: Text Alignment**

`0` = left, `1` = center, `2` = right (`global_staticalignment`).

- **Arg [19]: Window Class Name**

Wide-string override of `szWindowClass` via `wcscpy()`.

- **Arg [20]: Window Title**

Wide-string override of `szTitle` via `wcscpy()`.

---

## 7. Startup Commands 🔧

Execute external scripts or tools at launch and exit.

- **Arg [21]: Begin Command**

Shell command to run at startup (`global_begin`, default `"begin.ahk"`).

- **Arg [22]: End Command**

Shell command to run on exit (`global_end`, default `"end.ahk"`).

---

## 8. Sample Loading 📂

Specify where to load audio samples and which file types to include.

- **Arg [23]: Samples Folder**

Path to a folder or `.txt` list file (`global_samplesfolder`).

- **Arg [24]: File Filter**

Wildcard filter for sample files (e.g., `*.wav`) (`global_samplesfilter`).

---

## Notes & Clamping

- Any **numeric** argument out of its valid range is clamped to the nearest boundary (e.g., MIDI channel –3…15).
- **String** arguments exceeding expected lengths may be truncated by `CommandLineToArgvW/A`.
- Unrecognized or missing arguments fall back to built-in defaults.

---

**Example**

Launch on MIDI channel –1 (all), using ASIO device “My ASIO”, channels 2/3, window at (50,50), transparent, no title bar, font size 18, and load `C:\Samples` for `*.wav`:

```bash
spitonicmidiinstrumentpolysamplerswin32.exe \
  "My MIDI Port" -1 "My ASIO" 2 3 50 50 600 400 180 \
  0 0 0 18 255 200 200 1 \
  "MyClass" "My Title" \
  "start.bat" "stop.bat" \
  "C:\\Samples" "*.wav"
```

This will override all defaults and immediately begin loading your sample library on launch.