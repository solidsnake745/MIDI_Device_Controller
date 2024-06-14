#include "MIDI_Pulse_Controller.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

std::map<uint8_t, MIDI_Pulse_Controller::mapEntry*> MIDI_Pulse_Controller::_noteMap;

//Global singleton instance
MIDI_Pulse_Controller MPC = MIDI_Pulse_Controller::getInstance();

MIDI_Pulse_Controller *MIDI_Pulse_Controller::_instance = NULL;

MIDI_Pulse_Controller::MIDI_Pulse_Controller()
{
}

MIDI_Pulse_Controller &MIDI_Pulse_Controller::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Pulse_Controller();
	return *_instance;
}

void MIDI_Pulse_Controller::addMapping(uint8_t note, IO_DeviceEnum device, uint8_t output)
{
	//Check note is not already mapped
	if(_noteMap.count(note) > 0)
	{
		_debug.debugln(15, F("Note %d is already added"), note);
		return;
	}
	
	//Retrieve device to map to
	IO_Device* d = IOF.getDevice(device);
	
	//Check the device is populated
	if(!d)
	{
		_debug.debugln(15, F("Device not populated"));
		return;
	}		

	if(d->isValidMapping(output))
	{
		d->setMaxDuration(output, _defaultDuration);
		_noteMap[note] = new mapEntry(d, output);
	}		
	else
		_debug.debugln(15, F("Mapping to output %d is not valid"), output);
}

void MIDI_Pulse_Controller::deleteMapping(uint8_t note)
{	
	auto find = _noteMap.find(note);
	if(find == _noteMap.end())
		_debug.debugln(15, F("Note %d is not mapped"), note);
	else
	{
		delete(find->second);
		_noteMap.erase(find);
	}
}

void MIDI_Pulse_Controller::pulseNote(uint8_t note)
{
	auto find = _noteMap.find(note);
	if(find == _noteMap.end())
		_debug.debugln(15, F("Note %d is not mapped"), note);
	else
	{
		mapEntry* e = find->second;
		e->device->setOutput(e->out, true);
		MDC.noteAssigned();
	}
}

void MIDI_Pulse_Controller::stopNote(uint8_t note)
{
	auto find = _noteMap.find(note);
	if(find == _noteMap.end())
		_debug.debugln(15, F("Note %d is not mapped"), note);
	else
	{
		mapEntry* e = find->second;
		e->device->setOutput(e->out, false);
	}
}

void MIDI_Pulse_Controller::stopNotes()
{
	IO_Device* d;
	d = IOF.getDevice(DigitalWrite);;
	if(d)
		d->stopOuts();
	
	d = IOF.getDevice(SN74HC595N);
	if(d)
		d->stopOuts();
}