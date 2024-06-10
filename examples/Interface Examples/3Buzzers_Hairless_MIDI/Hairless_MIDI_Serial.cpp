#include "Hairless_MIDI_Serial.h"

//Constructors and instance management
//_______________________________________________________________________________________________________

//Global singleton instance
Hairless_MIDI_Serial HMS = Hairless_MIDI_Serial::getInstance();

Hairless_MIDI_Serial *Hairless_MIDI_Serial::_instance = NULL;

Hairless_MIDI_Serial::Hairless_MIDI_Serial()
{
	Serial.setTimeout(10);
}

Hairless_MIDI_Serial &Hairless_MIDI_Serial::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new Hairless_MIDI_Serial();
	return *_instance;
}

//Serial Handling
//_______________________________________________________________________________________________________
MIDI_Message Hairless_MIDI_Serial::parseSerial()
{
	uint8_t stat = Serial.read();
	MIDI_Message msg = MIDI_Message(stat);

	char buf[2];
	switch(msg.getType())
	{
		case NoteOn:
		case NoteOff:
		case PitchBend:
		case ControlChange:
		case PolyPressure:
			if(Serial.readBytes(buf, 2) == 2)
				msg.setData(uint8_t(buf[0]), uint8_t(buf[1]));
			else
				msg = MIDI_Message(0);
			break;

		case ProgramChange:
		case ChannelPressure:
			msg.setData(Serial.read());
			break;

		case Undefined:
			break;
	}

	return msg;
}

void Hairless_MIDI_Serial::process()
{
	if(Serial.available() >= 2)
	{
		MIDI_Message msg = parseSerial();
		_midiHandler.handleMIDI(msg);
	}
}