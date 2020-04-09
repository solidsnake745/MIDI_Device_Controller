#include "MIDI_Collection_Factory.h"

SerialDebug MIDI_Collection_Factory::_debug(DEBUG_DEVICECOLLECTION_FACTORY);

//Global singleton instance
MIDI_Collection_Factory MCF = MIDI_Collection_Factory::getInstance();

MIDI_Collection_Factory *MIDI_Collection_Factory::_instance = NULL;

MIDI_Collection_Factory::MIDI_Collection_Factory(){}

MIDI_Collection_Factory &MIDI_Collection_Factory::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Collection_Factory();
	return *_instance;
}

MIDI_Device *MIDI_Collection_Factory::getDeviceFromMDC(uint8_t index)
{
	return MDC.getDevice(index);
}

Base_MIDI_Device_Collection *MIDI_Collection_Factory::createInitialCollection(uint8_t index, CollectionType type)
{
	_debug.debugln(1, F("Adding collection at %d"), index);
	
	if(index > MAX_COLLECTIONS - 1)
	{
		_debug.debugln(5, F("Can't add collection at index %d"), index);
		_debug.debugln(5, F("Max collection index is %d"), MAX_COLLECTIONS - 1);
		return NULL;
	}

	if(MCC.getCollection(index) != NULL)
	{		
		_debug.debugln(5, F("Collection already exists at index %d"), index);
		return NULL;
	}
	
	Base_MIDI_Device_Collection *newCollection;
	
	switch(type)
	{
		case Direct:
			newCollection = new Direct_Collection(); break;			
		case FirstAvailable:
			newCollection = new FirstAvailable_Collection(); break;			
		case RoundRobin:
			newCollection = new RoundRobin_Collection(); break;			
		default:
			// _debug.debugln(5, F("Defaulting to base device Collection"));
			newCollection = new Base_MIDI_Device_Collection(); break;
	}
	
	MCC.addCollection(index, newCollection);
	
	return newCollection;
}