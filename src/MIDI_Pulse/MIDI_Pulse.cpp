#include "MIDI_Pulse.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

//Constructors
//_____________________________________________________________________________________________
MIDI_Pulse::MIDI_Pulse()
{
	_debug.debugln(8, F("Device created"));
}

MIDI_Pulse::~MIDI_Pulse()
{
	_debug.debugln(8, F("Device deleted"));	
}

//Configuration
//_____________________________________________________________________________________________


//Operation
//_____________________________________________________________________________________________
void MIDI_Pulse::processNotes()
{
	if(!_outIO)
	{
		_debug.debugln(20, F("%d - Pulse output not setup"), _id);
		return;
	}
	
	if(_outIO->getOutput(_outNum))
		_currentDuration.addMicros(MIDI_Periods::getResolution());
	
	checkMaxDuration();
}

void MIDI_Pulse::checkMaxDuration()
{
	if(_maxDuration.isZero())
		return;
	
	if(_maxDuration < _currentDuration)
		stopPulse();
}

void MIDI_Pulse::pulse()
{
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Pulse output not setup"), _id);
		return;
	}
	
	_outIO->setOutput(_outNum, HIGH);
	if(_parent)
		_parent->noteAssigned();
}

void MIDI_Pulse::stopPulse()
{
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Pulse output not setup"), _id);
		return;
	}
	
	_outIO->setOutput(_outNum, LOW);
	_currentDuration.reset();
}

//Testing/debug
//_____________________________________________________________________________________________
void MIDI_Pulse::testOutputDirect(uint32_t duration)
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