# Pitch Shifting and Advanced Sample Processing

This section explores how the sampler applies **real-time pitch shifting** to loaded samples. It explains the thread-safe variant `smbPitchShift_threadsafe`, its role in the processing pipeline, and how to use it in a multi-module, multi-core environment.

## Thread-Safe Pitch Shifting per Module 🛡️

To support **concurrent** pitch shifts without state corruption, the code provides `smbPitchShift_threadsafe`. It maps each pitch-shift call to a unique module index (0–15), isolating internal buffers per module. This design lets up to 16 modules process samples in parallel on multi-core CPUs.

### API Overview

The **declaration** of the thread-safe function lives in `SMBPITCHSHIFT.H`:

```cpp
void smbPitchShift_threadsafe(
    int midimoduleindex,
    float pitchShift,
    long numSampsToProcess,
    long fftFrameSize,
    long osamp,
    float sampleRate,
    float* indata,
    float* outdata
);
```

This variant extends the classic `smbPitchShift` by using **per-module** buffers instead of shared statics .

### Parameters

The table below describes each parameter:

| Parameter | Type | Description |
| --- | --- | --- |
| **midimoduleindex** | int | Module index (0–15). Map one-to-one with MIDI channels. |
| **pitchShift** | float | Pitch factor: 0.5 (one octave down) to 2.0 (one octave up). |
| **numSampsToProcess** | long | Number of samples in `indata[]`. |
| **fftFrameSize** | long | FFT window size (power of two, e.g., 2048). |
| **osamp** | long | Oversampling factor (e.g., 4 for moderate quality, 32 for best quality). |
| **sampleRate** | float | Input sample rate in Hz (e.g., 44100.0). |
| **indata** | float* | Pointer to input buffer (in-place processing allowed). |
| **outdata** | float* | Pointer to output buffer. |


### Usage Example 💻

Here’s a typical call within the sample-processing pipeline. It shifts both left and right channels for module `global_samplermodulesindex` by a given semitone offset:

```cpp
#include "smbPitchShift.h"

// Compute pitch factor from semitones
int semitoneOffset = midinote - referencemidinote;
float pitchFactor = pow(2.0f, semitoneOffset / 12.0f);

// Apply thread-safe pitch shift on left channel
smbPitchShift_threadsafe(
    global_samplermodulesindex,       // unique module index
    pitchFactor,
    myLeftWavSet.numSamples,
    2048,                              // FFT frame size
    4,                                 // oversampling
    44100.0f,                          // sample rate
    myLeftWavSet.pSamples,
    myLeftWavSet.pSamples
);

// Mirror the same shift on right channel
smbPitchShift_threadsafe(
    global_samplermodulesindex,
    pitchFactor,
    myRightWavSet.numSamples,
    2048,
    4,
    44100.0f,
    myRightWavSet.pSamples,
    myRightWavSet.pSamples
);
```

This example comes from the `pitchshift` helper in **smbpitchshift.cpp** .

### Implementation Details

Key design points in `smbpitchshift.cpp`:

- Defines `MAX_CONCURRENT_SMBPITCHSHIFT` as 16.
- Allocates per-module buffers:
- `gInFIFO[16][MAX_FRAME_LENGTH]`
- `gOutFIFO[16][MAX_FRAME_LENGTH]`
- FFT workspace and phase arrays per module .
- Asserts if `midimoduleindex` is out of [0,15].
- Internally mirrors the standard STFT-based algorithm but indexes all buffers by module.

### Configuration Tips

- **FFT Size vs. Latency:** Larger `fftFrameSize` (e.g., 4096) yields smoother pitch shifts but adds latency.
- **Oversampling (**`**osamp**`**):** Higher values improve audio quality at the cost of CPU.
- **Module Indexing:** Always assign a unique `midimoduleindex` per sampler instance.

### Processing Workflow

```mermaid
flowchart LR
  A[Load Sample into WavSet]
  B[Split into Left & Right]
  C[Thread-Safe Pitch Shift per Module]
  D[Recombine Channels]
  E[Create Tonic Sample Table]
  A --> B
  B --> C
  C --> D
  D --> E
```

This flow describes how samples move from disk into the live sampler table.

### Best Practices 📝

- Initialize all sampler modules with distinct channel indexes (0–15).
- Avoid calling `smbPitchShift_threadsafe` twice with the same index at once.
- Monitor CPU usage when raising `osamp` or `fftFrameSize`.
- Use debug logs (`global_mididebugmode`) to trace per-module processing.

```card
{
    "title": "Module Index",
    "content": "Assign a unique midimoduleindex (0\u201315) per sampler to avoid data corruption."
}
```

By following these guidelines, you ensure **safe**, **parallel**, and **high-quality** pitch shifting across multiple sampler modules.