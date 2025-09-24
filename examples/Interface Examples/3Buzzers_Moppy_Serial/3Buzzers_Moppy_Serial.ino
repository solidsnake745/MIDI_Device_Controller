//Example sketch for playing MIDI from Moppy2 serial input
//Moppy2: https://github.com/Sammy1Am/Moppy2
#include <MIDI_Device_Controller.h>
#include "Moppy2_Serial.h"

// Device address for this microcontroller (only messages sent to this address
// will be processed.
#define DEVICE_ADDRESS 0x01

// Minimum and maximum sub-addresses that messages will be processed for.
// E.g. if you have 8 drives this would be 1 and 8. If you have 16, 1 and 16.
#define MIN_SUB_ADDRESS 1
#define MAX_SUB_ADDRESS 8

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
  Serial.begin(57600); //Default baud rate from the Moppy2 library

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

  //Setup Moppy2_Serial
  M2S.setup(DEVICE_ADDRESS, MIN_SUB_ADDRESS, MAX_SUB_ADDRESS);

  //Setup Moppy2 event handlers
  M2S.setCustomSystemHandle(onCustomSystemMessage);
  M2S.setCustomDeviceHandle(onCustomDeviceMessage);

  //Setup MIDI event handlers
  M2S.setNoteOffHandle(onNoteOff);
  M2S.setNoteOnHandle(onNoteOn);
  M2S.setPitchBendHandle(onPitchChange);

  MDC.playStartupSequence();
}

void loop() {
  M2S.process(); //Read incoming Moppy message data
  MDC.process(); //MIDI Device controller background tasks
}

void onCustomSystemMessage(Moppy_Message* msg)
{
  switch(msg->getCommand()) 
  {
  // NETBYTE_SYS_PING is handled by the network adapter (Moppy2_Serial) directly
  case NETBYTE_SYS_START: // Sequence start
    MDC.startPlaying();
    break;
  case NETBYTE_SYS_STOP: // Sequence stop
    MDC.stopPlaying();
    break;
  case NETBYTE_SYS_RESET: // System reset
    //This doesn't really apply to this example as we're not using devices that track position
    //Leaving it in in case this sketch is copied as a basis for something that does
    MDC.resetDevicePositions();
    break;
  }
}

void onCustomDeviceMessage(Moppy_Message* msg)
{
  switch(msg->getCommand()) 
  {
  case NETBYTE_DEV_RESET: // Reset
    {
      //This doesn't really apply to this example as we're not using devices that track position
      //Leaving it in in case this sketch is copied as a basis for something that does
      MIDI_Pitch* d = MDC.getPitchDevice(msg->getSubAddress() - 1);
      if(d) d->resetPosition();
    }
      break;
  }
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //FYI channel is 1 indexed in the Moppy framework by default (see Sub Address in the mapper)
  if(channel == 10) return; //Ignore the drum track
  MCC.stopNote(0, note);
}

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) 
{
  if(channel == 10) return; //Ignore the drum track
  if(velocity != 0)
    MCC.playNote(0, note);
  else
    MCC.stopNote(0, note);
}

void onPitchChange(uint8_t channel, int pitch)
{
  if(channel == 10) return; //Ignore the drum track
  MCC.bendNote(0, pitch);
}