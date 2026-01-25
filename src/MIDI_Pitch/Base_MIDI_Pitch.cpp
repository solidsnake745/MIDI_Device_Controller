#include "Base_MIDI_Pitch.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

void Base_MIDI_Pitch::playNote(uint8_t note, void* assignedBy)
{
	if(!isEnabled()) 
	{
		_debug.debugln(7, F("%d - Not enabled"), _id);
		return;
	}
	
	_currentNote = note;
	_currentPeriod = getBasePeriod();
	_lastAssignedBy = assignedBy;
	
	_debug.debugln(7, F("%d - Note %d (%d) assigned"), _id, _currentNote, _currentPeriod);
	
	if(_parent) _parent->noteAssigned();	
}

void Base_MIDI_Pitch::playPeriod(uint32_t period, void* assignedBy)
{
	if(!isEnabled()) 
	{
		_debug.debugln(7, F("%d - Not enabled"), _id);
		return;
	}
	
	_currentNote = 255;	
	_currentPeriod = period;
	_lastAssignedBy = assignedBy;
	
	_debug.debugln(7, F("%d - Period %d assigned"), _id, _currentPeriod);

	if(_parent) _parent->noteAssigned();
}