#include "MIDI_Pitch_Node.h"
#include "../MIDI_Devices/MIDI_Pitch.h"
#include "Base_MIDI_Pitch_Collection.h"

SerialDebug MIDI_Pitch_Node::_debug(DEBUG_DEVICENODE);

MIDI_Pitch_Node::MIDI_Pitch_Node(MIDI_Pitch *d, Base_MIDI_Pitch_Collection *dc)
{
	device = d;
	parent = dc;
}

void MIDI_Pitch_Node::playNote(uint8_t note)
{
	device->playNote(note);	
	// lastAssignStamp = millis();
}

void MIDI_Pitch_Node::stopNote()
{
	device->stopNote();	
	// lastAssignStamp = 0;
}

bool MIDI_Pitch_Node::tryPlayNote(uint8_t note)
{
	if(device->isAvailable())
	{
		//DEBUG2(device->getID(), F(" - Available"))
		device->playNote(note);
		return true;
	}
	
	//DEBUG2(device->getID(), F(" - Not available"))
	return false;
}

bool MIDI_Pitch_Node::tryStopNote(uint8_t note)
{
	if(device->getCurrentNote() == note)
	{
		//DEBUG2(device->getID(), F(" - Matches"))
		device->stopNote();
		return true;
	}
	
	//DEBUG3(device->getID(), F(" - Doesn't match: "), device->getCurrentNote())
	return false;
}