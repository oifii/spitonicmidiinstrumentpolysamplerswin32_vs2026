# Troubleshooting and Logs – MIDI Routing and Transposition Checks

When pressing keys on your MIDI controller yields no sound, follow these checks in order to isolate the issue. Verify MIDI device selection, channel configuration, and octave range. Use runtime logs and UI messages to narrow down the root cause.

## 1. Verify MIDI Input Device Selection 🎛️

Ensure the sampler is listening to your physical or virtual MIDI port.

- Press **K** to select your hardware controller (e.g., “Q49”).
- Press **L** to select a loopMIDI port (e.g., “loopMIDI Port 1”).

| Key | Device Name | Status Message |
| --- | --- | --- |
| K | Q49 | `global_inputmididevicename is Q49` |
| L | loopMIDI Port 1 | `global_inputmididevicename is loopMIDI Port 1` |


```cpp
// Example: pressing L sets loopMIDI Port 1
global_inputmididevicename_prev = global_inputmididevicename;
global_inputmididevicename = "loopMIDI Port 1";
swprintf(pWCHAR, L"global_inputmididevicename is %s\n", mywstring.c_str());
StatusAddText(pWCHAR);
```

This behavior is implemented in the window message handler for **K** and **L** keys .

## 2. Confirm MIDI Channel Configuration

The sampler listens only on the configured channel or range. You can set this via:

1. **Command-line argument** `global_inputmidichannel`
2. **Runtime** by typing numbers then pressing **M** (Module key)

| Value | Behavior |
| --- | --- |
| 0 – 15 | Listen to a single MIDI channel |
| −1 | Listen to **all** channels mapping notes to each loaded module |
| −2 | Listen to **all** channels mapping into modules cyclically |
| −3 | Listen to **all** channels mapping **all** notes to the selected module |


```cpp
// On ‘M’ key: parse input string and update
int inputmidichannel = atoi(global_input_string.c_str());
if (inputmidichannel >= -3 && inputmidichannel <= 15)
    global_inputmidichannel = inputmidichannel;
swprintf(pWCHAR, L"global_inputmidichannel is %d\n", global_inputmidichannel);
StatusAddText(pWCHAR);
```

This M-key logic and special values are defined in the main window procedure  and clamped in WinMain .

## 3. Check Octave Transposition Range 🎚️

Octave shifts can push notes outside the 0–127 MIDI range, muting them entirely. Reset or adjust as needed:

- **Q** / **Up Arrow**: shift **up** by one octave
- **Z** / **Down Arrow**: shift **down** by one octave
- **Page Up**: increase shift across **all** modules
- **Page Down**: decrease shift across **all** modules
- **R**: reset **all** modules to zero shift

```cpp
// Example: Q or Up Arrow increases shift
global_numberofoctavetoshift[selected] = min(11, global_numberofoctavetoshift[selected] + 1);
swprintf(pWCHAR,
    L"transposing %d octave all midi events mapping to sampler module %d\n",
    global_numberofoctavetoshift[selected], selected);
StatusAddText(pWCHAR);
```

Logs report the new shift and module index . Ensure shifts stay within ±11 octaves, and resultant note numbers remain in [0,127].

## 4. Use MIDI Debug Mode and UI Messages 🐞

Enable verbose MIDI logging to see incoming messages and channel mappings:

- **D** key toggles `global_mididebugmode` between **0** and **1**.
- When enabled, every raw MIDI event is printed via `CSpiMidiUtility::output()`.

```cpp
if (global_mididebugmode == 1)
    mySpiMidiUtility.output(event.message);
```

Review the console or sample-specific log files for:

- **MIDI status bytes** (Note On/Off, CC, Program change)
- **Channel number** and **mapped module index**
- **Raw data1/data2** values

These logs help confirm that data is reaching the sampler, and whether it’s filtered out by channel or out-of-range transposition .

---

By systematically checking device, channel, octave, and debug logs, you can pinpoint why no sound issues occur and correct routing or configuration errors quickly.