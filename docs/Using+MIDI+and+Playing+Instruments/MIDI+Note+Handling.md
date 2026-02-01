## MIDI Note Handling 🎹

When a MIDI message arrives, the sampler parses it into four components: **command**, **channel**, **note number**, and **velocity**. Based on these, it applies octave shifts, then routes the event to the appropriate polyphonic synthesizer instance (`PolySynth`) for playback or release.

### Parsing Incoming MIDI Messages

Each MIDI event is read from the PortMidi input stream. The core parsing loop looks like this:

```cpp
while ((count = Pm_Read(global_pPmStreamMIDIIN, &event, 1))) {
    if (count != 1) continue;

    // Extract raw MIDI fields
    int command = Pm_MessageStatus(event.message) & MIDI_CODE_MASK;
    int chan    = Pm_MessageStatus(event.message) & MIDI_CHN_MASK;
    int data1   = Pm_MessageData1(event.message);
    int data2   = Pm_MessageData2(event.message);

    // ... (channel filtering logic) ...
}
```

- **command**: MIDI message type (note on/off, program change, control change).
- **chan**: MIDI channel (0–15).
- **data1**: Note number (0–127).
- **data2**: Velocity or controller value (0–127).

### Applying Octave Shifts

Before triggering playback, the code applies a global octave offset per channel:

```cpp
int midiNoteNumber = data1;
midiNoteNumber += 12 * global_numberofoctavetoshift[chan];
if (midiNoteNumber < 0 || midiNoteNumber > 127)
  midiNoteNumber = data1;
```

- `global_numberofoctavetoshift` is an array mapping each channel to an integer octave offset.
- Notes outside the valid 0–127 range revert to the original pitch.

### Triggering noteOn and noteOff

Depending on the parsed **command** and **velocity**, the sampler performs:

| Condition | Action |
| --- | --- |
| `command == MIDI_OFF_NOTE` | `noteOff(midiNoteNumber)` |
| `command == MIDI_ON_NOTE && data2 == 0` | Treated as Note Off → `noteOff(midiNoteNumber)` |
| `command == MIDI_ON_NOTE && data2 > 0` | `noteOn(module, midiNoteNumber, data2)` |


```cpp
if (command == MIDI_OFF_NOTE || (command == MIDI_ON_NOTE && data2 == 0)) {
  poly[chan].noteOff(midiNoteNumber);
  global_numberofnotes_on--;
}
else if (command == MIDI_ON_NOTE) {
  poly[chan].noteOn(chan, midiNoteNumber, data2);
  global_numberofnotes_on++;
}
```

- **Note On** events pass:
- **module (chan)** — selects the sampler module
- **midiNoteNumber** — final note number after shift
- **velocity** (`data2`) — drives Tonic’s velocity-sensitive envelopes and filters
- **Note Off** events simply release the voice for that note.

### Internal Voice Allocation

Under the hood, `PolySynth` uses a basic allocator that:

1. **Selects an available voice** or steals one if all are busy.
2. **Configures buffer playback** for the sample corresponding to the MIDI note.
3. **Sets Tonic control parameters**:
4. `"polyNote"` → actual MIDI note
5. `"polyGate"` → 1 for note on, 0 for note off
6. `"polyVelocity"` → velocity value
7. `"polyVoiceNumber"` → internal voice index

```cpp
void BasicPolyphonicAllocator::noteOn(int moduleid, int note, int velocity) {
  int voiceNumber = getNextVoice(note);
  if (voiceNumber < 0) return;
  global_psuperplayer[moduleid][voiceNumber].setBuffer(note);
  voice.synth.setParameter("polyNote", note);
  voice.synth.setParameter("polyGate", 1.0);
  voice.synth.setParameter("polyVelocity", velocity);
  voice.synth.setParameter("polyVoiceNumber", voiceNumber);
  // Queue management...
}
```

This ensures **polyphonic** playback with proper velocity-sensitive shaping.

### Handling Special Cases

- **Note On with velocity = 0** is explicitly treated as Note Off, per MIDI spec.
- **Program Change** messages (`MIDI_CH_PROGRAM`) are currently ignored.
- **Control Change** messages (`MIDI_CTRL`) are also mostly ignored, except when both controller number and value are zero. In that case, legacy hooks could launch external scripts (now deprecated).

---

By following this flow, the sampler delivers responsive, multitimbral performance, letting Tonic shape each voice’s envelope and filter dynamically according to MIDI velocity and octave configuration.