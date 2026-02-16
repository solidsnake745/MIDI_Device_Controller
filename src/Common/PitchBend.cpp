#include "PitchBend.h"

float PitchBend::calculateFactor(int16_t bend, bool shiftRange)
{
	//Expecting values in the range of -8192 to 8191
	//Depending on the framework supplying MIDI data, values could instead range from 0 to 16383
	//Shift them down to the expected range for this to work
	if(shiftRange) bend -= 8192;
	
	#if BEND_MODE == 0
		//Original formula I used in past versions which assumes BEND_PER_OCTAVE = 1
		//Has worked for all my purposes
		float result = pow(2.0, bend / 8192.0);
	#elif BEND_MODE == 1
		//More technically correct formula for calculating pitchbends, configurable through Settings.h
		float result = pow(2.0, (BEND_PER_OCTAVE * (bend / 8192.0)));
	#else
		//Default in case some invalid value is specified
		float result = pow(2.0, bend / 8192.0);
	#endif
	return result;
}