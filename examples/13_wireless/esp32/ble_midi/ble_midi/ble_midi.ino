// Requires BLE-MIDI library by lathoub and MIDI library by Francois Best
#include <MIDI.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>

BLEMIDI_CREATE_DEFAULT_INSTANCE()

bool isConnected = false;
int lastSeconds = 0;

void setup() {
  
  Serial.begin(115200);

  MIDI.begin();

  BLEMIDI.setHandleConnected([]() {
    isConnected = true;
    Serial.println("BLE-MIDI connected");
  });
  BLEMIDI.setHandleDisconnected([]() {
    isConnected = false;
    Serial.println("BLE-MIDI disconnected");
  });

  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    Serial.print("note on: ");
    Serial.print(channel);
    Serial.print(" ");
    Serial.print(note);
    Serial.print(" ");
    Serial.println(velocity);
    MIDI.sendNoteOn(note, velocity, channel);
  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    Serial.print("note off: ");
    Serial.print(channel);
    Serial.print(" ");
    Serial.print(note);
    Serial.print(" ");
    Serial.println(velocity);
    MIDI.sendNoteOff(note, velocity, channel);
  });
  MIDI.setHandleControlChange([](byte channel, byte control, byte value) {
    Serial.print("control change: ");
    Serial.print(channel);
    Serial.print(" ");
    Serial.print(control);
    Serial.print(" ");
    Serial.println(value);
    MIDI.sendControlChange(control, value, channel);
  });
  MIDI.setHandleProgramChange([](byte channel, byte program) {
    Serial.print("program change: ");
    Serial.print(channel);
    Serial.print(" ");
    Serial.println(program);
    MIDI.sendProgramChange(program, channel);
  });
  MIDI.setHandlePitchBend([](byte channel, int pitch) {
    Serial.print("pitch bend: ");
    Serial.print(channel);
    Serial.print(" ");
    Serial.println(pitch);
    MIDI.sendPitchBend(pitch, channel);
  });  
}

void loop() {
  
  if (!isConnected) return;

  // Send MIDI messages
  int seconds = (millis()/1000) % 127;

  if (seconds != lastSeconds) {
    lastSeconds = seconds;
    
    MIDI.sendControlChange(20, seconds, 1);
  }

  // Receive MIDI messages
  MIDI.read();
}
