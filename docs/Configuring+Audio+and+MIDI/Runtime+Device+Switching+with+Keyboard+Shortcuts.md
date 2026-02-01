# Configuring Audio and MIDI – Runtime Device Switching with Keyboard Shortcuts

This section explains how to switch audio and MIDI devices on the fly using keyboard shortcuts in the main application window.  Each shortcut reconfigures the selected device name, logs feedback to the status area, and prepares the system for reconnection.

## Keyboard Shortcuts Overview

Use the following keys to select input or output devices at runtime:

| Key | 🎛️ Device Type | Action |
| --- | --- | --- |
| :---: | :------------------------------- | :------------------------------------------------- |
| **K** | 🎹 Hardware MIDI Keyboard | Select your physical MIDI controller (e.g. “Q49”) |
| **L** | 🔄 loopMIDI Virtual Port | Route MIDI through loopMIDI Port 1 |
| **V** | 🛠️ Virtual Audio Cable Device | Switch audio output to VB-Audio Virtual CABLE |
| **W** | 🖥️ Display Audio Output | Use your monitor’s audio output (Intel Display Audio) |
| **X** | 🎧 Computer Speakers/Headphones | Route audio to built-in speakers or headphones |


## How It Works

> **Note:** Shortcuts only apply when devices are currently . Press **C** to toggle connect/disconnect before switching.

Each shortcut handler follows these steps:

1. **Disconnect**

If `global_audiomidi_devices == "connected"`, the code aborts switching until you disconnect (via the **C** key) .

1. **Set Device Name**

The handler saves the previous name, assigns a new device string to `global_inputmididevicename` or `mySPIAudioDevice.global_audiooutputdevicename`, and formats a status message.

1. **Log Feedback**

Calls `StatusAddText(...)` to display the newly selected device name in the status text area, confirming your choice.

## Code Snippet

Below is an excerpt from the **WM_KEYDOWN** message handler showing how each shortcut is implemented:

```cpp
// In your WindowProc or message handling function:
case WM_KEYDOWN:
    // K – Hardware MIDI Keyboard
    if (wParam == 0x4B) {
        // 1) Ensure devices are disconnected
        if (global_audiomidi_devices == "connected") break;
        // 2) Switch to hardware keyboard
        global_inputmididevicename_prev = global_inputmididevicename;
        global_inputmididevicename = "Q49";
        swprintf(pWCHAR, L"global_inputmididevicename is %s\n",
                 std::wstring(global_inputmididevicename).c_str());
        StatusAddText(pWCHAR);
    }
    // L – loopMIDI Virtual Port
    else if (wParam == 0x4C) {
        if (global_audiomidi_devices == "connected") break;
        global_inputmididevicename_prev = global_inputmididevicename;
        global_inputmididevicename = "loopMIDI Port 1";
        swprintf(pWCHAR, L"global_inputmididevicename is %s\n",
                 std::wstring(global_inputmididevicename).c_str());
        StatusAddText(pWCHAR);
    }
    // V – Virtual Audio Cable
    else if (wParam == 0x56) {
        if (global_audiomidi_devices == "connected") break;
        global_audiooutputdevicename_prev =
            mySPIAudioDevice.global_audiooutputdevicename;
        mySPIAudioDevice.global_audiooutputdevicename =
            "CABLE Input (VB-Audio Virtual C";
        swprintf(pWCHAR, L"global_audiooutputdevicename is %s\n",
                 std::wstring(
                   mySPIAudioDevice.global_audiooutputdevicename).c_str());
        StatusAddText(pWCHAR);
    }
    // W – Display Audio Output
    else if (wParam == 0x57) {
        if (global_audiomidi_devices == "connected") break;
        global_audiooutputdevicename_prev =
            mySPIAudioDevice.global_audiooutputdevicename;
        mySPIAudioDevice.global_audiooutputdevicename =
            "55S425CA (Intel(R) Display Audi";
        swprintf(pWCHAR, L"global_audiooutputdevicename is %s\n",
                 std::wstring(
                   mySPIAudioDevice.global_audiooutputdevicename).c_str());
        StatusAddText(pWCHAR);
    }
    // X – Computer Speakers/Headphones
    else if (wParam == 0x58) {
        if (global_audiomidi_devices == "connected") break;
        global_audiooutputdevicename_prev =
            mySPIAudioDevice.global_audiooutputdevicename;
        mySPIAudioDevice.global_audiooutputdevicename =
            "Speakers/Headphones (Realtek(R)";
        swprintf(pWCHAR, L"global_audiooutputdevicename is %s\n",
                 std::wstring(
                  mySPIAudioDevice.global_audiooutputdevicename).c_str());
        StatusAddText(pWCHAR);
    }
    // C – Toggle Connect/Disconnect
    else if (wParam == 0x43) {
        // Handles PortAudio and PortMidi shutdown or initialization...
    }
    break;
```

## Best Practices

- **Always disconnect** devices (press **C**) before switching.
- **Verify** new device names in `devices.txt` if you log to file.
- **Extend** by adding new cases under `WM_KEYDOWN` with unique hex codes.

```card
{
    "title": "Tip",
    "content": "Use clear, unique device name strings to avoid mismatches in your host system."
}
```

This mechanism provides quick, in-session reconfiguration of audio and MIDI routing without restarting the application, streamlining live performance and testing workflows.