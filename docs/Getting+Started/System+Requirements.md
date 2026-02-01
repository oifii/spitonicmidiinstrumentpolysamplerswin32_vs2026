# Getting Started – System Requirements

Before you begin installing or building **SpitonicMIDIInstrumentPolySamplers**, ensure your development machine meets the following prerequisites. These requirements guarantee compatibility with the project’s Visual C++ toolset, the Windows 10 SDK, PortAudio, Tonic, and MIDI I/O libraries.

## Supported Platforms 🖥️

This section outlines the core platform and toolset specifications required to compile and run the application.

| Requirement | Details |
| --- | --- |
| Operating System | Windows 10 or later |
| Architecture | x86 (Win32) and x64 |
| Visual Studio | VS2019 or VS2022 (or later) |
| Platform Toolset | **v145** |
| Windows SDK | **Windows 10 SDK** (10.0.xxxxx.x) |
| C++ Workload | **Desktop development with C++** component installed |


### Verifying the Toolset

Open your `.vcxproj` in Visual Studio and confirm the following entries in your project file:

```xml
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
  <PlatformToolset>v145</PlatformToolset>
  <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
</PropertyGroup>
```

## Development Environment ⚙️

Set up Visual Studio with the C++ toolchain and dependencies:

- **Visual Studio Edition**
- Community, Professional, or Enterprise edition of VS2019/VS2022 (17.x)
- **Workload**
- Desktop development with C++ (includes MSVC, CMake, Windows 10 SDK)
- **C++ Language Standard**
- C++17 or later

Ensure the **Windows 10 SDK** is selected during installation.

```card
{
    "title": "SDK Installation",
    "content": "Use the Visual Studio Installer to add the Windows 10 SDK matching the 10.0 target version."
}
```

## Audio Setup 🔊

A functioning audio subsystem is required to send and receive audio through PortAudio:

- **PortAudio Library** (v19 or later)
- Include headers from `lib-src/portaudio-2021/.../include`
- Link against `portaudio_x86.lib` or `portaudio_x64.lib`
- **Supported Host APIs**
- MME (Windows Multimedia Extensions)
- ASIO (Steinberg ASIO)
- WDM-KS (Windows Kernel Streaming)
- **Audio Initialization**

The code initializes PortAudio at startup:

```cpp
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    // Handle error…
  }
  SelectAudioInputDevice();
  SelectAudioOutputDevice();
```

- **Device Selection Tools**
- Use `SelectAudioInputDevice()` and `SelectAudioOutputDevice()` utilities to choose I/O endpoints.

## MIDI Input 🎹

To capture MIDI events, you’ll need at least one MIDI input device or virtual port:

- **Hardware**
- USB MIDI keyboard or MIDI interface
- **Virtual**
- loopMIDI, rtpMIDI, or equivalent
- **Library Support**
- PortMidi: headers in `lib-src/portmidi/...`, link `portmidi_s.lib` or `portmidi-dynamic.lib`
- RtMidi (used in demo synths): instantiates `RtMidiIn` to open ports

```cpp
RtMidiIn *midiIn = new RtMidiIn();
if (midiIn->getPortCount() == 0) {
  std::cerr << "No MIDI ports available!\n";
  exit(1);
}
midiIn->openPort(0);
midiIn->setCallback(&midiCallback);
```

---

With these components in place, you can proceed to clone the repository, configure your solution in Visual Studio, build the application, and start loading sample folders for real-time polyphonic playback.