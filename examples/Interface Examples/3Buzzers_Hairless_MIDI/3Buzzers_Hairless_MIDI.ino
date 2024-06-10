//Example sketch for playing MIDI from a serial input from Hairless MIDI
//Hairless MIDI: https://projectgus.github.io/hairless-midiserial/
#include <MIDI_Device_Controller.h>
#include "Hairless_MIDI_Serial.h"

#if ARDUINO_ARCH_AVR
  #warning "Compiling for Arduino AVR"
  const int resolution = 100;
  const uint8_t pins[] = {2, 3, 4};
#elif defined(CORE_TEENSY)
  const uint8_t pins[] = {0, 1, 2};
  #if ARDUINO_TEENSY32
    #warning "Compiling for Teensy 3.2"
    const int resolution = 20;
  #elif ARDUINO_TEENSY40 || ARDUINO_TEENSY41
    #warning "Compiling for Teensy 4.0/4.1"
    const int resolution = 5;
  #else
    //Default to a standard value for other teensy boards
    const int resolution = 40;
  #endif
// #elif ARDUINO_ARCH_ESP8266 //TODO: Implement support
//   #warning "Compiling for ESP8266"
//   int resolution = 100;
//   uint8_t pins[] = {0, 1, 2};
// #elif ARDUINO_ARCH_ESP32 //TODO: Implement support
//   #warning "Compiling for ESP32"
//   int resolution = 20;
//   #define LED_BUILTIN 2
//   uint8_t pins[] = {4, 12, 13};
#endif

void setup() {
  Serial.begin(115200);

  //Setup MIDI_Device_Controller
  MDC.setResolution(resolution);
  MDC.setLEDPin(LED_BUILTIN);
  MDC.setMaxDuration(0); //No limit on how long any one device can play a note

  int numPins = (sizeof(pins)/sizeof(uint8_t));
  IO_DigitalWrite *io = IOF.createDigitalIO(numPins);  
  Base_MIDI_Pitch_Collection *c = MCF.createCollection(0, FirstAvailable);
  for(int x = 0; x < numPins; x++)
  {
    //Add pin to IO
    io->addOutput(pins[x]);

    //Setup MIDI pitch device
    MIDI_Pitch *d = new MIDI_Pitch();
    d->setStepPin(DigitalWrite, pins[x]);
    MDC.addDevice(x, d);

    //Add device to our collection
    c->addDevice(MDC.getDevice(x));
  }

  //Setup MIDI Serial handlers
  HMS.setNoteOffHandle(onNoteOff);
  HMS.setNoteOnHandle(onNoteOn);  
  HMS.setControlChangeHandle(onControlChange);
  HMS.setPitchBendHandle(onPitchChange);

  delay(200);
  MDC.playStartupSequence(); //Complicated end beep
  delay(200);
}

void loop() {
  HMS.process(); //Read incoming Hairless MIDI data
  MDC.process(); //MIDI Device controller background tasks
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  if(channel == 9) return; //Ignore the drum track
  MCC.stopNote(0, note);
}

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) 
{
  if(channel == 9) return; //Ignore the drum track
  if(velocity != 0)
    MCC.playNote(0, note);
  else
    MCC.stopNote(0, note);
}

void onControlChange(uint8_t channel, uint8_t data1, uint8_t data2)
{
  //"All Sound Off" (120) or "All Notes Off" (123) commands
  //Note: This is usually called multiple times - once for each channel (0-15)
  if(data1 == 120 || data1 == 123)
    MDC.stopPlaying();
}

void onPitchChange(uint8_t channel, uint16_t pitch)
{
  if(channel == 9) return; //Ignore the drum track
  
  //pitch value is original MIDI data which can be from 0 to 16383
  //Passing in true for shiftRange to shift it to the expected range of -8192 to 8191
  MCC.bendNote(0, pitch, true);
}