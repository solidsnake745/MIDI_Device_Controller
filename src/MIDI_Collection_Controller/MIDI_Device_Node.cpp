#include "MIDI_Device_Node.h"
#include "../MIDI_Device_Controller/MIDI_Device.h"
#include "Base_MIDI_Device_Collection.h"

SerialDebug MIDI_Device_Node::_debug(DEBUG_DEVICENODE);

MIDI_Device_Node::MIDI_Device_Node(MIDI_Device *d, Base_MIDI_Device_Collection *dc)
{
	device = d;
	parent = dc;
}

void MIDI_Device_Node::playNote(uint8_t note)
{
	device->playNote(note);	
	// lastAssignStamp = millis();
}

void MIDI_Device_Node::stopNote()
{
	device->stopNote();	
	// lastAssignStamp = 0;
}

bool MIDI_Device_Node::tryPlayNote(uint8_t note)
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

bool MIDI_Device_Node::tryStopNote(uint8_t note)
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