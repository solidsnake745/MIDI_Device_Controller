#include "MIDI_Message.h"

MIDI_Message::MIDI_Message(MIDI_MsgType t, uint8_t ch, uint8_t d1, uint8_t d2) : MIDI_Message(d1, d2)
{
	setType(t);
	setChannel(ch);
}

MIDI_Message::MIDI_Message(uint8_t st, uint8_t d1, uint8_t d2) : MIDI_Message(d1, d2)
{
	setStatus(st);
}

MIDI_Message::MIDI_Message(uint8_t d1, uint8_t d2)
{
	setData1(d1);
	setData2(d2);
}

MIDI_Message::MIDI_Message(uint8_t st)
{
	setStatus(st);
}

MIDI_MsgType MIDI_Message::getType() 
{
	switch(_status >> 4)
	{
		case 0: return Undefined;
		case 8: return NoteOff;
		case 9: return NoteOn;
		case 10: return PolyPressure;
		case 11: return ControlChange;
		case 12: return ProgramChange;
		case 13: return ChannelPressure;
		case 14: return PitchBend;
		default: return Undefined;
	}
}

void MIDI_Message::setType(MIDI_MsgType t)
{
	uint8_t msbData;
	uint8_t lsbData = (_status & 0b00001111);

	switch(t)
	{
		case Undefined: msbData = 0; break;
		case NoteOff: msbData = 8 << 4; break;
		case NoteOn: msbData = 9 << 4; break;
		case PolyPressure: msbData = 10 << 4; break;
		case ControlChange: msbData = 11 << 4; break;
		case ProgramChange: msbData = 12 << 4; break;
		case ChannelPressure: msbData = 13 << 4; break;
		case PitchBend: msbData = 14 << 4; break;
		default: msbData = 0; break;
	}

	//Set new value
	_status = msbData + lsbData;
}

bool MIDI_Message::setChannel(uint8_t ch)
{
	//Validate
	if (ch > 15)
	{
	#if MIDIMSG_INCLUDE_SERIALDEBUG
		_debug.println(F("Invalid setChannel: %d"), ch);
	#endif
		return false;
	}

	//Calculate and set new value
	uint8_t newValue = (_status & 0b11110000) + ch;
	_status = newValue;
	return true;
}

void MIDI_Message::setStatus(uint8_t st)
{
	//Status byte uses all 8 bits and the most significant bit is always 1
	uint8_t type = st >> 4;
	if(type > 7 && type < 15)					
		setType(static_cast<MIDI_MsgType>(type));

	setChannel(st & 0x0F);
}

void MIDI_Message::setData1(uint8_t da1) 
{ 
	//Validate
	if (da1 > 127)
	{
	#if MIDIMSG_INCLUDE_SERIALDEBUG
		_debug.println(F("Invalid setData1: %d"), da1);
	#endif		
		return;
	}
	
	//0 to 127 (7 bits)
	_data1 = da1;
}

void MIDI_Message::setData2(uint8_t da2)
{ 
	//Validate
	if (da2 > 127)
	{
	#if MIDIMSG_INCLUDE_SERIALDEBUG
		_debug.println(F("Invalid setData2: %d"), da2);
	#endif
		return;
	}
	
	//0 to 127 (7 bits)
	_data2 = da2;
}

void MIDI_Message::setBendValue(uint16_t bv)
{
	//Validate
	if (bv > 16383)
	{
	#if MIDIMSG_INCLUDE_SERIALDEBUG
		_debug.println(F("Invalid setBendValue: %d"), bv);
	#endif		
		return;
	}
	
	//0 to 16,383 (14 bits)
	//Convert 16 bit value into two 7 bit bytes
	setData1(bv & 0x7F); //LSB of bv
	setData2(bv >> 7); //MSB of bv
}