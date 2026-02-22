#include "Direct_Collection.h"

bool Direct_Collection::playNote(uint8_t note)
{
	bool result = false;
	MIDI_Pitch_Node* node = start;
	
	while(node)
	{
		node->playNote(note);
		result = true;
		node = node->getNextNode();
	}
	
	return result;
}

void Direct_Collection::stopNote(uint8_t note)
{
	MIDI_Pitch_Node* node = start;
	while(node)
	{
		node->tryStopNote(note);
		node = node->getNextNode();
	}
}