#include <MIDI_Device_Controller.h>
#include "src/MIDI_Serial/MIDI_Serial.h"

void setup() {
  //Set relevant controller properties
  MDC.setResolution(50);
  MDC.setLEDPin(13);
  
  //Create MIDI Devices
#ifdef CORE_TEENSY
  //First three pins on Teensy
  MDF.createBuzzer(0, 0);
  MDF.createBuzzer(1, 1);
  MDF.createBuzzer(2, 2);
#else
  //First three pins on most Arduino boards
  MDF.createBuzzer(0, 2);
  MDF.createBuzzer(1, 3);
  MDF.createBuzzer(2, 4);
#endif

  //Create a collection
  MCF.createCollection(0, FirstAvailable, 0, 1, 2);

  //Setup MIDI Serial
  MS.setNoteOnHandle(OnNoteOn);
  MS.setNoteOffHandle(OnNoteOff);
  MS.setPitchBendHandle(OnPitchChange);

  MDC.playStartupSequence(0); //Complicated end beep
}

void loop() {
  MS.read(); //Read incoming Hairless MIDI data
  MDC.process(); //MIDI Device controller background tasks
}

void OnNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) 
{
  MCC.playNote(channel, note);
}

void OnNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  MCC.stopNote(channel, note);
}

void OnPitchChange(uint8_t channel, uint16_t pitch)
{
  MCC.bendNote(channel, pitch);
}
