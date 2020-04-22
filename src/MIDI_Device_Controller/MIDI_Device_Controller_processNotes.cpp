#include "../MIDI_Device_Controller.h"

void MIDI_Device_Controller::processNotes()
{
	_debug.debugln(20, "Process start");

	//Play MIDI_Device notes
	int i = 0;
	while(i < MAX_DEVICES && _numEnabled > 0)
	{
		MIDI_Device *d = _enabledDevices[i++];
		if(!d) return;
			
		if(d->_currentNote < 0) 
		{
			_debug.debugln(20, "%d - No note", d->_id);
			continue;
		}
		
		if(d->_currentNote == 0)
		{			
			_debug.debugln(20, "%d - Resetting properties", d->_id);
			d->resetProperties();
			continue;
		}
		
		uint32_t resolution = MIDI_Periods::getResolution();		
		d->_currentDuration += resolution;
		
		if(_maxDuration != 0 && d->_currentDuration >= _maxDuration) 
		{
			_debug.debugln(20, "%d - Reached max duration", d->_id);
			d->_currentNote = 0;
			continue;
		}
		
		d->_currentTick++;
		if(d->_currentTick >= d->_currentPeriod) 
		{
			_debug.debugln(20, "%d - Toggling step", d->_id);
			
			d->toggleStep();
			d->_currentTick = 0;
		}
		
		if(d->_maxPosition > 0 && d->_currentPosition >= d->_maxPosition) 
		{
			_debug.debugln(20, "%d - Toggling direction", d->_id);
			
			d->toggleDirection();
			d->zeroPosition();
		}
	}
	
	//Play shift register notes
	if(_MSR_instance)		
		_MSR_instance->playNotes();
	else
		_debug.debugln(20, "No register device");
	
	_debug.debugln(20, "Process end");
}