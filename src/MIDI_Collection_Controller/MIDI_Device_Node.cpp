#include "MIDI_Device_Node.h"
#include "../MIDI_Device_Controller/MIDI_Device.h"
#include "Base_MIDI_Device_Collection.h"

SerialDebug MIDI_Device_Node::_debug(DEBUG_DEVICENODE);

MIDI_Device_Node::MIDI_Device_Node(MIDI_Device *d, Base_MIDI_Device_Collection *dc)
{
	device = d;
	parent = dc;
}

bool MIDI_Device_Node::tryAssign(uint8_t note)
{
	if(device->isAvailable())
	{
		//DEBUG2(device->getID(), F(" - Available"))
		device->assignNote(note);
		return true;
	}
	
	//DEBUG2(device->getID(), F(" - Not available"))
	return false;
}

bool MIDI_Device_Node::tryClear(uint8_t note)
{
	if(device->getCurrentNote() == note)
	{
		//DEBUG2(device->getID(), F(" - Matches"))
		device->clearNote();
		return true;
	}
	
	//DEBUG3(device->getID(), F(" - Doesn't match: "), device->getCurrentNote())
	return false;
}

void MIDI_Device_Node::assignNote(uint8_t note)
{
	device->assignNote(note);	
	// lastAssignStamp = millis();
}

void MIDI_Device_Node::clearNote()
{
	device->clearNote();	
	// lastAssignStamp = 0;
}