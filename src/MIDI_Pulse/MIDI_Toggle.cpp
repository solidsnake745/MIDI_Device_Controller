#include "MIDI_Toggle.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

//Constructors
//_____________________________________________________________________________________________
MIDI_Toggle::MIDI_Toggle()
{
	_debug.debugln(8, F("Device created"));
}

MIDI_Toggle::~MIDI_Toggle()
{
	_debug.debugln(8, F("Device deleted"));	
}

//Configuration
//_____________________________________________________________________________________________

//Operation
//_____________________________________________________________________________________________
void MIDI_Toggle::processNotes()
{
	//Nothing to do here
}

void MIDI_Toggle::pulse()
{
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Pulse output not setup"), _id);
		return;
	}
	
	_outIO->toggleOutput(_outNum);
	if(_parent)
		_parent->noteAssigned();
}

void MIDI_Toggle::stopPulse()
{
	//Nothing to do here
}

//Testing/debug
//_____________________________________________________________________________________________
void MIDI_Toggle::testOutputDirect(uint32_t duration)
{
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Pulse output not setup"), _id);
		return;
	}
	
	_outIO->setOutput(_outNum, HIGH);
	_outIO->updateOutputs();
	delayMicroseconds(duration);
	_outIO->setOutput(_outNum, LOW);
	_outIO->updateOutputs();
}