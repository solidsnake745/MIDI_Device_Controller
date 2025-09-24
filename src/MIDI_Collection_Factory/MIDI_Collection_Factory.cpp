#include "MIDI_Collection_Factory.h"

MIDI_Collection_Factory::MIDI_Collection_Factory(){}

MIDI_Collection_Factory& MIDI_Collection_Factory::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == nullptr) _instance = new MIDI_Collection_Factory();
	return *_instance;
}

MIDI_Pitch* MIDI_Collection_Factory::getDeviceFromMDC(uint8_t index)
{
	return MDC.getPitchDevice(index);
}

Base_MIDI_Pitch_Collection* MIDI_Collection_Factory::createCollection(uint8_t index, CollectionType type)
{
	_debug.debugln(1, F("Adding collection at %d"), index);
	
	if(index > MAX_COLLECTIONS - 1)
	{
		_debug.debugln(5, F("Can't add collection at index %d"), index);
		_debug.debugln(5, F("Max collection index is %d"), MAX_COLLECTIONS - 1);
		return nullptr;
	}

	if(MCC.getCollection(index) != nullptr)
	{		
		_debug.debugln(5, F("Collection already exists at index %d"), index);
		return nullptr;
	}
	
	Base_MIDI_Pitch_Collection* newCollection;
	
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
			newCollection = new Base_MIDI_Pitch_Collection(); break;
	}
	
	MCC.addCollection(index, newCollection);
	
	return newCollection;
}