## Configuring ASIO Channels 🎚️

This section explains how to select and override ASIO output channels in the sampler. ASIO channel selectors let you route audio to specific busses on multi-channel interfaces. By default, the first two channels are used, but you can change them via command-line arguments.

### Default Channel Mapping

By default, the sampler maps:

- **Left output** to ASIO channel 1 (index 0)
- **Right output** to ASIO channel 2 (index 1)

These defaults ensure compatibility with standard stereo setups.

```cpp
// Default ASIO channel selectors
global_outputAudioChannelSelectors[0] = 0; // left
global_outputAudioChannelSelectors[1] = 1; // right
```

### Overriding Channels via Command-Line

You can override the default selectors using the 5th and 6th command-line arguments (`argv[4]` and `argv[5]`):

| Argument Index | Selector Array Index | Default | Description |
| --- | --- | --- | --- |
| ---------------: | ---------------------- | --------- | ---------------------------------------------- |
| 4 | `[0]` (left) | 0 | ASIO channel index for left output |
| 5 | `[1]` (right) | 1 | ASIO channel index for right output |


**Examples:**

- `MySampler.exe ... 2 3` maps left → channel 3 and right → channel 4.
- `MySampler.exe ... 8 9` targets channels 9 and 10 on a 10-bus interface.

```cpp
if (nArgs > 4) {
    global_outputAudioChannelSelectors[0] = atoi(szArgList[4]); // left channel
}
if (nArgs > 5) {
    global_outputAudioChannelSelectors[1] = atoi(szArgList[5]); // right channel
}
```

### How It Integrates with PortAudio

Before opening the audio stream, the code wraps these selectors in a **PaAsioStreamInfo** structure. PortAudio then applies them when using the ASIO host API:

```cpp
global_asioOutputInfo.size = sizeof(PaAsioStreamInfo);
global_asioOutputInfo.hostApiType = paASIO;
global_asioOutputInfo.version = 1;
global_asioOutputInfo.flags = paAsioUseChannelSelectors;
global_asioOutputInfo.channelSelectors = global_outputAudioChannelSelectors;
```

When the chosen device uses ASIO, this info is attached; otherwise, PortAudio falls back to default mixing:

```cpp
if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paASIO) {
    global_outputParameters.hostApiSpecificStreamInfo = &global_asioOutputInfo;
} else {
    global_outputParameters.hostApiSpecificStreamInfo = NULL;
}
```

### Practical Tips

- Ensure your interface’s ASIO driver is installed and active.
- Verify channel counts in your DAW or control panel.
- Use even-odd pairs (e.g., 0/1, 2/3) to maintain stereo balance.

```card
{
    "title": "ASIO Mapping Tip",
    "content": "Avoid gaps: use consecutive channel indices for left/right to prevent silent outputs."
}
```

---

By correctly setting `argv[4]` and `argv[5]`, you can route audio to any pair of channels on a multi-bus ASIO interface, giving you flexible output configurations for advanced studio setups.