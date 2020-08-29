#include "MIDI_Device_Factory.h"

//Global singleton instance
MIDI_Device_Factory MDF = MIDI_Device_Factory::getInstance();

MIDI_Device_Factory *MIDI_Device_Factory::_instance = NULL;

MIDI_Device_Factory::MIDI_Device_Factory()
{
}

bool MIDI_Device_Factory::validateIndex(uint8_t index)
{
	if(index > MAX_PITCH_DEVICES - 1)
	{
		// _debug.println(F("Can't add pitch device at index %d; max index is %d"), index, MAX_PITCH_DEVICES - 1);
		return false;
	}
		
	if(MDC.getDevice(index))
	{
		// _debug.debugln(5, F("Device already created at index %d"), index);
		return false;
	}
	
	return true;
}
	
MIDI_Device_Factory &MIDI_Device_Factory::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Device_Factory();
	return *_instance;
};

MIDI_Pitch *MIDI_Device_Factory::createFDD(uint8_t index, uint8_t stepPin, uint8_t dirPin)
{
	if(!validateIndex(index))
		return NULL;
	
	//_debug.debugln(5, F("FDD device created at index %d"), index);	
	MDC.addDevice(index, new MIDI_Pitch(stepPin, dirPin, 158));
	return MDC.getDevice(index);
}

MIDI_Pitch *MIDI_Device_Factory::createStepper(uint8_t index, uint8_t stepPin, int8_t dirPin, int32_t maxPosition)
{
	if(!validateIndex(index))
		return NULL;
	
	//_debug.debugln(5, F("Stepper device created at index %d"), index);
	MDC.addDevice(index, new MIDI_Pitch(stepPin, dirPin, maxPosition));
	return MDC.getDevice(index);
}

MIDI_Pitch *MIDI_Device_Factory::createHDD(uint8_t index, uint8_t signalPin)
{
	if(!validateIndex(index))
		return NULL;
	
	//_debug.debugln(5, F("HDD device created at index %d"), index);
	MDC.addDevice(index, new MIDI_Pitch(signalPin));
	return MDC.getDevice(index);
}

MIDI_Pitch *MIDI_Device_Factory::createBuzzer(uint8_t index, uint8_t signalPin)
{
	if(!validateIndex(index))
		return NULL;
	
	//_debug.debugln(5, F("Buzzer device created at index %d"), index);
	MDC.addDevice(index, new MIDI_Pitch(signalPin));
	return MDC.getDevice(index);
}

MIDI_Shift_Register *MIDI_Device_Factory::createShiftRegister(uint8_t size, uint8_t startingNote, uint8_t latchPin)
{
	MDC.initializeShiftRegister(size, startingNote, latchPin);
	return MDC.getShiftRegister();
}