#include "MIDI_Device_Chains.h"
#include "MIDI_Device_Node.h"

void Direct_Chain::assignNote(uint8_t note)
{
	MIDI_Device_Node *node = start;
	
	while(node)
	{
		if(node->device->isAvailable())
			node->assignNote(note);
		
		node = node->next;
	}
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

void FirstAvailable_Chain::assignNote(uint8_t note)
{
	MIDI_Device_Node *node = start;
	
	while(node)
	{
		if(node->device->isAvailable())
		{
			node->assignNote(note);
			return;
		}
		
		node = node->next;
	}
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

void RoundRobin_Chain::assignNote(uint8_t note)
{
	if(count == 1)
	{
		start->tryAssign(note);
		return;
	}
	
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
			return;
		}
	}

	// DEBUG2(F("Note assigned to device ID "), nextAssign->device->getID())
	lastAssign = nextAssign;
}

void RoundRobin_Chain::clearNote(uint8_t note)
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