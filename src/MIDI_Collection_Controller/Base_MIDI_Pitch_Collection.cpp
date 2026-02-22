#include "Base_MIDI_Pitch_Collection.h"
#include "../MIDI_Device_Controller.h" //Need the definition of methods

void Base_MIDI_Pitch_Collection::deleteNode(MIDI_Pitch_Node* node)
{
	//Handle deleting the start node
	if(node == start)
	{
		start = node->next;		
		_debug.debugln(DEBUG, F("Start node reassigned"));
		
		if(start)
		{
			start->prev = NULL;
			_debug.debugln(DEBUG, F("New start device ID: "), start->_device->getID());
		}
		else
			_debug.debugln(DEBUG, F("Reassigned to nothing"));
		
		delete node;
		_debug.debugln(DEBUG, F("Deleted"));
		return;
	}
	
	//Handle deleting the end node
	if(node == end)
	{
		end = node->prev;

		_debug.debugln(DEBUG, F("End node reassigned"));
		if(end)
		{
			end->next = NULL;
			_debug.debugln(DEBUG, F("New end device ID: %d"), end->_device->getID());
		}
		else
			_debug.debugln(DEBUG, F("Reassigned to nothing"));
		
		delete node;
		_debug.debugln(DEBUG, F("Deleted"));
		return;
	}
	
	//Handle deleting a normal node
	//Handle the previous node's next reference - set to current node's next
	node->prev->next = node->next;
	_debug.debugln(DEBUG, F("Previous node reassigned"));
	
	//Handle the next node's previous reference - set to current node's prev
	node->next->prev = node->prev;
	_debug.debugln(DEBUG, F("Next node reassigned"));	

	delete node;
}

void Base_MIDI_Pitch_Collection::addDevice(Base_MIDI_Pitch* d)
{
	//Handle first node insertion
	if(!start)
	{
		start = new MIDI_Pitch_Node(d, this);
		_count++;
		_debug.debugln(DEBUG, F("Starting node added"));
		return;
	}
	
	//Prevent adding duplicate devices
	MIDI_Pitch_Node* node = start;
	while(node)
	{		
		if(node->_device->getID() == d->getID())
		{
			_debug.debugln(DEBUG, F("Device ID %d already added"), node->_device->getID());
			return;
		}
		
		if(!node->next) break;
		node = node->next;
	}
	
	//Create, setup, and add new node	
	MIDI_Pitch_Node* newNode = new MIDI_Pitch_Node(d, this);
	node->next = newNode;
	newNode->prev = node;
	end = newNode;
	_count++;
	_debug.debugln(DEBUG, F("New node added"));
}

void Base_MIDI_Pitch_Collection::removeDevice(uint8_t id)
{
	//Handle empty chain scenario
	if(!start)
	{
		_debug.println(F("No nodes in this chain"));
		return;
	}
	
	//Find and delete node with given device ID
	MIDI_Pitch_Node* node = start;
	while(node)
	{
		_debug.debugln(DEBUG, F(" Searching - current ID: %d"), node->_device->getID());
		if(node->_device->getID() != id)
		{
			node = node->next;
			continue;
		}

		_debug.debugln(4, F("Node found"));
		deleteNode(node);
		_count--;
		
		if(start == end)
		{
			_debug.debugln(DEBUG, F("Only 1 node remaining after deleting"));
			end = NULL;
		}
		
		return;
	}
	
	_debug.println(F("No node with device ID %d found"), id);
}

void Base_MIDI_Pitch_Collection::reset() { _debug.debugln(DEBUG, F("Reset collection received")); }

void Base_MIDI_Pitch_Collection::printStatus()
{
	_debug.println(F("%d nodes in this chain"), _count);	
	if(!start) return;
	
	_debug.debugln(DEBUG, F("Start of the chain"));
	_debug.debugln(DEBUG);
	
	int i = 0;
	MIDI_Pitch_Node* node = start;
	while(node)
	{
		_debug.println(F("Device Node %d: ID %d"), i++, node->_device->getID());
		
		if(node->prev)		
			_debug.debugln(DEBUG, F("  Previous Device ID: %d"), node->prev->_device->getID());
		else		
			_debug.debugln(DEBUG, F("  Previous Device ID: NULL"));
		
		if(node->next)
			_debug.debugln(DEBUG, F("  Next Device ID: %d"), node->next->_device->getID());
		else
			_debug.debugln(DEBUG, F("  Next Device ID: NULL"));

		_debug.debugln(DEBUG);
		node = node->next;
	}
	
	_debug.debugln(DEBUG, F("End of the chain"));
}

bool Base_MIDI_Pitch_Collection::playNote(uint8_t note)
{
	_debug.debugln(DEBUG, F("Note %d assignment received"), note);
	return true;
}

void Base_MIDI_Pitch_Collection::bendNote(int16_t bend, bool shiftRange)
{
	//Calculate factor once and use for bending all devices
	float pitchFactor = PitchBend::calculateFactor(bend, shiftRange);
	
	MIDI_Pitch_Node* node = start;
	while(node)
	{
		node->bendNoteByFactor(pitchFactor);
		node = node->next;
	}
}

void Base_MIDI_Pitch_Collection::stopNote(uint8_t note) { _debug.debugln(DEBUG, F("Note %d clear received"), note); }

void Base_MIDI_Pitch_Collection::testPitchBend()
{
	bool currentSetting = MDC.isAutoPlayEnabled();
	MDC.setAutoPlay(true);
	
	for(uint8_t c = 0; c < _count; c++)
		playNote(MIDDLE_C_NOTE);

	bendNote(0);
	delayMicroseconds(500);
	
	for(int16_t i = -8192; i <= 8191; i++)
	{
		bendNote(i);
		delayMicroseconds(500);
	}
	
	for(uint8_t c = 0; c < _count; c++)
		stopNote(MIDDLE_C_NOTE);
	
	MDC.stopPlaying();
	MDC.setAutoPlay(currentSetting);
};