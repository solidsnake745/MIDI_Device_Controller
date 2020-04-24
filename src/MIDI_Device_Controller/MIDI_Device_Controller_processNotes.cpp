#include "../MIDI_Device_Controller.h"

void MIDI_Device_Controller::processNotes()
{
	_debug.debugln(20, "Process start");

	//Play MIDI_Device notes
	int i = 0;
	while(i < _numEnabled && _numEnabled > 0)
	{
		MIDI_Device *d = _enabledDevices[i++];
		if(!d) continue;
		d->playNotes();
	}
	
	//Play shift register notes
	if(_MSR_instance)		
		_MSR_instance->playNotes();
	else
		_debug.debugln(20, "No register device");
	
	_debug.debugln(20, "Process end");
}