#include "Base_MIDI_SoftPWM.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

//Constructors
//_____________________________________________________________________________________________

//Configuration
//_____________________________________________________________________________________________

//Operation
//_____________________________________________________________________________________________
void Base_MIDI_SoftPWM::pulse()
{
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Pulse output not setup"), _id);
		return;
	}
	
	if(_currentPWMState != Running)
		_currentPWMState = Starting;
	
	setLengthOnPulse();
	_currentDuration.reset();
	
	if(_parent) _parent->noteAssigned();
}

//Testing/debug
//_____________________________________________________________________________________________
void Base_MIDI_SoftPWM::testInterrupt(uint32_t length, uint32_t period, uint16_t duration)
{
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Pulse output not setup"), _id);
		return;
	}
	
	_currentLength = length;
	_currentTick = 0;
	_currentPeriod = period;
	_currentPWMState = Starting;
	
	if(_parent) _parent->noteAssigned();
	delay(duration);
	_currentLength = 0;
	_currentTick = 0;
	_currentPeriod = 0;
}