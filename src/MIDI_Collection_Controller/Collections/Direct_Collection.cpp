#include "Direct_Collection.h"
#include "../MIDI_Pitch_Node.h"

bool Direct_Collection::playNote(uint8_t note)
{
	bool result = false;
	MIDI_Pitch_Node *node = start;
	
	while(node)
	{
		if(node->device->isAvailable())
		{
			node->playNote(note);
			result = true;
		}
		
		node = node->next;
	}
	
	return result;
}

void Direct_Collection::stopNote(uint8_t note)
{
	MIDI_Pitch_Node *node = start;
	
	while(node)
	{
		if(node->device->getCurrentNote() == note)
			node->stopNote();
		
		node = node->next;
	}
}