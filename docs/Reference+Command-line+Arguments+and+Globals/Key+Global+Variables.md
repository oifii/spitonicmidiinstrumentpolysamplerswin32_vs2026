# Reference: Command-Line Arguments and Globals – Key Global Variables

This section details the command-line options and global variables that configure the sampler’s behavior at startup and runtime. Understanding these settings helps you customize MIDI routing, audio I/O, sample loading, window layout, and performance on Windows desktop systems.

## 🎛️ Command-Line Arguments

Each argument in the startup command maps directly to a global variable. Maintain the order below when launching the executable:

| Position | Argument | Variable | Description |
| --- | --- | --- | --- |
| ---------: | -------------------------------------- | ------------------------------------- | --------------------------------------------------------------------------------------------------------------- |
| 1 | `<MIDI Device Name>` | `global_inputmididevicename` | Name of the MIDI input port (e.g., “Q49”, “loopMIDI Port 1”). |
| 2 | `<MIDI Channel>` | `global_inputmidichannel` | MIDI channel to listen on (0–15). Special: -1=all modules, -2=map by channel, -3=random. |
| 3 | `<Audio Output Device Name>` | `global_audiooutputdevicename` | ASIO/WDM device for audio output. |
| 4 | `<ASIO Channel Left>` | `global_outputAudioChannelSelectors[0]` | ASIO channel index for left output (0,2,4…). |
| 5 | `<ASIO Channel Right>` | `global_outputAudioChannelSelectors[1]` | ASIO channel index for right output (1,3,5…). |
| 6 | `<Window X>` | `global_x` | Initial window X position in pixels. |
| 7 | `<Window Y>` | `global_y` | Initial window Y position in pixels. |
| 8 | `<Window Width>` | `global_xwidth` | Window width in pixels. |
| 9 | `<Window Height>` | `global_yheight` | Window height in pixels. |
| 10 | `<Window Alpha>` | `global_alpha` | Window transparency (0-255). |
| 11 | `<Title Bar On>` | `global_titlebardisplay` | Show title bar? 1=Yes, 0=No. |
| 12 | `<Menu Bar On>` | `global_menubardisplay` | Show menu bar? 1=Yes, 0=No. |
| 13 | `<Accelerators On>` | `global_acceleratoractive` | Enable keyboard accelerators? 1=Yes, 0=No. |
| 14 | `<Font Height>` | `global_fontheight` | Main font size in pixels. |
| 15 | `<Font Color R>` | `global_fontcolor_r` | Font red component (0-255). |
| 16 | `<Font Color G>` | `global_fontcolor_g` | Font green component (0-255). |
| 17 | `<Font Color B>` | `global_fontcolor_b` | Font blue component (0-255). |
| 18 | `<Static Alignment>` | `global_staticalignment` | 0=Left, 1=Center, 2=Right alignment for static text. |
| 19 | `<Window Class Name>` | `szWindowClass` | Custom Win32 window class. |
| 20 | `<Window Title>` | `szTitle` | Custom window title. |
| 21 | `<Start Script>` | `global_begin` | Script or batch run at launch (e.g., “begin.ahk”). |
| 22 | `<End Script>` | `global_end` | Script or batch run on close (e.g., “end.ahk”). |
| 23 | `<Samples Folder>` | `global_samplesfolder` | Root directory for sample subfolders. |
| 24 | `<Samples Filter>` | `global_samplesfilter` | Wildcard pattern for sample files (e.g., “*.wav”). |
| 25 | `<Mode String>` | `global_modestring` | Comma-separated flags: DEBUG, SPITEXT, SPIVOICE, SPIRECORD. |
| 26 | `<CPU Cores>` | `global_cores` | Thread count for sample loading. |
| 27 | `<Min Loop Duration (s)>` | `global_loopsamples_tominimum_s` | Loop short samples to ≥ this length. |
| 28 | `<Reverse Every Other>` | `global_reverseeveryothersample` | Boolean (0/1) to alternate sample direction. |


**Example**

```bash
spitonicmidiinstrument.exe "Q49" 0 "E-MU ASIO" 0 1 100 200 800 600 200 1 0 1 24 255 255 255 0 \
"MyClass" "Spitonic Sampler" "begin.bat" "end.bat" "./Samples" "*.wav" DEBUG 4 -1.0 0
```

---

## 🔧 Global Variables

Below are the most critical globals you may adjust via code or command line. Defaults reflect typical sampler setups.

### Sampling & Module Configuration

| Name | Type | Default / Range | Purpose |
| --- | --- | --- | --- |
| **SPITMIPS_MAXNUMBEROFSAMPLERMODULES** | `#define` | `16` | Maximum sampler modules loaded into memory. |
| **SPITMIPS_NSAMPLES** | `const int` | `128` | Number of MIDI notes per module (0–127). |
| **SPITMIPS_NUMBEROFVOICES** | `const int` | `16` | Polyphony per module. |
| **global_samplesfolder** | `string` | `"."` | Root path for sample subfolders. |
| **global_samplesfilter** | `string` | `"*.wav"` | File pattern for sample loading. |
| **global_samplesfolders** | `vector<string>` | Empty | Discovered subfolders under `global_samplesfolder`. |
| **global_numberofsamplermodules** | `int` | `-1` | Actual number of modules detected/loaded. |
| **global_samplefilenames** | `vector<string>[modules]` | Empty lists | Full paths of each sample per module. |
| **global_sampleduration_s** | `float[modules][128]` | Zero-initialized | Duration in seconds of each sample. |


### MIDI Routing & Transposition

| Name | Type | Default / Notes | Purpose |
| --- | --- | --- | --- |
| **global_inputmidichannel** | `int` | `0` | MIDI channel filter. |
| **global_midichanneltosamplermoduleremapping** | `int[modules]` | Identity mapping | Map incoming channels to sampler indices. |
| **global_numberofoctavetoshift** | `int[modules]` | Zero (no shift) | Per-module octave transposition. |
| **global_modestring** | `string` | `""` | Mode flags for debug/text/voice/record. |
| **global_audiomidi_devices** | `string` | `"disconnected"` | Tracks connection state of audio/MIDI I/O. |


### UI & Window Appearance

| Name | Type | Default | Description |
| --- | --- | --- | --- |
| **global_x**, **global_y** | `int` | `100`, `200` | Window top-left coordinates. |
| **global_xwidth**, **global_yheight** | `int` | `400`, `400` | Window dimensions in pixels. |
| **global_alpha** | `BYTE` | `200` | Window transparency (0 fully transparent, 255 opaque). |
| **global_titlebardisplay** | `int` | `1` | Show title bar: 1=Yes, 0=No. |
| **global_menubardisplay** | `int` | `0` | Show menu bar: 1=Yes, 0=No. |
| **global_acceleratoractive** | `int` | `1` | Enable key accelerators: 1=Yes, 0=No. |
| **global_fontheight** | `int` | `24` | Main font size in pixels. |
| **global_fontcolor_r/g/b** | `BYTE` | `255,255,255` | Font color components. |
| **global_staticalignment** | `int` | `0` | Static text alignment: 0=Left,1=Center,2=Right. |


---

> **Tip:** Always supply all required arguments in order. Missing or mis-ordered parameters can lead to default behavior that may not match your intended MIDI or UI setup.