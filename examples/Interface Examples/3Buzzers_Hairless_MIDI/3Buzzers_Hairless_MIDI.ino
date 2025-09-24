//Example sketch for playing MIDI from a serial input from Hairless MIDI
//Hairless MIDI: https://projectgus.github.io/hairless-midiserial/
#include <MIDI_Device_Controller.h>
#include "Hairless_MIDI_Serial.h"

#if ARDUINO_ARCH_AVR
  // #warning "Compiling for Arduino AVR (Uno/Nano/etc.)"
  const uint8_t pins[] = {2, 3, 4};
#elif defined(CORE_TEENSY)
  const uint8_t pins[] = {0, 1, 2};
  #if ARDUINO_TEENSY32
    // #warning "Compiling for Teensy 3.2"
  #elif ARDUINO_TEENSY40 || ARDUINO_TEENSY41
    // #warning "Compiling for Teensy 4.0/4.1"
  #else
    // #warning "Compiling for some Teensy board"
  #endif
#elif ARDUINO_ARCH_ESP32
  // #warning "Compiling for ESP32"  
  #define LED_BUILTIN 2
  uint8_t pins[] = {4, 16, 17}; //16 = RX2, 17 = TX2
#endif

void setup() 
{
  Serial.begin(115200);

  //Setup MIDI_Device_Controller
  MDC.setLEDPin(LED_BUILTIN);

  int numPins = (sizeof(pins)/sizeof(uint8_t));
  IO_DigitalWrite* io = IOF.createDigitalIO(numPins);  
  Base_MIDI_Pitch_Collection* c = MCF.createCollection(0, FirstAvailable);
  for(int x = 0; x < numPins; x++)
  {
    //Add pin to IO
    io->addPin(pins[x]);

    //Setup MIDI pitch device
    MIDI_Pitch* d = new MIDI_Pitch();
    d->setStepPin(IODigital, pins[x]);
    MDC.addPitchDevice(x, d);

    //Add device to our collection
    c->addDevice(MDC.getPitchDevice(x));
  }

  //Setup MIDI Serial handlers
  HMS.setNoteOffHandle(onNoteOff);
  HMS.setNoteOnHandle(onNoteOn);  
  HMS.setControlChangeHandle(onControlChange);
  HMS.setPitchBendHandle(onPitchChange);

  MDC.playStartupSequence();
}

void loop() 
{
  HMS.process(); //Read incoming Hairless MIDI data
  MDC.process(); //MIDI Device controller background tasks
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //FYI channel is 0 indexed in the HairlessMIDI framework
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

void onPitchChange(uint8_t channel, int pitch)
{
  if(channel == 9) return; //Ignore the drum track
  
  //'pitch' value is original MIDI data which can be from 0 to 16383
  //The calculations for pitch bending expects a range of -8192 to 8191 and a lot of frameworks do this shift already
  //HairlessMIDI does not and transmits these original values
  //Passing in true for shiftRange is necessary and shifts it to the expected range (subtracts 8192)
  MCC.bendNote(0, pitch, true);
}