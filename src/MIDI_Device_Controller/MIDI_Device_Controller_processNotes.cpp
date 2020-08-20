#include "../MIDI_Device_Controller.h"

void MIDI_Device_Controller::processNotes()
{
	_debug.debugln(20, F("Process start"));

	//Play MIDI_Pitch notes
	int i = 0;
	while(i < _numEnabled && _numEnabled > 0)
	{
		MIDI_Pitch *d = _enabledPitchDevices[i++];
		if(!d) continue;
		d->playNotes();
	}
	
	//Play MIDI_Shift_Register notes
	if(_MSR_instance)		
		_MSR_instance->playNotes();
	else
		_debug.debugln(20, F("No register device"));
	
	_debug.debugln(20, F("Process end"));
}