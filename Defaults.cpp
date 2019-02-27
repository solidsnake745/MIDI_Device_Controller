#include "Defaults.h"

#if Defaults_h_DEBUG > 0
	#include "SerialDebug.h"
	#define DEFAULTS_PRINT(x) SDBG.println(x);
	#define DEFAULTS_PRINT(x, args...) SDBG.println(x, args);
	#define DEFAULTS_DEBUG(x, y, args...) SDBG.debugln(x, y, args);	
#else
	#define DEFAULTS_PRINT(x)
	#define DEFAULTS_PRINT(x, args...)
	#define DEFAULTS_DEBUG(x, y, args...)
#endif
	
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
	DEFAULTS_PRINT(F("Device %d default step pin: %d"), device, getStepDefault(device))
}

void Defaults::printStepDefaults()
{
	for(uint8_t i = 0; i < MAX_DEFAULTS; i++)
		printStepDefault(i);
}

void Defaults::printDirDefault(uint8_t device)
{
	DEFAULTS_PRINT(F("Device %d default direction pin: %d"), device, getDirDefault(device))			
}

void Defaults::printDirDefaults()
{
	for(uint8_t i = 0; i < MAX_DEFAULTS; i++)	
		printDirDefault(i);
}

void Defaults::printMaxPositionDefault(uint8_t device)
{
	DEFAULTS_PRINT(F("Device %d default max position: %d"), device, getMaxPosDefault(device))			
}

void Defaults::printMaxPositionDefaults()
{
	for(uint8_t i = 0; i < MAX_DEFAULTS; i++)	
		printMaxPositionDefault(i);
}	