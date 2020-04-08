#include "MIDI_Chain_Controller.h"

SerialDebug MIDI_Chain_Controller::_debug(DEBUG_DEVICECHAIN_CONTROLLER);

//Constructors and instance management
//_______________________________________________________________________________________________________

//Global singleton instance
MIDI_Chain_Controller MCC = MIDI_Chain_Controller::getInstance();

MIDI_Chain_Controller *MIDI_Chain_Controller::_instance = NULL;

MIDI_Chain_Controller::MIDI_Chain_Controller()
{
}

MIDI_Chain_Controller &MIDI_Chain_Controller::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Chain_Controller();
	return *_instance;
}

//Device management
//_______________________________________________________________________________________________________
MIDI_Device_Chain *MIDI_Chain_Controller::_chains[MAX_CHAINS];

void MIDI_Chain_Controller::printStatus()
{
	int i = 0;
	while(i != MAX_CHAINS)
	{
		_debug.println(F("Chain %d"), i);
		if(_chains[i])
		{
			_debug.debugln(5, F("Populated"));
			_chains[i]->printStatus();
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

void MIDI_Chain_Controller::addChain(uint8_t index, MIDI_Device_Chain *c)
{
	if(index > MAX_CHAINS - 1)
	{
		_debug.println(F("Can't add chain at index %d"), index);
		_debug.debugln(5, F("Max chain index is %d"), MAX_CHAINS - 1);
		return;
	}

	if(_chains[index] != NULL)
	{		
		_debug.println(F("Chain already exists at index %d"), index);
		return;
	}
	
	_chains[index] = c;
}

MIDI_Device_Chain *MIDI_Chain_Controller::getChain(uint8_t index)
{
	if(index > MAX_CHAINS - 1)
	{
		// _debug.debugln(3, F("Max index is %d"), MAX_CHAINS - 1);
		return NULL;
	}
	
	return _chains[index];
}

void MIDI_Chain_Controller::deleteChain(uint8_t index)
{
	if(index > MAX_CHAINS - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_CHAINS - 1);
		return;
	}
	
	if(_chains[index])
	{
		_debug.debugln(2, F("Deleting chain at %d"), index);
		delete _chains[index];
		_chains[index] = NULL;
		return;
	}
	else
	{
		_debug.debugln(2, F("No chain at %d"), index);
	}
}

void MIDI_Chain_Controller::assignNote(int8_t index, uint8_t note)
{	
	MIDI_Device_Chain *c = getChain(index);
	if(!c) return;
	c->assignNote(note);
}

void MIDI_Chain_Controller::clearNote(int8_t index, uint8_t note)
{
	MIDI_Device_Chain *c = getChain(index);
	if(!c) return;
	c->clearNote(note);
}

void MIDI_Chain_Controller::pitchBend(int8_t index, uint16_t bend)
{
	MIDI_Device_Chain *c = getChain(index);
	if(!c) return;
	c->pitchBend(bend);
}

//Settings
//_______________________________________________________________________________________________________

//Tests/Debug
//_______________________________________________________________________________________________________
