#include "MIDI_Chain_Factory.h"

//Global singleton instance
MIDI_Chain_Factory MCF = MIDI_Chain_Factory::getInstance();

MIDI_Chain_Factory *MIDI_Chain_Factory::_instance = NULL;

MIDI_Chain_Factory::MIDI_Chain_Factory()
{
}

MIDI_Chain_Factory &MIDI_Chain_Factory::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Chain_Factory();
	return *_instance;
}

void MIDI_Chain_Factory::createChain(uint8_t index, ChainType_t type, uint8_t deviceIndexes[])
{
	if(index > MAX_CHAINS - 1)
	{
		// _debug.println(F("Can't add chain at index %d"), index);
		// _debug.debugln(5, F("Max chain index is %d"), MAX_CHAINS - 1);
		return;
	}

	if(MCC.getChain(index) != NULL)
	{		
		// _debug.println(F("Chain already exists at index %d"), index);
		return;
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
	
	//Disable warning for sizeof on an array
	//We know we want the total size of the array to figure out how many elements it contains
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsizeof-array-argument"
	uint8_t numDevices = sizeof(deviceIndexes) / sizeof(uint8_t);
#pragma GCC diagnostic pop
	
	if(numDevices > MAX_DEVICES)
		numDevices = MAX_DEVICES;

	// _debug.debugln(5, F("Attempting to add %d device(s)"), numDevices);
	
	int i = 0;
	while(i != numDevices)
		newChain->addDevice(MDC.getDevice(deviceIndexes[i++]));
	
	MCC.addChain(index, newChain);
}