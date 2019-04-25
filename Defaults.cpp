#include "Defaults.h"

SerialDebug Defaults::_debug;
constexpr int8_t Defaults::STEPPIN_DEFAULTS[];
constexpr int8_t Defaults::DIRPIN_DEFAULTS[];
constexpr int16_t Defaults::MAXPOSITION_DEFAULTS[];

int8_t Defaults::getStepDefault(uint8_t device) 
{ 
	return pgm_read_word(STEPPIN_DEFAULTS + device); 
}

int8_t Defaults::getDirDefault(uint8_t device) 
{ 
	return pgm_read_word(DIRPIN_DEFAULTS + device); 
}

int16_t Defaults::getMaxPosDefault(uint8_t device) 
{ 
	return pgm_read_word(MAXPOSITION_DEFAULTS + device); 
}

void Defaults::printStepDefault(uint8_t device)
{
	_debug.println(F("Device %d default step pin: %d"), device, getStepDefault(device));
}

void Defaults::printStepDefaults()
{
	for(uint8_t i = 0; i < MAX_DEFAULTS; i++)
		printStepDefault(i);
}

void Defaults::printDirDefault(uint8_t device)
{
	// _debug.println(F("Device %d default direction pin: %d"), device, getDirDefault(device));
}

void Defaults::printDirDefaults()
{
	for(uint8_t i = 0; i < MAX_DEFAULTS; i++)	
		printDirDefault(i);
}

void Defaults::printMaxPositionDefault(uint8_t device)
{
	// _debug.println(F("Device %d default max position: %d"), device, getMaxPosDefault(device));
}

void Defaults::printMaxPositionDefaults()
{
	for(uint8_t i = 0; i < MAX_DEFAULTS; i++)	
		printMaxPositionDefault(i);
}	