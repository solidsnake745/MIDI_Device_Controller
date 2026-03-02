#include "MIDI_Pulse_Mapper.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

std::map<uint8_t, Base_MIDI_Pulse*> MIDI_Pulse_Mapper::_noteMap;

MIDI_Pulse_Mapper::MIDI_Pulse_Mapper() {}

void MIDI_Pulse_Mapper::addMapping(uint8_t note, Base_MIDI_Pulse* d)
{
	//Check note is not already mapped
	if(_noteMap.count(note) > 0)
	{
		_debug.println(F("Note %d is already added"), note);
		return;
	}
	
	//Save the mapping
	_noteMap[note] = d;
}

void MIDI_Pulse_Mapper::deleteMapping(uint8_t note)
{	
	if(getMappedDevice(note))
		_noteMap.erase(_noteMap.find(note));
	
	_debug.println(F("Note %d was unmapped"), note);
}

Base_MIDI_Pulse* const MIDI_Pulse_Mapper::getMappedDevice(uint8_t note)
{
	if(_noteMap.empty())
	{
		_debug.println(F("No notes are mapped"));
		return nullptr;
	}
	
	auto find = _noteMap.find(note);
	if(find == _noteMap.end())
	{
		_debug.println(F("Note %d is not mapped"), note);
		return nullptr;
	}
	
	return find->second;
}

void MIDI_Pulse_Mapper::pulseNote(uint8_t note)
{
	Base_MIDI_Pulse* d = getMappedDevice(note);
	if(d) d->pulse();
}

void MIDI_Pulse_Mapper::stopNote(uint8_t note)
{
	Base_MIDI_Pulse* d = getMappedDevice(note);
	if(d) d->stopPulse();
}

void MIDI_Pulse_Mapper::stopNotes()
{
	if(_noteMap.empty())
	{
		_debug.println(F("No notes are mapped; not stopping any pulse devices"));
		return;
	}
	
	//This needs to be updated to find unique devices and then stop them so they're only stopped once
	//TODO: Use MDC to stop all Base_MIDI_Pulse devices
	auto i = _noteMap.begin();
	while(i != _noteMap.end())
	{
		_debug.debugln(DEBUG, F("Stopping device mapped to note %d"), i->first);
		i++->second->stopPulse();
	}
	
	_debug.println(F("All pulse devices stopped"));
}