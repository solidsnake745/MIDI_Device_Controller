#include "RoundRobin_Collection.h"

void RoundRobin_Collection::reset()
{
	lastAssign = nullptr;
}

bool RoundRobin_Collection::playNote(uint8_t note)
{
	if(_count == 1)
		return start->tryPlayNote(note);
	
	MIDI_Pitch_Node* nextAssign;	
	if(!lastAssign)
		nextAssign = start;
	else
		nextAssign = lastAssign->getNextNode();
	
	if(!nextAssign) //At the end of the list, go back to the start
		nextAssign = start;
	
	while(true)
	{
		if(nextAssign->tryPlayNote(note)) break;
		nextAssign = nextAssign->getNextNode();
	
		if(!nextAssign) //At the end of the list, go back to the start
			nextAssign = start;
		
		if(nextAssign == lastAssign)
		{
			//On the last node, try assigning to the original last assigned node and exit out
			return nextAssign->tryPlayNote(note);
		}
	}

	// DEBUG2(F("Note assigned to device ID "), nextAssign->device->getID())
	lastAssign = nextAssign;
	return true;
}

void RoundRobin_Collection::stopNote(uint8_t note)
{
	//_debug.debugln(5, F("Looking to clear note "), note);
	
	if(!lastAssign)
	{
		// DEBUG(F("No last assigned node"))
		return;
	}

	if(_count == 1)
	{
		lastAssign->tryStopNote(note);
		return;
	}
	
	MIDI_Pitch_Node* nextClear = lastAssign->getPrevNode();
	if(!nextClear) //At the start of the list, go back to the end
		nextClear = end;
	
	MIDI_Pitch_Node* stop = nextClear;
	// DEBUG2(F("Starting at device ID "), nextClear->device->getID())
	
	while(true)
	{		
		if(nextClear->tryStopNote(note)) break;
		nextClear = nextClear->getPrevNode();
	
		if(!nextClear) //At the start of the list, go back to the end
			nextClear = end;
		
		if(nextClear == stop)
		{
			// DEBUG(F("No matching node found"))
			return;
		}
	}
	
	// DEBUG2(F("Note cleared from device ID "), nextClear->device->getID())
}