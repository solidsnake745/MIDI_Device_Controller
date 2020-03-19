#include "MIDI_Serial.h"

SerialDebug MIDI_Serial::_debug(DEBUG_MIDISERIAL);

//Constructors and instance management
//_______________________________________________________________________________________________________

//Global singleton instance
MIDI_Serial MS = MIDI_Serial::getInstance();

MIDI_Serial *MIDI_Serial::_instance = NULL;

MIDI_Serial::MIDI_Serial()
{
	Serial.setTimeout(10);
}

MIDI_Serial &MIDI_Serial::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Serial();
	return *_instance;
}

//Serial Handling
//_______________________________________________________________________________________________________
MIDI_Message MIDI_Serial::parseSerial()
{
  uint8_t stat = Serial.read();
  MIDI_Message msg = MIDI_Message(stat);
  
  char buf[2];
  switch(msg.getType())
  {
      case MsgType::NoteOn:
      case MsgType::NoteOff:
      case MsgType::PitchBend:
      case MsgType::ControlChange:
      case MsgType::PolyPressure:
        if(Serial.readBytes(buf, 2) == 2)
          msg.setData(uint8_t(buf[0]), uint8_t(buf[1]));
        else
          msg = MIDI_Message(0);
        break;

      case MsgType::ProgramChange:
      case MsgType::ChannelPressure:
        msg.setData(Serial.read());
        break;
      
      case MsgType::Undefined:
        break;
  }

  return msg;
}

void MIDI_Serial::read()
{
	if(Serial.available() >= 2)
	{
		MIDI_Message msg = parseSerial();
		_midiHandler.handleMIDI(msg);
	}
}