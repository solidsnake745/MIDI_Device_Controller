#include "FirstAvailable_Collection.h"

bool FirstAvailable_Collection::playNote(uint8_t note)
{
	MIDI_Pitch_Node* node = start;
	while(node)
	{
		if(node->tryPlayNote(note))
			return true;
		
		node = node->getNextNode();
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
		
		node = node->getNextNode();
	}
}