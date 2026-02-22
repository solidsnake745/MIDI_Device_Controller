#include "MIDI_Collection_Controller.h"

MIDI_Collection_Controller& MIDI_Collection_Controller::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == nullptr) _instance = new MIDI_Collection_Controller();
	return *_instance;
}

void MIDI_Collection_Controller::printStatus()
{
	int i = 0;
	while(i != MAX_COLLECTIONS)
	{
		_debug.println(F("Collection %d"), i);
		if(_collections[i])
		{
			_debug.debugln(DEBUG, F("Populated"));
			_collections[i]->printStatus();
		}
		else
		{
			_debug.println(F("Empty"));
		}
		
		_debug.println();
		delay(10);
		i++;
	}
}

void MIDI_Collection_Controller::addCollection(uint8_t index, Base_MIDI_Pitch_Collection* c)
{
	if(index > MAX_COLLECTIONS - 1)
	{
		_debug.println(F("Can't add collection at index %d; max is %d"), index, MAX_COLLECTIONS - 1);
		return;
	}

	if(_collections[index] != nullptr)
	{		
		_debug.println(F("Collection already exists at index %d"), index);
		return;
	}
	
	_collections[index] = c;
	_debug.println(F("Collection added at index %d"), index);
}

Base_MIDI_Pitch_Collection* MIDI_Collection_Controller::getCollection(uint8_t index)
{
	if(index > MAX_COLLECTIONS - 1)
	{
		_debug.println(F("No collection at index %d; max is %d"), index, MAX_COLLECTIONS - 1);
		return nullptr;
	}
	
	return _collections[index];
}

// void MIDI_Collection_Controller::deleteCollection(uint8_t index)
// {
	// if(index > MAX_COLLECTIONS - 1)
	// {
		// _debug.debugln(3, F("Max index is %d"), MAX_COLLECTIONS - 1);
		// return;
	// }
	
	// if(_collections[index])
	// {
		// _debug.debugln(2, F("Deleting collection at %d"), index);
		// delete _collections[index];
		// _collections[index] = nullptr;
		// return;
	// }
	// else
	// {
		// _debug.debugln(2, F("No collection at %d"), index);
	// }
// }

void MIDI_Collection_Controller::playNote(uint8_t index, uint8_t note)
{
	Base_MIDI_Pitch_Collection* c = getCollection(index);
	if(!c) return;
	c->playNote(note);
}

void MIDI_Collection_Controller::bendNote(uint8_t index, int16_t bend, bool shiftRange)
{
	Base_MIDI_Pitch_Collection* c = getCollection(index);
	if(!c) return;
	c->bendNote(bend, shiftRange);
}

void MIDI_Collection_Controller::stopNote(uint8_t index, uint8_t note)
{
	Base_MIDI_Pitch_Collection* c = getCollection(index);
	if(!c) return;
	c->stopNote(note);
}

void MIDI_Collection_Controller::reset(uint8_t index)
{
	Base_MIDI_Pitch_Collection* c = getCollection(index);
	if(!c) return;
	c->reset();
}

void MIDI_Collection_Controller::resetAll()
{
	int i = 0;
	while(i != MAX_COLLECTIONS)
		reset(i++);
}