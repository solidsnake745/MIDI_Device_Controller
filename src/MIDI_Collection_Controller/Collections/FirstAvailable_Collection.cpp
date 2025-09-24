#include "FirstAvailable_Collection.h"
#include "../MIDI_Pitch_Node.h"

bool FirstAvailable_Collection::playNote(uint8_t note)
{
	MIDI_Pitch_Node* node = start;
	while(node)
	{
		if(node->tryPlayNote(note))
			return true;
		
		node = node->next;
	}
	
	return false;
}

void FirstAvailable_Collection::stopNote(uint8_t note)
{	
	MIDI_Pitch_Node* node = start;
	while(node)
	{
		if(node->tryStopNote(note))
			return;
		
		node = node->next;
	}
}