//Example sketch for playing MIDI from USB MIDI input through the Teensy library
//Teensy USB MIDI: https://www.pjrc.com/teensy/td_midi.html

//Make sure USB Type (Tools -> USB Type) is set to one of the types including "MIDI"
//Device will currently show up as 'Buzzer MIDI'
//To change it, update name.c (taken from Teensy example: USB_MIDI -> MIDI_name)
#include <MIDI_Device_Controller.h>

#ifdef CORE_TEENSY  
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
#else
  #error "Only for Teensy boards"
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
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleControlChange(onControlChange);
  usbMIDI.setHandlePitchChange(onPitchChange);

  delay(200);
  MDC.playStartupSequence(); //Complicated end beep
  delay(200);
}

void loop() {
  usbMIDI.read(); //Read incoming USB MIDI data
  MDC.process(); //MIDI Device controller background tasks
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
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

void onControlChange(uint8_t channel, uint8_t data1, uint8_t data2)
{
  //"All Sound Off" (120) or "All Notes Off" (123) commands
  //Note: This is usually called multiple times - once for each channel (1-16)
  if(data1 == 120 || data1 == 123)
    MDC.stopPlaying();
}

void onPitchChange(uint8_t channel, int pitch)
{
  if(channel == 10) return; //Ignore the drum track
  MCC.bendNote(0, pitch);
}