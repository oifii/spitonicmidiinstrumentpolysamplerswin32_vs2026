# Pitch Shifting and Advanced Sample Processing – Looping and Reverse Samples

This section explains how the sampler engine handles pitch shifting, looping of short samples, and reversing to avoid glitches. You’ll learn about key configuration flags, internal mechanisms, and log files that help verify your sample mappings.

## Looping Short Samples ⥂

When a sample is shorter than a specified minimum duration, the engine can automatically loop it:

- **Flag**: `global_loopsamples_tominimum_s`

Defines the minimum sample duration (in seconds). A value > 0 enables looping .

- **Reverse Option**: `global_reverseeveryothersample`

When looping non-seamless samples, this reverses every other loop to reduce clicks .

- **Loop Logic**:

In `createTonicSampleTable()`, if the sample is too short, it invokes `LoopSample()` with these flags .

```cpp
WavSet myLoopedWavSet;
if ((global_loopsamples_tominimum_s > 0.0f)
 && (global_loopsamples_tominimum_s > pWavSet->GetWavSetLength())) {
  bool ceilloopduration = true;
  float actualDuration =
    myLoopedWavSet.LoopSample(
      pWavSet,
      global_loopsamples_tominimum_s,
      -1.0f,       // loop start offset
       0.0f,       // loop end offset
       ceilloopduration,
       global_reverseeveryothersample
    );
  pWavSet = &myLoopedWavSet;
}
```

## Pitch Shifting ♫

To fill missing MIDI-note gaps, the engine pitch-shifts nearby samples:

- **Data Structures**
- `global_suppliedmidinotes[module][stage]`: original notes loaded
- `global_pitchshiftedmidinotes[module][stage]`: notes created via pitch shift
- **Function**: `pitchshift(moduleIndex, targetNote, referenceNote)`

Calculates semitone difference and applies FFT-based shifting using `smbPitchShift_threadsafe()`  .

- **Multi-stage Filling**

Repeats up to 10 stages, each time expanding the set of available notes until all 128 MIDI notes have buffers.

## Handling Missing Samples – Silence Fallback

If no sample is available or pitch shifting cannot fill a note, the engine substitutes a silence buffer:

- **Counter**: `global_numberofmidinotes_silenced[module]`

Incremented per missing note when loading a 10 s silence file .

- **Warning Logs**

Each silenced note is logged as:

```text
  warning, sample silence for midinote <N>
```

in `samples_modidX.txt` .

## Log Files for Verification 📄

After loading, each sampler module writes a debug file named `samples_modid<X>.txt`:

| Filename | Purpose |
| --- | --- |
| samples_modid0.txt | Module 0 mapping, warnings, summaries |
| samples_modid1.txt | Module 1 mapping, warnings, summaries |
| … | … |


At the end of loading, you’ll see lines like:

```text
module index 0, global_numberofmidinotes_silenced 12
```

This summarizes how many notes fell back to silence for each module .

## Configuring Loop & Reverse via Command Line

You can override the defaults at runtime by appending two parameters:

```bash
spitonicmidi.exe "<SampleFolder>" "<Filter>" <LoopMinSeconds> <ReverseFlag>
```

- `<LoopMinSeconds>`: float ≥ 0 (e.g., 2.0 to loop samples shorter than 2 s)
- `<ReverseFlag>`: 0 (off) or 1 (on)

Internally, these map to:

```cpp
if (nArgs > 27)
  global_loopsamples_tominimum_s = atof(szArgList[27]);
if (nArgs > 28)
  global_reverseeveryothersample = atoi(szArgList[28]);
```

fileciteturn0file13

## Recommended Workflow

1. **Organize Samples**

Place your WAV files in per-module folders. Filenames should include the MIDI note number.

1. **Run the Sampler**

```bash
   spitonicmidi.exe "C:\MySamples\Horn" "*.wav"
```

1. **Enable Looping & Reverse** (optional)

```bash
   spitonicmidi.exe "C:\MySamples\Horn" "*.wav" 1.5 1
```

1. **Inspect Logs**

Open `samples_modid0.txt`, `samples_modid1.txt`, etc., to confirm pitch shifts, loops, and silences.

```card
{
    "title": "Best Practice",
    "content": "Pre-trim and normalize your samples before loading to minimize unwanted looping artifacts."
}
```

---

By understanding these flags and examining the generated logs, you can ensure your sampler library behaves as expected, with smooth loops, transparent pitch-shifting, and controlled handling of missing notes.