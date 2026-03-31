DigiShow Basic Functionality
============================

1_hotkey.dgs
Respond to key presses via the Hot Key interface. 
In this example, press A to turn the light on, and press B to turn it off. Hot Key is often used in debugging environments where no sensors are installed.

2_pipe.dgs
Copy and transfer signal variables to multiple output targets using a Virtual Pipe. 
In this example, a change in a binary signal within the virtual pipe is converted into an analog signal, which then affects three lighting channels, achieving one-to-many control.

3_presets.dgs
The basic use of presets: save both the output signal values and the LINK states of input–output signal mappings as a snapshot in a preset, allowing one-tap recall later.
In this example:
1. Some presets created in the preset launcher panel, store dimming values for the DMX output channels.
2. Two additional presets, "midi-to-dmx ON" and "midi-to-dmx OFF," store the LINK states of signal mappings from MIDI CC input to DMX dimming output.
3. Two extra signal links added to the signal link table, trigger dimming presets in response to key presses.

4_cues.dgs
Beyond snapshots, preset scenes can also contain signal outputs that change over a timeline — this requires attaching a Cue Player to the preset.
In this example:
Actions for adjusting screen colors and playing audio are placed on a specific CUE timeline. Right‑click a preset launch button and select “Edit Preset” to find the output signals with a highlighted +CUE button; clicking it lets you view and edit the timeline in the Cue Player.

5_signal_types.dgs
DigiShow supports mutual mapping between three signal types: Binary (on/off) signals, Analog signals, and Note signals.
This example shows the possible conversions between signal types and how to set the mapping parameters.

6_midi_mapping.dgs
Signal mapping is the core feature of DigiShow, allowing different types of input and output signals to be linked and converted.
In this example:
1. Notes and pitch‑bend signals from a MIDI keyboard are mapped to change the screen’s backlight color.
2. The volume knob (CC7) signal from a MIDI keyboard is mapped to adjust the screen image opacity.
