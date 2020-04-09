#include "MIDI_Collection_Controller.h"

SerialDebug MIDI_Collection_Controller::_debug(DEBUG_DEVICECHAIN_CONTROLLER);

//Constructors and instance management
//_______________________________________________________________________________________________________

//Global singleton instance
MIDI_Collection_Controller MCC = MIDI_Collection_Controller::getInstance();

MIDI_Collection_Controller *MIDI_Collection_Controller::_instance = NULL;

MIDI_Collection_Controller::MIDI_Collection_Controller()
{
}

MIDI_Collection_Controller &MIDI_Collection_Controller::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Collection_Controller();
	return *_instance;
}

//Device management
//_______________________________________________________________________________________________________
Base_MIDI_Device_Collection *MIDI_Collection_Controller::_collections[MAX_COLLECTIONS];

void MIDI_Collection_Controller::printStatus()
{
	int i = 0;
	while(i != MAX_COLLECTIONS)
	{
		_debug.println(F("Collection %d"), i);
		if(_collections[i])
		{
			_debug.debugln(5, F("Populated"));
			_collections[i]->printStatus();
		}
		else
		{
			_debug.println(F("Empty"));
		}
		
		_debug.println("");
		delay(10);
		i++;
	}
}

void MIDI_Collection_Controller::addCollection(uint8_t index, Base_MIDI_Device_Collection *c)
{
	if(index > MAX_COLLECTIONS - 1)
	{
		_debug.println(F("Can't add collection at index %d"), index);
		_debug.debugln(5, F("Max collection index is %d"), MAX_COLLECTIONS - 1);
		return;
	}

	if(_collections[index] != NULL)
	{		
		_debug.println(F("Collection already exists at index %d"), index);
		return;
	}
	
	_collections[index] = c;
}

Base_MIDI_Device_Collection *MIDI_Collection_Controller::getCollection(uint8_t index)
{
	if(index > MAX_COLLECTIONS - 1)
	{
		// _debug.debugln(3, F("Max index is %d"), MAX_COLLECTIONS - 1);
		return NULL;
	}
	
	return _collections[index];
}

void MIDI_Collection_Controller::deleteCollection(uint8_t index)
{
	if(index > MAX_COLLECTIONS - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_COLLECTIONS - 1);
		return;
	}
	
	if(_collections[index])
	{
		_debug.debugln(2, F("Deleting collection at %d"), index);
		delete _collections[index];
		_collections[index] = NULL;
		return;
	}
	else
	{
		_debug.debugln(2, F("No collection at %d"), index);
	}
}

void MIDI_Collection_Controller::assignNote(int8_t index, uint8_t note)
{	
	Base_MIDI_Device_Collection *c = getCollection(index);
	if(!c) return;
	c->assignNote(note);
}

void MIDI_Collection_Controller::clearNote(int8_t index, uint8_t note)
{
	Base_MIDI_Device_Collection *c = getCollection(index);
	if(!c) return;
	c->clearNote(note);
}

void MIDI_Collection_Controller::pitchBend(int8_t index, uint16_t bend)
{
	Base_MIDI_Device_Collection *c = getCollection(index);
	if(!c) return;
	c->pitchBend(bend);
}

//Settings
//_______________________________________________________________________________________________________

//Tests/Debug
//_______________________________________________________________________________________________________
