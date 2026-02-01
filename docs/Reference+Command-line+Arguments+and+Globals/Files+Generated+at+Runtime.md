# Reference: Command-line Arguments and Globals – Files Generated at Runtime

When you launch the sampler instrument, it automatically generates several plain-text log files. These files reside in the working directory and record how the application discovered audio devices, which sample modules it loaded, and how MIDI notes map to individual samples. Each file is designed for **human readability** and helps troubleshoot configuration or playback issues.

---

## Generated Files Overview

Below is a summary of the runtime files:

| File Name | Purpose | Contents Snapshot |
| --- | --- | --- |
| 📝 **devices.txt** | Audio device enumeration and selection log | `id=0, name=Microphone (Realtek High Definition Audio)`<br/>`Selected input device: 2` |
| 📝 **samples.txt** | Summary of how many sampler modules will load | `will load 3 sampler module(s)` |
| 📝 **samples_modidX.txt** | Detailed mapping for each sampler module X | `note 60 → piano_C4.wav (1.234 s)`<br/>`note 61 → piano_CS4.wav (1.098 s)` |
| ⚙️ **Additional logs** | Diagnostic output when debug flags are set | MIDI event traces, text overlays, recorded voice logs |


---

## devices.txt

This file logs all PortAudio devices detected and indicates which input/output device the app selects.

- **Creation**

Opened at startup via:

```cpp
  pFILE = fopen("devices.txt", "w");
  pFILE2 = fopen("samples.txt", "w");
```

- **Typical Contents**

```text
  id=0, name=Microphone (Realtek High Definition Audio)
  id=1, name=Line In (Realtek High Definition Audio)
  id=2, name=Speakers (Intel(R) Display Audio)
  Selected input device: 0
  Selected output device: 2
```

- **Use Case**
- Verify that your desired audio interface is detected.
- Confirm correct input/output selection when troubleshooting silence or feedback loops.

---

## samples.txt

This summary file reports how many sampler modules the application will load based on your `--samplesfolder` and `--samplesfilter` command-line parameters.

- **Creation**

After device selection, the code writes:

```cpp
  fprintf(pFILE2, "will load %d sampler module(s)\n", global_numberofsamplermodules);
```

- **Typical Contents**

```text
  will load 3 sampler module(s)
  **********************************************************************************************************************
  module id 0 on midi channel id 0
  loading samples in C:\Users\You\Samples\Piano
  **********************************************************************************************************************
  module id 1 on midi channel id 1
  loading samples in C:\Users\You\Samples\Bass
  **********************************************************************************************************************
  module id 2 on midi channel id 2
  loading samples in C:\Users\You\Samples\Drums
```

- **Use Case**
- Ensure the application found and is initializing the correct number of sample folders.
- Validate folder paths and file-filter patterns.

---

## samples_modidX.txt

For each sampler module (ID `X`), a corresponding `samples_modidX.txt` file is generated, detailing which MIDI notes map to which sample file and their durations.

- **Creation**

The startup routine loops through all possible module slots:

```cpp
  for (int i = 0; i < SPITMIPS_MAXNUMBEROFSAMPLERMODULES; i++) {
      char buf[32];
      sprintf(buf, "_modid%d", i);
      string fname = string("samples") + buf + ".txt";
      if (i < global_numberofsamplermodules)
          pFILEarray[i] = fopen(fname.c_str(), "w");
      else
          DeleteFileA(fname.c_str());
  }
```

- **Typical Contents (module 0)**

```text
  note 60 → piano_C4.wav (1.234 s)
  note 61 → piano_CS4.wav (1.098 s)
  note 62 → piano_D4.wav (1.276 s)
  …
```

- **Use Case**
- Inspect individual note-to-sample mappings.
- Verify sample duration and looping behavior.
- Cross-check expected MIDI channel assignments.

---

## Additional Diagnostic Logs

By setting the **mode** command-line argument (25th parameter) to include keywords like `DEBUG`, `SPITEXT`, `SPIVOICE`, or `SPIRECORD`, the application emits extra logs:

- **Enabling Debug Modes**

```cpp
  if (nArgs > 25) {
      global_modestring = szArgList[25];
      std::transform(...);
      if (global_modestring.find("DEBUG") != string::npos)
          global_mididebugmode = 1;
      …
  }
```

- **Generated Artifacts**
- **MIDI event traces** in the main window or console
- **Text overlays** via external AHK scripts
- **Recorded voice logs** when `SPIRECORD` is active

These diagnostic outputs complement the primary log files and are invaluable when diagnosing complex issues with real-time MIDI/Audio interaction.

---

## Viewing and Troubleshooting

- Open **.txt** files in any text editor (Notepad, VS Code, Sublime).
- Compare timestamps and error messages when things don’t behave as expected.
- Use the **mode** argument to enable richer diagnostics before reproducing an issue.

---

```card
{
    "title": "Tip",
    "content": "Always inspect devices.txt first to ensure audio hardware is recognized."
}
```

```card
{
    "title": "Best Practice",
    "content": "Regenerate logs after changing sample folders or filters to capture updated module info."
}
```