# Installation – Prepare Sample Libraries

Before running the sampler, you must supply one or more **sample libraries**—folders of audio files or a text file listing multiple folders. Each entry becomes a separate sampler module, mapped to a MIDI channel and played polyphonically.

## Organizing Sample Folders 📁

Create or locate folders containing your `.wav` samples. Each folder represents one **sampler module**.

- Name folders clearly (e.g. `Piano`, `Drums`, `Strings`).
- Inside each folder, place one or more `.wav` files named to indicate their MIDI note (e.g. `C4.wav`, `D#4.wav`).
- The application will scan each folder for files matching `*.wav` by default .

**Recommended structure:**

| Folder Path | Purpose |
| --- | --- |
| `C:\Samples\Piano\` | Grand piano samples mapped to MIDI notes |
| `C:\Samples\DrumKit\` | Drum hits (kick, snare, hi-hat) |
| `C:\Samples\Guitar\Acoustic\` | Acoustic guitar samples |


## Using a Sample-List Text File 📄

Instead of a single folder, you can pass a **text file** listing multiple sample-folder paths, one per line. This file allows you to:

- Combine diverse libraries in one session.
- Randomize module selection if you list more folders than allowed.

**Example **`**libraries.txt**`**:**

```txt
C:\Samples\Piano
C:\Samples\DrumKit
D:\MyLoops\AmbientPads
```

When the app detects a `.txt` extension, it reads each line into `global_samplesfolderschoice` and then:

```cpp
if (global_samplesfolder.rfind(".txt") != string::npos) {
  // Read each line as a folder path
  while (getline(ifs, temp)) {
    global_samplesfolderschoice.push_back(temp);
  }
  // If more than max modules, pick random subset
  if (global_samplesfolderschoice.size() > SPITMIPS_MAXNUMBEROFSAMPLERMODULES) {
    while (global_samplesfolders.size() < SPITMIPS_MAXNUMBEROFSAMPLERMODULES) {
      global_samplesfolders.push_back(
        global_samplesfolderschoice[
          RandomInt(0, global_samplesfolderschoice.size() - 1)
        ]
      );
    }
  } else {
    global_samplesfolders = global_samplesfolderschoice;
  }
} else {
  global_samplesfolders.push_back(global_samplesfolder);
}
```

## Module Limit & Random Selection 🎲

The constant `**SPITMIPS_MAXNUMBEROFSAMPLERMODULES**` defines the maximum modules the sampler will load in one session. If your text file lists more folders, the app picks a random subset up to that limit .

| Constant | Default Value | Description |
| --- | --- | --- |
| `SPITMIPS_MAXNUMBEROFSAMPLERMODULES` | 16 | Maximum sampler modules per session |


## Next Steps

> **Tip:** To change this limit, adjust the macro in `PolySynth_main.cpp` before building.

1. Place your sample folders or sample-list text file in an accessible location.
2. Pass the folder path (or `libraries.txt`) as the **samples folder parameter** when launching the app.
3. The app will display in `samples.txt` how many modules it will load.

Once modules are loaded, each sampler module maps to a MIDI channel (0–15). You can now configure your MIDI input device, select the desired MIDI channel, and start playing your custom sampler instrument.