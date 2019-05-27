#include "DeviceChain_Direct.h"
#include "DeviceNode.h"

void DeviceChain_Direct::assignNote(uint8_t note)
{
	DeviceNode *node = start;
	
	while(node)
	{
		if(node->device->isAvailable())
			node->assignNote(note);
		
		node = node->next;
	}
}

void DeviceChain_Direct::clearNote(uint8_t note)
{
	DeviceNode *node = start;
	
	while(node)
	{
		if(node->device->getCurrentNote() == note)
			node->clearNote();
		
		node = node->next;
	}
}

void DeviceChain_FA::assignNote(uint8_t note)
{
	DeviceNode *node = start;
	
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

void DeviceChain_FA::clearNote(uint8_t note)
{	
	DeviceNode *node = start;
	
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

void DeviceChain_RR::assignNote(uint8_t note)
{
	if(count == 1)
	{
		start->tryAssign(note);
		return;
	}
	
	DeviceNode *nextAssign;	
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

void DeviceChain_RR::clearNote(uint8_t note)
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
	
	DeviceNode *nextClear = lastAssign->prev;
	if(!nextClear) //At the start of the list, go back to the end
		nextClear = end;
	
	DeviceNode *stop = nextClear;
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