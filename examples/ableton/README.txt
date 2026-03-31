Using DigiShow with Ableton Live
================================

demo1_dancing_ink
Dancing ink (ferrofluid) installation – demo adding more expression to electronic music

1. In Ableton, open dancing_ink.als. Tracks Drums and Zheng produce music. Track Electromagnets has MIDI CC envelopes (CC21–CC24) controlling the electromagnets. These MIDI signals go to the IAC Bus (or loopMIDI).

2. In DigiShow, open dancing_ink.dgs. Four signal links map CC signals from Ableton (via IAC/loopMIDI) to Arduino digital pins connected to the electromagnets. Music in Ableton then drives the ferrofluid movement.

(Note: Ableton's MIDI track output port must match DigiShow's MIDI input port.)

--

demo2_jam_with_things
"Jam with things" – demo creating your own experimental instrument

In DigiShow, open jam_with_things.dgs. Signal links map sensor inputs on Arduino to MIDI notes, which are sent to the IAC Bus (or loopMIDI).

In Ableton, open jam_with_things.als. MIDI tracks with various instruments receive notes from DigiShow (via IAC/loopMIDI) and play them as a synthesizer.

(Note: DigiShow's MIDI output port must match Ableton's MIDI track input port.)

