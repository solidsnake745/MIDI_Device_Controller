#include "RoundRobin_Collection.h"
#include "../MIDI_Device_Node.h"

bool RoundRobin_Collection::assignNote(uint8_t note)
{
	if(count == 1)
		return start->tryAssign(note);
	
	MIDI_Device_Node *nextAssign;	
	if(!lastAssign)
		nextAssign = start;
	else
		nextAssign = lastAssign->next;
	
	if(!nextAssign) //At the end of the list, go back to the start
		nextAssign = start;
	
	while(true)
	{
		if(nextAssign->tryAssign(note)) break;
		nextAssign = nextAssign->next;
	
		if(!nextAssign) //At the end of the list, go back to the start
			nextAssign = start;
		
		if(nextAssign == lastAssign)
		{
			// DEBUG(F("No available node found"))
			return false;
		}
	}

	// DEBUG2(F("Note assigned to device ID "), nextAssign->device->getID())
	lastAssign = nextAssign;
	return true;
}

void RoundRobin_Collection::clearNote(uint8_t note)
{
	//_debug.debugln(5, F("Looking to clear note "), note);
	
	if(!lastAssign)
	{
		// DEBUG(F("No last assigned node"))
		return;
	}

	if(count == 1)
	{
		lastAssign->tryClear(note);
		return;
	}
	
	MIDI_Device_Node *nextClear = lastAssign->prev;
	if(!nextClear) //At the start of the list, go back to the end
		nextClear = end;
	
	MIDI_Device_Node *stop = nextClear;
	// DEBUG2(F("Starting at device ID "), nextClear->device->getID())
	
	while(true)
	{		
		if(nextClear->tryClear(note)) break;
		nextClear = nextClear->prev;
	
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