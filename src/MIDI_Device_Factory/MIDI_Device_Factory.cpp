#include "MIDI_Device_Factory.h"

//Global singleton instance
MIDI_Device_Factory MDF = MIDI_Device_Factory::getInstance();

MIDI_Device_Factory *MIDI_Device_Factory::_instance = NULL;

MIDI_Device_Factory::MIDI_Device_Factory()
{
}

MIDI_Device_Factory &MIDI_Device_Factory::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Device_Factory();
	return *_instance;
};

MIDI_Shift_Register *MIDI_Device_Factory::createShiftRegister(uint8_t size, uint8_t startingNote, uint8_t latchPin)
{
	MDC.initializeShiftRegister(size, startingNote, latchPin);
	return MDC.getShiftRegister();
}