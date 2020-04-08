#include "MIDI_Message.h"

void MIDI_Message::setData(uint8_t da1, uint8_t da2) { setData1(da1); setData2(da2); };

MsgType MIDI_Message::getType() 
{
	switch(_status >> 4)
	{
		case 0: return MsgType::Undefined;
		case 8: return MsgType::NoteOff;
		case 9: return MsgType::NoteOn;
		case 10: return MsgType::PolyPressure;
		case 11: return MsgType::ControlChange;
		case 12: return MsgType::ProgramChange;
		case 13: return MsgType::ChannelPressure;
		case 14: return MsgType::PitchBend;
		default: return MsgType::Undefined;
	}
};

uint8_t MIDI_Message::getChannel() { return _status & 0x0F; };    
uint8_t MIDI_Message::getStatus() { return _status; };
uint8_t MIDI_Message::getData1() { return _data1; };
uint8_t MIDI_Message::getData2() { return _data2; };

uint16_t MIDI_Message::getBendValue() { return (((uint16_t) getData2() << 8) + getData1()); };

void MIDI_Message::runTest()
{
	//Initial set and check
	MIDI_Message test(B10001010, 54, 126);
	// PRINT2("Status: ", test.getStatus()); //8
	// PRINT2("Channel: ", test.getChannel()); //10
	// PRINT2("Data1: ", test.getData1()); //54
	// PRINT2("Data2: ", test.getData2()); //126

	//Increment all values by 1 and check
	test.setType(MsgType::NoteOn);
	test.setChannel(11);
	test.setData1(55);
	test.setData2(127);
	// PRINT2("Status: ", test.getStatus()); //9
	// PRINT2("Channel: ", test.getChannel()); //11
	// PRINT2("Data1: ", test.getData1()); //55
	// PRINT2("Data2: ", test.getData2()); //127

	//Set bend value and check
	test.setBendValue(0x0107);
	// PRINT2("Bend Value: ", test.getBendValue()); //256 + 7 = 263
};

void MIDI_Message::setType(MsgType t)
{
	uint8_t msbData;
	uint8_t lsbData = (_status & B00001111);

	switch(t)
	{
		case MsgType::Undefined: msbData = 0; break;
		case MsgType::NoteOff: msbData = 8 << 4; break;
		case MsgType::NoteOn: msbData = 9 << 4; break;
		case MsgType::PolyPressure: msbData = 10 << 4; break;
		case MsgType::ControlChange: msbData = 11 << 4; break;
		case MsgType::ProgramChange: msbData = 12 << 4; break;
		case MsgType::ChannelPressure: msbData = 13 << 4; break;
		case MsgType::PitchBend: msbData = 14 << 4; break;
		default: msbData = 0; break;
	}

	//Set new value      
	_status = msbData + lsbData;
};

bool MIDI_Message::setChannel(uint8_t ch)
{
	//Validate
	if (ch > 15)
	{
		//DEBUG2("Invalid setChannel: ", ch)
		return false;
	}

	//Calculate and set new value
	uint8_t newValue = (_status & B11110000) + ch;
	_status = newValue;
	return true;
};

void MIDI_Message::setStatus(uint8_t st)
{
	uint8_t type = st >> 4;
	if(type > 7 && type < 15)					
		setType(static_cast<MsgType>(type));

	setChannel(st & 0x0F);
};

void MIDI_Message::setData1(uint8_t da1) { _data1 = da1; };
void MIDI_Message::setData2(uint8_t da2) { _data2 = da2; };

void MIDI_Message::setBendValue(uint16_t bv)
{
	//0 to 16,383
	setData1(bv & 0xFF); //LSB of bv
	setData2(bv >> 8); //MSB of bv
};