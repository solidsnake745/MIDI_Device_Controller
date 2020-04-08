#include "MIDI_Chain_Factory.h"

SerialDebug MIDI_Chain_Factory::_debug(DEBUG_DEVICECHAIN_FACTORY);

//Global singleton instance
MIDI_Chain_Factory MCF = MIDI_Chain_Factory::getInstance();

MIDI_Chain_Factory *MIDI_Chain_Factory::_instance = NULL;

MIDI_Chain_Factory::MIDI_Chain_Factory(){}

MIDI_Chain_Factory &MIDI_Chain_Factory::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Chain_Factory();
	return *_instance;
}

MIDI_Device *MIDI_Chain_Factory::getDeviceFromMDC(uint8_t index)
{
	return MDC.getDevice(index);
}

MIDI_Device_Chain *MIDI_Chain_Factory::createInitialChain(uint8_t index, ChainType_t type)
{
	if(index > MAX_CHAINS - 1)
	{
		_debug.debugln(5, F("Can't add chain at index %d"), index);
		_debug.debugln(5, F("Max chain index is %d"), MAX_CHAINS - 1);
		return NULL;
	}

	if(MCC.getChain(index) != NULL)
	{		
		_debug.debugln(5, F("Chain already exists at index %d"), index);
		return NULL;
	}
	
	MIDI_Device_Chain *newChain;
	
	switch(type)
	{
		case Direct:
			newChain = new Direct_Chain(); break;			
		case FirstAvailable:
			newChain = new FirstAvailable_Chain(); break;			
		case RoundRobin:
			newChain = new RoundRobin_Chain(); break;			
		default:
			// _debug.debugln(5, F("Defaulting to base device chain"));
			newChain = new MIDI_Device_Chain(); break;
	}
	
	MCC.addChain(index, newChain);
	
	return newChain;
}