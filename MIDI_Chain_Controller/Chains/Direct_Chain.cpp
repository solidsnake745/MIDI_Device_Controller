#include "Direct_Chain.h"
#include "../MIDI_Device_Node.h"

bool Direct_Chain::assignNote(uint8_t note)
{
	bool result = false;
	MIDI_Device_Node *node = start;
	
	while(node)
	{
		if(node->device->isAvailable())
		{
			node->assignNote(note);
			result = true;
		}
		
		node = node->next;
	}
	
	return result;
}

void Direct_Chain::clearNote(uint8_t note)
{
	MIDI_Device_Node *node = start;
	
	while(node)
	{
		if(node->device->getCurrentNote() == note)
			node->clearNote();
		
		node = node->next;
	}
}