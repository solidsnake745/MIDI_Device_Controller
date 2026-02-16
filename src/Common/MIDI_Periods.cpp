#include "MIDI_Periods.h"

void MIDI_Periods::calculatePeriods()
{
	//Calculate actual periods to be used in operation
	//There is some inaccuracy in this conversion, but that's OK
	//We need to change state twice per period so we double the resolution
	//Some devices do not need to do this, but we just need to double it again if so
	uint32_t dblResolution = 2 * _currentResolution;
	
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
		_debug.println(F("Invalid resolution set: %d; defaulting to: %d"), resolution, DEFAULT_RESOLUTION);
		_currentResolution = DEFAULT_RESOLUTION;
	}
	else
		_currentResolution = resolution;
	
	calculatePeriods();
}

uint32_t MIDI_Periods::getOriginalPeriod(uint16_t index) { return pgm_read_dword(ORIGINAL_PERIODS + index); };
void MIDI_Periods::printOriginalPeriod(uint8_t note) { _debug.println(F("Original period for %d: %d"), note, getOriginalPeriod(note)); }
void MIDI_Periods::printOriginalPeriods() { for(uint8_t i = 0; i < 128; i++) printOriginalPeriod(i); }
void MIDI_Periods::printCalculatedPeriod(uint8_t note) { _debug.println(F("Calculated period for %d: %d"), note, calculatedPeriods[note]); }
void MIDI_Periods::printCalculatedPeriods() { for(uint8_t i = 0; i < 128; i++) printCalculatedPeriod(i); }