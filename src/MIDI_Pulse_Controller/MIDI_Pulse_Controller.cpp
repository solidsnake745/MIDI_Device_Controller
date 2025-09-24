#include "MIDI_Pulse_Controller.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

std::map<uint8_t, Base_MIDI_Pulse*> MIDI_Pulse_Controller::_noteMap;

MIDI_Pulse_Controller::MIDI_Pulse_Controller() {}

void MIDI_Pulse_Controller::addMapping(uint8_t note, Base_MIDI_Pulse* d)
{
	//Check note is not already mapped
	if(_noteMap.count(note) > 0)
	{
		_debug.debugln(15, F("Note %d is already added"), note);
		return;
	}
	
	//Save the mapping
	_noteMap[note] = d;
}

void MIDI_Pulse_Controller::deleteMapping(uint8_t note)
{	
	auto find = _noteMap.find(note);
	if(find == _noteMap.end())
		_debug.debugln(15, F("Note %d is not mapped"), note);
	else
		_noteMap.erase(find);
}

Base_MIDI_Pulse* MIDI_Pulse_Controller::getMappedDevice(uint8_t note)
{
	auto find = _noteMap.find(note);
	if(find == _noteMap.end())
	{
		_debug.debugln(15, F("Note %d is not mapped"), note);
		return nullptr;
	}		
	else
		return find->second;
}

void MIDI_Pulse_Controller::pulseNote(uint8_t note)
{
	auto find = _noteMap.find(note);
	if(find == _noteMap.end())
		_debug.debugln(15, F("Note %d is not mapped"), note);
	else
		find->second->pulse();
}

void MIDI_Pulse_Controller::stopNote(uint8_t note)
{
	auto find = _noteMap.find(note);
	if(find == _noteMap.end())
		_debug.debugln(15, F("Note %d is not mapped"), note);
	else
		find->second->stopPulse();
}

void MIDI_Pulse_Controller::stopNotes()
{
	auto d = _noteMap.begin();
	while(d != _noteMap.end())
	{
		d->second->stopPulse();
		d++;
	}
}