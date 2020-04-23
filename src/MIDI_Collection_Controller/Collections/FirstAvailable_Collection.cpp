#include "FirstAvailable_Collection.h"
#include "../MIDI_Device_Node.h"

bool FirstAvailable_Collection::playNote(uint8_t note)
{
	MIDI_Device_Node *node = start;
	
	while(node)
	{
		if(node->device->isAvailable())
		{
			node->playNote(note);
			return true;
		}
		
		node = node->next;
	}
	
	return false;
}

void FirstAvailable_Collection::stopNote(uint8_t note)
{	
	MIDI_Device_Node *node = start;
	
	while(node)
	{
		if(node->device->getCurrentNote() == note)
		{
			node->stopNote();
			return;
		}
		
		node = node->next;
	}
}