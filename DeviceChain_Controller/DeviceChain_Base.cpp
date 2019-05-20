#include "DeviceChain_Base.h"
#include "DeviceNode.h"

DeviceChain_Base::~DeviceChain_Base()
{
};

void DeviceChain_Base::printStatus()
{
	// PRINT2(count, F(" nodes in this chain"))	
	if(!start) return;
	
	// PRINT(F("Start of the chain"))
	// PRINT()
	
	//int i = 0;
	DeviceNode *node = start;
	while(node)
	{
		// PRINT2(F("Status for Device Node "), i++)
		// PRINT2(F("  Device ID: "), node->device->getID())
		
		if(node->prev)
		{
			// PRINT2(F("  Previous Device ID: "), node->prev->device->getID())
		}
		else
		{
			// PRINT(F("  Previous Device ID: NULL"))
		}
		
		if(node->next)
		{
			// PRINT2(F("  Next Device ID: "), node->next->device->getID())
		}
		else
		{
			// PRINT(F("  Next Device ID: NULL"))
		}

		// PRINT()
		delay(10);
		node = node->next;
	}
	
	// PRINT(F("End of the chain"))	
};

void DeviceChain_Base::addDevice(Device *d)
{
	//Handle first node insertion
	if(!start)
	{
		start = new DeviceNode(d, this);
		count++;
		// PRINT(F("Starting node added"))
		return;
	}
	
	//Prevent adding duplicate devices
	DeviceNode *node = start;
	while(node)
	{		
		if(node->device->getID() == d->getID())
		{
			// PRINT3(F("Device ID "), node->device->getID(), F(" already added"))
			return;
		}
		
		if(!node->next) break;
		node = node->next;
	}
	
	//Create, setup, and add new node
	DeviceNode *newNode = new DeviceNode(d, this);
	node->next = newNode;
	newNode->prev = node;
	end = newNode;
	count++;
	// PRINT(F("New node added"))
};

void DeviceChain_Base::addDevices(Device devices[], int16_t numDevices)
{
	if(numDevices == -1)
		numDevices = sizeof(devices) / sizeof(devices[0]);

	// _debug.debugln(5, F("Attempting to add %d device(s)"), numDevices);
	
	int i = 0;
	while(i < numDevices)
		addDevice(&devices[i++]);
}

void DeviceChain_Base::removeDevice(uint8_t id)
{
	//Handle empty chain scenario
	if(!start)
	{
		// PRINT(F("No nodes in this chain"))
		return;
	}
	
	//Find and delete node with given device ID
	DeviceNode *node = start;
	while(node)
	{
		// DEBUG2(F(" Searching - current ID: "), node->device->getID())
		if(node->device->getID() != id)
		{
			node = node->next;
			continue;
		}

		// PRINT(F("Node found"))
		deleteNode(node);
		count--;
		
		if(start == end)
		{
			// DEBUG(F("Only 1 node remaining, clearing end node"))
			end = NULL;
		}
		
		return;
	}
	
	// PRINT3(F("No node with device ID "), id, F(" found"))
};

void DeviceChain_Base::assignNote(uint8_t note)
{
	// PRINT3(F("Assign note "), note, F(" received, but no strategy defined"))
};

void DeviceChain_Base::clearNote(uint8_t note)
{
	// PRINT3(F("Clear note "), note, F(" received, but no strategy defined"))
};

void DeviceChain_Base::pitchBend(uint16_t bend)
{
	DeviceNode *node = start;
	while(node)
	{
		node->device->pitchBend(bend);
		node = node->next;
	}
};

void DeviceChain_Base::deleteNode(DeviceNode *node)
{	
	//Handle deleting the start node
	if(node == start)
	{
		start = node->next;
		
		// PRINT(F("Start node reassigned"))
		if(start)
		{
			start->prev = NULL;
			// DEBUG2(F("New start device ID: "), start->device->getID())			
		}
		else
		{
			// DEBUG(F("Reassigned to nothing"))
		}
		
		delete node;
		// PRINT(F("Deleted"))
		return;
	}
	
	//Handle deleting the end node
	if(node == end)
	{
		end = node->prev;
		
		// PRINT(F("End node reassigned"))
		if(end)
		{
			end->next = NULL;
			// DEBUG2(F("New end device ID: "), end->device->getID())
		}
		else
		{
			// DEBUG(F("Reassigned to nothing"))
		}
		
		delete node;
		// PRINT(F("Deleted"))
		return;
	}
	
	//Handle deleting a normal node
	//Handle the previous node's next reference - set to current node's next
	node->prev->next = node->next;
	// DEBUG(F("Previous node reassigned"))
	
	//Handle the next node's previous reference - set to current node's prev
	node->next->prev = node->prev;
	// DEBUG(F("Next node reassigned"))

	delete node;
	// PRINT(F("Deleted"))
};