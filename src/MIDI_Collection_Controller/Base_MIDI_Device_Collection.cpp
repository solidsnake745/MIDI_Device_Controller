#include "Base_MIDI_Device_Collection.h"
#include "MIDI_Device_Node.h"

SerialDebug Base_MIDI_Device_Collection::_debug(DEBUG_DEVICECHAIN_BASE);

Base_MIDI_Device_Collection::~Base_MIDI_Device_Collection()
{
};

void Base_MIDI_Device_Collection::printStatus()
{
	_debug.println(F("%d nodes in this chain"), count);	
	if(!start) return;
	
	_debug.debugln(5, F("Start of the chain"));
	_debug.debugln(5);
	
	int i = 0;
	MIDI_Device_Node *node = start;
	while(node)
	{
		_debug.println(F("Device Node %d: ID %d"), i++, node->device->getID());
		//_debug.println(F("  Device ID: %d"), node->device->getID());
		
		if(node->prev)		
			_debug.debugln(5, F("  Previous Device ID: %d"), node->prev->device->getID());
		else		
			_debug.debugln(5, F("  Previous Device ID: NULL"));
		
		if(node->next)
			_debug.debugln(5, F("  Next Device ID: %d"), node->next->device->getID());
		else
			_debug.debugln(5, F("  Next Device ID: NULL"));

		_debug.debugln(5);
		delay(10);
		node = node->next;
	}
	
	_debug.debugln(5, F("End of the chain"));
};

void Base_MIDI_Device_Collection::addDevice(MIDI_Device *d)
{
	//Handle first node insertion
	if(!start)
	{
		start = new MIDI_Device_Node(d, this);
		count++;
		_debug.debugln(5, F("Starting node added"));
		return;
	}
	
	//Prevent adding duplicate devices
	MIDI_Device_Node *node = start;
	while(node)
	{		
		if(node->device->getID() == d->getID())
		{
			_debug.debugln(5, F("Device ID %d already added"), node->device->getID());
			return;
		}
		
		if(!node->next) break;
		node = node->next;
	}
	
	//Create, setup, and add new node	
	MIDI_Device_Node *newNode = new MIDI_Device_Node(d, this);
	node->next = newNode;
	newNode->prev = node;
	end = newNode;
	count++;
	_debug.debugln(5, F("New node added"));
};

void Base_MIDI_Device_Collection::addDevices(MIDI_Device devices[], uint16_t numDevices)
{	
	if(numDevices > MAX_DEVICES)
		numDevices = MAX_DEVICES;

	_debug.debugln(5, F("Attempting to add %d device(s)"), numDevices);
	
	int i = 0;
	while(i < numDevices)
		addDevice(&devices[i++]);
}

void Base_MIDI_Device_Collection::removeDevice(uint8_t id)
{
	//Handle empty chain scenario
	if(!start)
	{
		_debug.println(F("No nodes in this chain"));
		return;
	}
	
	//Find and delete node with given device ID
	MIDI_Device_Node *node = start;
	while(node)
	{
		_debug.debugln(5, F(" Searching - current ID: %d"), node->device->getID());
		if(node->device->getID() != id)
		{
			node = node->next;
			continue;
		}

		_debug.debugln(4, F("Node found"));
		deleteNode(node);
		count--;
		
		if(start == end)
		{
			_debug.debugln(5, F("Only 1 node remaining after deleting"));
			end = NULL;
		}
		
		return;
	}
	
	_debug.println(F("No node with device ID %d found"), id);
};

bool Base_MIDI_Device_Collection::assignNote(uint8_t note)
{
	_debug.debugln(5, F("Note %d assignment received"), note);
	return true;
};

void Base_MIDI_Device_Collection::clearNote(uint8_t note)
{
	_debug.debugln(5, F("Note %d clear received"), note);
};

void Base_MIDI_Device_Collection::pitchBend(uint16_t bend)
{
	MIDI_Device_Node *node = start;
	while(node)
	{
		node->device->pitchBend(bend);
		node = node->next;
	}
};

void Base_MIDI_Device_Collection::deleteNode(MIDI_Device_Node *node)
{	
	//Handle deleting the start node
	if(node == start)
	{
		start = node->next;		
		_debug.debugln(5, F("Start node reassigned"));
		
		if(start)
		{
			start->prev = NULL;
			_debug.debugln(5, F("New start device ID: "), start->device->getID());
		}
		else
			_debug.debugln(5, F("Reassigned to nothing"));
		
		delete node;
		_debug.debugln(5, "Deleted");
		return;
	}
	
	//Handle deleting the end node
	if(node == end)
	{
		end = node->prev;

		_debug.debugln(5, F("End node reassigned"));
		if(end)
		{
			end->next = NULL;
			_debug.debugln(5, F("New end device ID: %d"), end->device->getID());
		}
		else
			_debug.debugln(5, F("Reassigned to nothing"));
		
		delete node;
		_debug.debugln(5, "Deleted");
		return;
	}
	
	//Handle deleting a normal node
	//Handle the previous node's next reference - set to current node's next
	node->prev->next = node->next;
	_debug.debugln(5, F("Previous node reassigned"));
	
	//Handle the next node's previous reference - set to current node's prev
	node->next->prev = node->prev;
	_debug.debugln(5, F("Next node reassigned"));	

	delete node;
	// PRINT(F("Deleted"))
};