#include "MIDI_Periods.h"

#if MIDI_Periods_h_DEBUG > 0
	#include "SerialDebug.h"
	#define PERIODS_PRINT(x) SDBG.println(x);
	#define PERIODS_PRINT(x, args...) SDBG.println(x, args);
	#define PERIODS_DEBUG(x, y, args...) SDBG.debugln(x, y, args);	
#else
	#define PERIODS_PRINT(x)
	#define PERIODS_PRINT(x, args...)
	#define PERIODS_DEBUG(x, y, args...)
#endif

uint16_t MIDI_Periods::getOriginalPeriod(uint16_t index) 
{ 
	return pgm_read_dword(ORIGINAL_PERIODS + index); 
}

//Used to update and maintain calculatedPeriods with _currentResolution
//Sets resolution and computes adjusted resolution to be used for note processing
void MIDI_Periods::calculatePeriods(uint16_t resolution)
{
	if(resolution < MIN_RESOLUTION || resolution > MAX_RESOLUTION)
	{			
		PERIODS_DEBUG(1, F("Invalid resolution set: %d"), resolution)
		PERIODS_DEBUG(2, F("Defaulting to: %d"), DEFAULT_RESOLUTION)
		_currentResolution = DEFAULT_RESOLUTION;
	}
	else
		_currentResolution = resolution;
	
	//Calculate actual periods to be used in operation
	//There is some inaccuracy in this conversion, but that's OK
	uint32_t dblResolution = 2 * getResolution(); //We need to change state twice per period
	
	for(uint8_t i = 0; i < 128; i++) 
	{
		uint32_t basePeriod = getOriginalPeriod(i);	
		
		//If our resolution is greater than the period, the note will never play
		//Disable that note by assigning 0, else calculate the corresponding period
		calculatedPeriods[i] = basePeriod < dblResolution ? 0 : (basePeriod / dblResolution);
	}
}

uint16_t MIDI_Periods::calculatedPeriods[];
constexpr uint32_t MIDI_Periods::ORIGINAL_PERIODS[];
uint32_t MIDI_Periods::_currentResolution;

uint32_t MIDI_Periods::getResolution() 
{
	return _currentResolution;
}

void MIDI_Periods::setResolution(uint32_t resolution = DEFAULT_RESOLUTION) 
{ 
	calculatePeriods(resolution);
}

void MIDI_Periods::printOriginalPeriod(uint8_t note)
{
	PERIODS_DEBUG(1, F("Original period for %d: %d"), note, getOriginalPeriod(note))
}	

void MIDI_Periods::printOriginalPeriods()
{
	for(uint8_t i = 0; i < 128; i++)	
		printOriginalPeriod(i);
}

void MIDI_Periods::printCalculatedPeriod(uint8_t note)
{
	PERIODS_DEBUG(1, F("Calculated period for %d: %d"), note, calculatedPeriods[note])
}

void MIDI_Periods::printCalculatedPeriods()
{
	for(uint8_t i = 0; i < 128; i++)	
		printCalculatedPeriod(i);
}