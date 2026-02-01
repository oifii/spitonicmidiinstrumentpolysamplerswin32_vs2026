# Running the Application – Basic Launch

After you build the project, you can launch the sampler from its Debug or Release output folder. The application reads all its configuration from **positional** command-line arguments (up to at least 29). At minimum, you must supply your audio output device name and the sample folder (or a text file listing sample folders) when you launch the executable.

## Locating the Executable

- Navigate to your build folder:
- `…\spitonicmidiinstrumentpolysamplerswin32_vs2026\Debug\`
- or `…\Release\`
- Find the executable named:

```bash
  spitonicmidiinstrumentpolysamplerswin32.exe
```

## Invoking the Executable

You run the sampler by supplying a series of positional parameters. Here’s the minimal form, assuming you leave other parameters blank (`""`) until you reach the sample folder and filter:

```bash
spitonicmidiinstrumentpolysamplerswin32.exe "" "" "Your ASIO Device Name" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "C:\MySamples" "*.wav"
```

- The **3rd** argument is **Audio Output Device Name**.
- The **23rd** argument is **Sample Folder** (or a `.txt` list file).
- The **24th** argument is **Sample Filter** (e.g. `*.wav`).

All intermediate arguments can be empty strings (`""`) to use defaults.

## Command-Line Arguments Reference

The parser in `_tWinMain` maps `argv[1]…argv[n]` to configuration fields. Up to at least 29 parameters control MIDI, audio, UI, and sample loading .

| Index | Target Variable | Type | Description |
| --- | --- | --- | --- |
| 1 | global_inputmididevicename | string | MIDI input device name (e.g. `"In From MIDI Yoke: 1"`) |
| 2 | global_inputmidichannel | int | MIDI channel to listen (`-3` to `15`, `-1`=all modules) |
| 3 | mySPIAudioDevice.global_audiooutputdevicename | string | Audio output device name (e.g. `"E-MU ASIO"`) |
| 4 | mySPIAudioDevice.global_outputAudioChannelSelectors[0] | int | ASIO channel index for left output |
| 5 | mySPIAudioDevice.global_outputAudioChannelSelectors[1] | int | ASIO channel index for right output |
| 6 | global_x | int | Window X position (pixels) |
| 7 | global_y | int | Window Y position (pixels) |
| 8 | global_xwidth | int | Window width (pixels) |
| 9 | global_yheight | int | Window height (pixels) |
| 10 | global_alpha | int | Window transparency (0–255) |
| 11 | global_titlebardisplay | int | Show title bar (`1`) or hide (`0`) |
| 12 | global_menubardisplay | int | Show menu bar (`1`) or hide (`0`) |
| 13 | global_acceleratoractive | int | Enable accelerator keys (`1`) or disable (`0`) |
| 14 | global_fontheight | int | UI font height (pixels) |
| 15 | global_fontcolor_r | int | UI font color – red component (`0–255`) |
| 16 | global_fontcolor_g | int | UI font color – green component (`0–255`) |
| 17 | global_fontcolor_b | int | UI font color – blue component (`0–255`) |
| 18 | global_staticalignment | int | Static text alignment (left/center/right index) |
| 19 | szWindowClass | wide string | Custom window class name (overrides default) |
| 20 | szTitle | wide string | Custom window title (overrides default) |
| 21 | global_begin | string | Shell command or script to run at startup |
| 22 | global_end | string | Shell command or script to run before exit |
| 23 | global_samplesfolder | string | Path to sample folder **or** `.txt` file listing sample folders |
| 24 | global_samplesfilter | string | File filter for samples (e.g. `*.wav`, `*.mp3`) |
| 25 | global_modestring | string | Mode flags: `"DEBUG"`, `"SPITEXT"`, `"SPIVOICE"`, `"SPIRECORD"` (comma-separated) |
| 26 | global_cores | int | Number of threads to use (auto-clamped to hardware concurrency) |
| 27 | global_loopsamples_tominimum_s | float | Minimum sample loop duration (seconds); `<=0` = disabled |
| 28 | global_reverseeveryothersample | bool | Reverse every other loaded sample to avoid glitches (`1` = enabled) |


## Example: Complete Launch

> **Note:** You can omit trailing parameters; defaults will apply.

Here’s a full example with most parameters set:

```bash
spitonicmidiinstrumentpolysamplerswin32.exe \
  "In From MIDI Yoke: 1" \        # 1: MIDI device
  0 \                             # 2: MIDI channel 0
  "E-MU ASIO" \                   # 3: ASIO device
  0 1 \                           # 4–5: ASIO left=0, right=1
  100 100 \                       # 6–7: Window X=100, Y=100
  800 600 \                       # 8–9: Width=800, Height=600
  200 \                           # 10: Semi-transparent (alpha=200)
  1 1 1 \                         # 11–13: Title, menu, accel = on
  14 255 255 255 2 \              # 14–18: Font=14px white; alignment=2
  "MyWinClass" "My Sampler" \     # 19–20: Custom class & title
  "setup.bat" "teardown.bat" \    # 21–22: Begin/end scripts
  "C:\Samples\MyKit" "*.wav"      # 23–24: Sample folder & filter
```

## Tips & Best Practices

- Use **empty strings** (`""`) for any parameters you wish to skip.
- If `global_samplesfolder` ends in `.txt`, the app treats it as a **list file**.
- Launch from a batch script to fix most settings once.
- Monitor the sampler output folder for debug logs:
- `devices.txt`
- `<timestamp>_samplers.txt`

```card
{
    "title": "Quick Setup",
    "content": "Specify your ASIO device and sample folder to get started within seconds."
}
```

Enjoy your polyphonic MIDI sampler!