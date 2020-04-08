#include "FirstAvailable_Chain.h"
#include "../MIDI_Device_Node.h"

bool FirstAvailable_Chain::assignNote(uint8_t note)
{
	MIDI_Device_Node *node = start;
	
	while(node)
	{
		if(node->device->isAvailable())
		{
			node->assignNote(note);
			return true;
		}
		
		node = node->next;
	}
	
	return false;
}

void FirstAvailable_Chain::clearNote(uint8_t note)
{	
	MIDI_Device_Node *node = start;
	
	while(node)
	{
		if(node->device->getCurrentNote() == note)
		{
			node->clearNote();
			return;
		}
		
		node = node->next;
	}
}