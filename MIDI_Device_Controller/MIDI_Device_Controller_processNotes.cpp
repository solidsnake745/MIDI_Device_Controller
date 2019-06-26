#include "../MIDI_Device_Controller.h"

void MIDI_Device_Controller::processNotes()
{	
	#if DEBUG_MODE >= 5
	DEBUG(F("Process start"))
	#endif

	int i = 0;
	while(i < MAX_DEVICES)
	{
		MIDI_Device *d = _enabledDevices[i++];
		if(!d) return;
			
		if(d->_currentNote < 0) 
		{
			#if DEBUG_MODE >= 5
				DEBUG3(d->_id, F(" - No note"), d->_currentNote)
			#endif
			continue;
		}
		
		if(d->_currentNote == 0)
		{
			#if DEBUG_MODE >= 5
				DEBUG2(d->_id, F(" - Waiting for reset"))
			#endif
			d->resetProperties();
			continue;
		}
		
		uint32_t resolution = MIDI_Periods::getResolution();		
		d->_currentDuration += resolution;
		
		if(_maxDuration != 0 && d->_currentDuration >= _maxDuration) 
		{
			#if DEBUG_MODE >= 2
				DEBUG2(d->_id, F(" - Reached max duration"))
				DEBUG2(F("Max Duration: "), _maxDuration)
			#endif
			
			d->_currentNote = 0;
			continue;
		}
		
		d->_currentTick++;
		if(d->_currentTick >= d->_currentPeriod) 
		{
			#if DEBUG_MODE >= 5
				DEBUG2(d->_id, F(" - Toggling step"))
			#endif
			
			d->toggleStep();
			d->_currentTick = 0;
		}
		
		if(d->_maxPosition > 0 && d->_currentPosition >= d->_maxPosition) 
		{
			#if DEBUG_MODE >= 5
			DEBUG2(d->_id, F(" - Toggling direction"))
			#endif
			
			d->toggleDirection();
			d->zeroPosition();
		}
	}
	
	#if DEBUG_MODE >= 5
	DEBUG(F("Process end"))
	#endif
}