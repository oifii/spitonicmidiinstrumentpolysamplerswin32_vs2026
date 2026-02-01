# Reference: Command-line Arguments and Globals

This section describes how  reads command-line parameters at startup and maps them to global variables. These globals configure audio I/O, window appearance, font styling, and sample loading without touching the GUI.

## Command-line Argument Reference 📋

Each argument beyond the executable name (`argv[0]`) is tested against the total count (`nArgs`) and, if present, assigned to a corresponding global. Arguments 1–2 are reserved or used by legacy code; 3 through 24 configure audio, window, font, and sample settings. Any extra arguments are ignored.

| Arg # | Global Variable | Default | Description |
| --- | --- | --- | --- |
| 1–2 | Reserved | – | Reserved for future use or legacy filename parameters. |
| 3 | `mySPIAudioDevice.global_audiooutputdevicename` | `""` | Output device name (e.g., ASIO or WDM endpoint). |
| 4 | `mySPIAudioDevice.global_outputAudioChannelSelectors[0]` | `0` | ASIO channel selector for left (first) channel. |
| 5 | `mySPIAudioDevice.global_outputAudioChannelSelectors[1]` | `1` | ASIO channel selector for right (second) channel. |
| 6 | `global_x` | `100` | Initial window X position in pixels. |
| 7 | `global_y` | `200` | Initial window Y position in pixels. |
| 8 | `global_xwidth` | `400` | Initial window width in pixels. |
| 9 | `global_yheight` | `400` | Initial window height in pixels. |
| 10 | `global_alpha` | `200` | Window transparency (0 = invisible … 255 = opaque). |
| 11 | `global_titlebardisplay` | `1` | Show title bar (0 = hide, 1 = show). |
| 12 | `global_menubardisplay` | `0` | Show menu bar (0 = hide, 1 = show). |
| 13 | `global_acceleratoractive` | `1` | Enable keyboard accelerators (0 = off, 1 = on). |
| 14 | `global_fontheight` | `24` | Base font height in logical units. |
| 15 | `global_fontcolor_r` | `255` | Font red channel (0–255). |
| 16 | `global_fontcolor_g` | `255` | Font green channel (0–255). |
| 17 | `global_fontcolor_b` | `255` | Font blue channel (0–255). |
| 18 | `global_staticalignment` | `0` | Static text alignment (0 = left, 1 = center, 2 = right). |
| 19 | `szWindowClass` | `"spitonicmidiinstrumentpolysamplerswin32class"` | Window class name (wide string). |
| 20 | `szTitle` | `"spitonicmidiinstrumentpolysamplerswin32title"` | Window title (wide string). |
| 21 | `global_begin` | `"begin.ahk"` | Shell command to run at sample-trigger begin. |
| 22 | `global_end` | `"end.ahk"` | Shell command to run at sample-trigger end. |
| 23 | `global_samplesfolder` | `""` | Path to sample folder or text file listing sample paths. |
| 24 | `global_samplesfilter` | `""` | File-pattern filter for sample scanning (e.g., `*.wav`). |


### Parsing Snippet

```cpp
// In _tWinMain, after CommandLineToArgvA/W:
if (nArgs > 3)  mySPIAudioDevice.global_audiooutputdevicename = szArgList[3];
if (nArgs > 4)  mySPIAudioDevice.global_outputAudioChannelSelectors[0] =
                 atoi(szArgList[4]);
if (nArgs > 5)  mySPIAudioDevice.global_outputAudioChannelSelectors[1] =
                 atoi(szArgList[5]);
if (nArgs > 6)  global_x        = atoi(szArgList[6]);
if (nArgs > 7)  global_y        = atoi(szArgList[7]);
if (nArgs > 8)  global_xwidth   = atoi(szArgList[8]);
if (nArgs > 9)  global_yheight  = atoi(szArgList[9]);
if (nArgs > 10) global_alpha    = atoi(szArgList[10]);
if (nArgs > 11) global_titlebardisplay = atoi(szArgList[11]);
if (nArgs > 12) global_menubardisplay  = atoi(szArgList[12]);
if (nArgs > 13) global_acceleratoractive = atoi(szArgList[13]);
if (nArgs > 14) global_fontheight = atoi(szArgList[14]);
if (nArgs > 15) global_fontcolor_r = atoi(szArgList[15]);
if (nArgs > 16) global_fontcolor_g = atoi(szArgList[16]);
if (nArgs > 17) global_fontcolor_b = atoi(szArgList[17]);
if (nArgs > 18) global_staticalignment = atoi(szArgList[18]);
if (nArgs > 19) wcscpy(szWindowClass, szArgListW[19]);
if (nArgs > 20) wcscpy(szTitle,       szArgListW[20]);
if (nArgs > 21) global_begin = szArgList[21];
if (nArgs > 22) global_end   = szArgList[22];
if (nArgs > 23) global_samplesfolder = szArgList[23];
if (nArgs > 24) global_samplesfilter = szArgList[24];
```

> **Note:** Arguments beyond index 24 are silently ignored by the current parser.

*(assignment order simplified) *

### Global Defaults

All CLI-backed globals are declared and initialized with sensible defaults in the global scope:

```cpp
// Window & appearance
int   global_x             = 100;
int   global_y             = 200;
int   global_xwidth        = 400;
int   global_yheight       = 400;
BYTE  global_alpha         = 200;
int   global_titlebardisplay = 1;
int   global_menubardisplay  = 0;
int   global_acceleratoractive = 1;

// Font
int   global_fontheight    = 24;
BYTE  global_fontcolor_r   = 255;
BYTE  global_fontcolor_g   = 255;
BYTE  global_fontcolor_b   = 255;
int   global_staticalignment = 0;

// Audio device (via SPIAudioDevice)
SPIAudioDevice mySPIAudioDevice;  // audiooutputdevicename = "", channelSelectors = {0,1}

// Samples
string global_begin         = "begin.ahk";
string global_end           = "end.ahk";
string global_samplesfolder = "";
string global_samplesfilter = "";
```

*(excerpt) *

---

By adjusting these parameters on launch, you can tailor the sampler’s UI layout, audio routing, font styling, and sample directory without recompiling or using the GUI setup.