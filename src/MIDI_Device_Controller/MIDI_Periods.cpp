#include "MIDI_Periods.h"

//Used to update and maintain calculatedPeriods with _currentResolution
//Sets resolution and computes adjusted resolution to be used for note processing
void MIDI_Periods::calculatePeriods(uint16_t resolution)
{
	//Calculate actual periods to be used in operation
	//There is some inaccuracy in this conversion, but that's OK
	uint32_t dblResolution = 2 * _currentResolution; //We need to change state twice per period
	
	for(uint8_t i = 0; i < 128; i++) 
	{
		uint32_t basePeriod = getOriginalPeriod(i);	
		
		//If our resolution is greater than the period, the note will never play
		//Disable that note by assigning 0, else calculate the corresponding period
		calculatedPeriods[i] = basePeriod < dblResolution ? 0 : (basePeriod / dblResolution);
	}
}

void MIDI_Periods::setResolution(uint32_t resolution) 
{
	if(resolution < MIN_RESOLUTION || resolution > MAX_RESOLUTION)
	{
		_debug.debugln(1, F("Invalid resolution set: %d"), resolution);
		_debug.println(F("Defaulting to: %d"), DEFAULT_RESOLUTION);
		_currentResolution = DEFAULT_RESOLUTION;
	}
	else
		_currentResolution = resolution;
	
	calculatePeriods(resolution);
}