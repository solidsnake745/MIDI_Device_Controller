#include "MIDI_Pitch_Node.h"

MIDI_Pitch_Node::MIDI_Pitch_Node(Base_MIDI_Pitch* d, Base_MIDI_Pitch_Collection *dc)
{
	_device = d;
	_parent = dc;
}


MIDI_Pitch_Node* const MIDI_Pitch_Node::getNextNode() { return next; }

MIDI_Pitch_Node* const MIDI_Pitch_Node::getPrevNode() { return prev; }

void MIDI_Pitch_Node::playNote(uint8_t note) { _device->playNote(note, _parent); }

void MIDI_Pitch_Node::bendNoteByFactor(float pitchFactor)
{
	if(_device->_lastAssignedBy == _parent)
		_device->bendNoteByFactor(pitchFactor);
}

void MIDI_Pitch_Node::stopNote() 
{
	if(_device->_lastAssignedBy == _parent)
		_device->stopNote();
}

bool MIDI_Pitch_Node::tryPlayNote(uint8_t note)
{
	if(_device->isAvailable())
	{
		_device->playNote(note, _parent);
		return true;
	}
	
	return false;
}

bool MIDI_Pitch_Node::tryStopNote(uint8_t note)
{
	if(_device->getCurrentNote() == note && _device->_lastAssignedBy == _parent)
	{
		_device->stopNote();
		return true;
	}
	
	return false;
}