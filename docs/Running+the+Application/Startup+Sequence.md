## Running the Application – Startup Sequence

When you launch the sampler instrument, it performs a well-defined sequence to configure itself, load samples, and start audio processing. Below is a step-by-step breakdown of what happens under the hood.

### Parsing Command-Line Arguments

On startup, the app reads `argc`/`argv` to override global defaults:

- **Audio device name** (`global_audiooutputdevicename`)
- **Output channel selectors** (`global_outputAudioChannelSelectors[0..1]`)
- **Window position & size** (`global_x`, `global_y`, `global_xwidth`, `global_yheight`)
- **UI appearance flags** (`global_alpha`, `global_titlebardisplay`, `global_menubardisplay`, `global_acceleratoractive`)
- **Font & alignment** (`global_fontheight`, `global_fontcolor_r/g/b`, `global_staticalignment`)
- **Class & title strings** (`szWindowClass`, `szTitle`)
- **Begin/end script paths** (`global_begin`, `global_end`)
- **Sampler folder or list file** (`global_samplesfolder`)
- **Sample filename filter** (`global_samplesfilter`)
- **MIDI debug mode** (`global_mididebugmode`)

| Arg Index | Parameter | Affects Global Variable |
| --- | --- | --- |
| 3 | Audio output device name | global_audiooutputdevicename |
| 4 | Left/right ASIO channels | global_outputAudioChannelSelectors |
| 5–8 | Window geometry | global_x, global_y, global_xwidth, global_yheight |
| 9–14 | UI flags & colors | global_alpha … global_staticalignment |
| 19–20 | Window class & title | szWindowClass, szTitle |
| 21–22 | Startup/end scripts | global_begin, global_end |
| 23 | Samples folder or list | global_samplesfolder |
| 24 | Sample filename filter | global_samplesfilter |
| 25 | MIDI debug mode | global_mididebugmode |


Mapping code excerpt:

```cpp
if(nArgs>21){ global_begin   = szArgList[21]; }
if(nArgs>23){ global_samplesfolder = szArgList[23]; }
if(nArgs>25){ global_mididebugmode = szArgList[25]; }
LocalFree(szArgList);
LocalFree(szArgListW);
```

### Executing the Startup Script 🎬

Immediately after parsing, the application launches a user-provided script (`begin.ahk` by default). This lets users customize UI behavior or pre-load external tools.

```cpp
ShellExecuteA(
    NULL,
    "open",
    global_begin.c_str(),
    "",
    NULL,
    nCmdShow
);
```

### Seeding & Opening Log Files ✍️

To timestamp random operations and record device/sample info, the app:

1. **Seeds** the RNG with the current tick count.
2. **Opens** two log files in the working directory:
3. `devices.txt` (audio/MIDI device enumeration)
4. `samples.txt` (sample loading details)

```cpp
srand(GetTickCount());
pFILE  = fopen("devices.txt", "w");
pFILE2 = fopen("samples.txt", "w");
```

### Initializing PortAudio 🎧

The core audio engine, PortAudio, is brought online:

```cpp
global_err = Pa_Initialize();
if (global_err != paNoError) {
  if (pFILE) {
    fprintf(pFILE, "portaudio initialization failed.\n");
    fclose(pFILE);
  }
  return 1;
}
```

On failure, an error is logged in **devices.txt** and the app exits.

### Selecting Audio Devices 🔊

With PortAudio ready, the app enumerates and selects the user-specified:

- **Input device** via `SelectAudioInputDevice()`
- **Output device** via `SelectAudioOutputDevice()`

These wrappers scan available devices, match names/IDs, and populate `PaStreamParameters`.

### Configuring Tonic Sample Rate ⚙️

Tonic’s global sample rate is then set to match the audio stream:

```cpp
Tonic::setSampleRate(SAMPLE_RATE);
```

Failing to set this still defaults to 44 100 Hz, but explicit configuration ensures consistency.

### Determining Sampler Modules 📂

The application next decides which sample sources to load:

- If `global_samplesfolder` ends with `.txt`, it’s treated as a **list file**:
- Each line is a folder path.
- If more than the maximum modules are listed, random selection fills remaining slots.
- Otherwise, it uses a **single folder** for all modules.

```cpp
if (global_samplesfolder.rfind(".txt") != string::npos) {
  ifstream ifs(global_samplesfolder);
  while (getline(ifs, temp))
    global_samplesfolderschoice.push_back(temp);
  …
} else {
  global_samplesfolders.push_back(global_samplesfolder);
}
global_numberofsamplermodules = global_samplesfolders.size();
assert(global_numberofsamplermodules >= 1);
```

Log entry:

```text
will load X sampler module(s)
```

### Loading Samples into Each Module 🎹

For each module index, the app:

1. Logs a header to **samples.txt**.
2. Calls `**loadSynthSamples(moduleIndex)**` to scan the folder, apply pitch shifts, and build `Tonic::SampleTable` instances.
3. Registers voices via `poly[moduleIndex].addVoices(createSynthVoice, SPITMIPS_NUMBEROFVOICES)`.

```cpp
for (moduleIndex = 0; moduleIndex < global_numberofsamplermodules; ++moduleIndex) {
  fprintf(pFILE2, "loading samples in %s\n", global_samplesfolders[moduleIndex].c_str());
  loadSynthSamples(moduleIndex);
  poly[moduleIndex].addVoices(createSynthVoice, SPITMIPS_NUMBEROFVOICES);
}
```

### Starting the PortAudio Stream ▶️

With samples in place, the audio callback stream is opened and started:

```cpp
err = Pa_OpenStream(
  &global_stream,
  NULL,
  &global_outputParameters,
  SAMPLE_RATE,
  FRAMES_PER_BUFFER,
  paClipOff,
  renderCallback,
  NULL
);
if (err) { log and exit }

err = Pa_StartStream(global_stream);
if (err) { log and exit }
```

This hands off audio processing to the real-time callback.

### Entering the Win32 GUI Message Loop 🖥️

Finally, the application enters its standard Win32 message loop:

```cpp
MSG msg;
while (GetMessage(&msg, NULL, 0, 0)) {
  if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}
return (int)msg.wParam;
```

At this point, the sampler is fully operational and responds to MIDI events and GUI actions.