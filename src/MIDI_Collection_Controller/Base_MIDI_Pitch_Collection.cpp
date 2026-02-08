#include "Base_MIDI_Pitch_Collection.h"
#include "../MIDI_Device_Controller.h" //Need the definition of methods

void Base_MIDI_Pitch_Collection::testPitchBend()
{
	bool currentSetting = MDC.isAutoPlayEnabled();
	MDC.setAutoPlay(true);
	
	for(uint8_t c = 0; c < _count; c++)
		playNote(MIDDLE_C_NOTE);

	bendNote(0);
	delayMicroseconds(500);
	
	for(int16_t i = -8192; i <= 8191; i++)
	{
		bendNote(i);
		delayMicroseconds(500);
	}
	
	for(uint8_t c = 0; c < _count; c++)
		stopNote(MIDDLE_C_NOTE);
	
	MDC.stopPlaying();
	MDC.setAutoPlay(currentSetting);
};