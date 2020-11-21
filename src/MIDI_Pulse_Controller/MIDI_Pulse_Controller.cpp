#include "MIDI_Pulse_Controller.h"

SerialDebug MIDI_Pulse_Controller::_debug(DEBUG_PulseController);

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

void MIDI_Pulse_Controller::addMapping(uint8_t note, PulseDevice device, uint8_t output)
{
	//Check note is not already mapped
	if(_noteMap.count(note) > 0)
	{
		_debug.debugln(15, F("Note %d is already added"), note);
		return;
	}
	
	//Retrieve device to map to
	IPulseNotes* d = NULL;
	switch(device)
	{
		case PulseDevice::DigitalIO:
			d = MDC.getDigitalIO();			
			break;
			
		case PulseDevice::SN74HC595N:
			d = MDC.getSN74HC595N();
			break;
	}
	
	//Check the device is populated
	if(!d)
	{
		_debug.debugln(15, F("Device not populated"));
		return;
	}		

	if(d->isValidMapping(output))
		_noteMap[note] = new mapEntry(d, output);
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
		e->device->pulseOutput(e->out);
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
		e->device->stopOutput(e->out);
	}
}

void MIDI_Pulse_Controller::stopNotes()
{
	IPulseNotes* d;
	d = MDC.getDigitalIO();
	if(d)
		d->stopOutputs();
	
	d = MDC.getSN74HC595N();
	if(d)
		d->stopOutputs();
}