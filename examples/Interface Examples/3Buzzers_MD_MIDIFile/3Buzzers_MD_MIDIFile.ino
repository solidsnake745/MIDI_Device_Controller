//Example sketch for playing MIDI files from an SD card using the MD_MIDIFile library
//MD_MIDIFile: https://github.com/MajicDesigns/MD_MIDIFile
#include <SdFat.h>
#include <MD_MIDIFile.h>
#include <MIDI_Device_Controller.h>

#if ARDUINO_ARCH_AVR
  #warning "Compiling for Arduino AVR"
  //NOTE: Does not compile for Uno/Nano - Not enough memory
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

static SdFat SD;
static MD_MIDIFile MD;
MIDI_Handler midiHandler;

const uint16_t WAIT_DELAY = 5000; //ms

//MIDIs listed below are available in the library folder under "Sample MIDIs"
//Place them on the SD card in a folder named 'MIDIs'
const char *midiList[] = 
{
  "MIDIs/Addams Family - Main.mid",
  "MIDIs/MM1 - Airman.mid",
  "MIDIs/MM1 - Bubbleman.mid",
  "MIDIs/Pirates of the Caribbean - He's A Pirate.mid",
  "MIDIs/Super Mario Bros - Game Over.mid",
  "MIDIs/Super Mario Bros - Level Complete.mid",
  "MIDIs/Super Mario Bros - Overworld.mid",
  "MIDIs/Super Mario Bros - Starman.mid",
  "MIDIs/Super Mario Bros - Underwater.mid",
  "MIDIs/Super Mario World - Overworld v2.mid",
  "MIDIs/Tetris - Theme A.mid",
  "MIDIs/Wii - Mii Channel Theme.mid",
  "MIDIs/Wii - Shopping Channel Theme.mid"
};

void setup() 
{
  Serial.begin(115200);
  delay(200);

  //NOTE: Setting up SDFat here works, but for some reason doesn't carry over to loop()
  //Needs to be called in loop() again for whatever reason so just setting it up there
  // Setup SDFat 
  // if (!SD.begin(SS, SPI_HALF_SPEED)) {
  //   SD.initErrorHalt(&Serial);
  // }

  //Setup MD_MIDIFile
  MD.begin(&SD);
  MD.setMidiHandler(midiCallback);

  //Setup MIDI_Device_Controller
  MDC.setResolution(resolution);
  
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
  
  //Setup our MIDI handler instance
  midiHandler.setNoteOnHandle(onNoteOn);
  midiHandler.setNoteOffHandle(onNoteOff);
  midiHandler.setPitchBendHandle(onPitchBend);

  delay(200);
  MDC.playStartupSequence(); //Complicated end beep
  delay(200);
}

static bool sdInitialized = false;
static enum { S_IDLE, S_PLAYING, S_END, S_WAIT_BETWEEN } state = S_IDLE;
static uint16_t currTune = ARRAY_SIZE(midiList);
static uint32_t timeStart;

void loop() 
{
  playMidiFiles();
  MDC.process();
}

//Original code taken and adapted from MD_MIDIFile_Play example
void playMidiFiles()
{
  switch (state)
  {
    case S_IDLE:    // now idle, set up the next tune
      Serial.println(F("IDLE"));

      currTune++;
      if (currTune >= ARRAY_SIZE(midiList))
        currTune = 0;

      // use the next file name and play it
      Serial.print(F("File: "));
      Serial.print(midiList[currTune]);
      
      if(!sdInitialized)
      {
        //No idea why, but setting this up in setup() does not carry over to loop()
        //So setting it up once here
        //Also have to supply SPI speed for it to work on Teensy and full speed fails
        if (!SD.begin(SS, SPI_HALF_SPEED))
          SD.initErrorHalt(&Serial);
        else          
          sdInitialized = true;
      }

      { //Scope this section of code so we can declare a local int err
        int err = MD.load(midiList[currTune]);
        if (err != MD_MIDIFile::E_OK)
        {
          Serial.print(F(" - MD load Error "));
          Serial.println(err);
          timeStart = millis();          
          state = S_WAIT_BETWEEN;
        }
        else
        {
          Serial.println(F("\nPLAYING"));
          state = S_PLAYING;
        }
      }      
      break;

    case S_PLAYING: // play the file
      if (!MD.isEOF())
        MD.getNextEvent();
      else
      {
        Serial.println(F("END"));
        state = S_END;
      }
      break;

    case S_END:   // done with this one
      MD.close();
      MDC.stopPlaying();
      timeStart = millis();
      Serial.println(F("WAIT BETWEEN"));
      state = S_WAIT_BETWEEN;      
      break;

    case S_WAIT_BETWEEN:    // signal finished with a dignified pause      
      if (millis() - timeStart >= WAIT_DELAY)
        state = S_IDLE;
      break;

    default:
      state = S_IDLE;
      break;
  }
}

void midiCallback(midi_event *pev)
{
  //Convert MD_MIDIFile event to a MIDI_Message object
  //midi_event->data stores original bytes of data in order of status, data1, data2
  MIDI_Message msg = MIDI_Message(pev->data[0], pev->data[1], pev->data[2]);

  //Handle this MIDI message
  midiHandler.handleMIDI(msg);

  //Serial print message data
  printMidiMsg(&msg);
}

void printMidiMsg(MIDI_Message *msg)
{
  switch(msg->getType())
  {
    case MsgType::NoteOff:
      Serial.print(F("Off")); break;
    case MsgType::NoteOn:
      Serial.print(F("On")); break;      
    case MsgType::PitchBend:
      Serial.print(F("Bend")); break;
    default:
      return; //Ignore other types of messages
  }  

  Serial.print(F("\t"));
  Serial.print(msg->getChannel());
  Serial.print(F("\t"));
  Serial.print(msg->getData1());
  Serial.print(F("\t"));
  Serial.print(msg->getData2());
  if(msg->getType() == MsgType::PitchBend)
  {
    Serial.print(F("\t"));
    Serial.println(msg->getBendValue());
  }  
  else
    Serial.println();
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //Distribute notes except for the drum track
  if(channel == 9) return;
  MCC.stopNote(0, note);
}

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
  //Distribute notes except for the drum track
  if(channel == 9) return;
  if(velocity != 0)
    MCC.playNote(0, note);
  else
    MCC.stopNote(0, note);
}

void onPitchBend(uint8_t channel, uint16_t pitch)
{
  //Distribute notes except for the drum track
  if(channel == 9) return;
  MCC.bendNote(0, pitch);
}