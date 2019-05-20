#include "../DeviceChain_Controller.h"

//Constructors and instance management
//_______________________________________________________________________________________________________

//Global singleton instance
DeviceChain_Controller DCC = DeviceChain_Controller::getInstance();

DeviceChain_Controller *DeviceChain_Controller::_instance = NULL;

DeviceChain_Controller::DeviceChain_Controller()
{
	
}

DeviceChain_Controller &DeviceChain_Controller::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new DeviceChain_Controller();
	return *_instance;
}

//Device management
//_______________________________________________________________________________________________________
DeviceChain_Base *DeviceChain_Controller::_chains[MAX_CHAINS];

void DeviceChain_Controller::printStatus()
{
	int i = 0;
	while(i != MAX_CHAINS)
	{
		if(_chains[i])
		{
		}
		else
		{
		}
		
		delay(10);
		i++;
	}
}

void DeviceChain_Controller::addChain(uint8_t index, DeviceChain_Base *c)
{
	if(index > MAX_CHAINS - 1)
	{
		// _debug.println(F("Can't add device with ID %d"), id);
		// _debug.debugln(5, F("Max device ID is %d"), MAX_CHAINS - 1);
		return;
	}

	if(_chains[index] != NULL)
	{		
		// _debug.println(F("Device slot %d is already populated"), id);
		return;
	}
	
	_chains[index] = c;
}

DeviceChain_Base *DeviceChain_Controller::getChain(uint8_t index)
{
	if(index > MAX_CHAINS - 1)
	{
		// _debug.debugln(3, F("Max index is %d"), MAX_CHAINS - 1);
		return NULL;
	}
	
	return _chains[index];
}

void DeviceChain_Controller::deleteChain(uint8_t index)
{
	if(index > MAX_CHAINS - 1)
	{
		// _debug.debugln(3, F("Max index is %d"), MAX_CHAINS - 1);
		return;
	}
	
	if(_chains[index])
	{
		// _debug.debugln(2, F("Removing device at %d"), index);
		delete _chains[index];
		_chains[index] = NULL;
		return;
	}
	else
	{
		// _debug.debugln(2, F("No device at %d"), index);
	}
}

void DeviceChain_Controller::assignNote(int8_t index, uint8_t note)
{	
	DeviceChain_Base *c = getChain(index);
	if(!c) return;
	c->assignNote(note);
}

void DeviceChain_Controller::clearNote(int8_t index, uint8_t note)
{
	DeviceChain_Base *c = getChain(index);
	if(!c) return;
	c->clearNote(note);
}

void DeviceChain_Controller::pitchBend(int8_t index, uint16_t bend)
{
	DeviceChain_Base *c = getChain(index);
	if(!c) return;
	c->pitchBend(bend);
}

//Settings
//_______________________________________________________________________________________________________

//Tests/Debug
//_______________________________________________________________________________________________________
