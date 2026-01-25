#include "MIDI_Pitch_Node.h"
#include "../MIDI_Pitch/Base_MIDI_Pitch.h"
#include "Base_MIDI_Pitch_Collection.h"

MIDI_Pitch_Node::MIDI_Pitch_Node(Base_MIDI_Pitch* d, Base_MIDI_Pitch_Collection* dc)
{
	_device = d;
	_parent = dc;
}

void MIDI_Pitch_Node::playNote(uint8_t note)
{
	_device->playNote(note, _parent);	
	// lastAssignStamp = millis();
}

void MIDI_Pitch_Node::bendNoteByFactor(float pitchFactor)
{
	if(_device->_lastAssignedBy == _parent)
		_device->bendNoteByFactor(pitchFactor);
}

void MIDI_Pitch_Node::stopNote()
{
	if(_device->_lastAssignedBy == _parent)
		_device->stopNote();	
	// lastAssignStamp = 0;
}

bool MIDI_Pitch_Node::tryPlayNote(uint8_t note)
{
	if(_device->isAvailable())
	{
		//DEBUG2(device->getID(), F(" - Available"))
		_device->playNote(note, _parent);
		return true;
	}
	
	//DEBUG2(device->getID(), F(" - Not available"))
	return false;
}

bool MIDI_Pitch_Node::tryStopNote(uint8_t note)
{
	if(_device->getCurrentNote() == note && _device->_lastAssignedBy == _parent)
	{
		//DEBUG2(device->getID(), F(" - Matches"))
		_device->stopNote();
		return true;
	}
	
	//DEBUG3(device->getID(), F(" - Doesn't match: "), device->getCurrentNote())
	return false;
}