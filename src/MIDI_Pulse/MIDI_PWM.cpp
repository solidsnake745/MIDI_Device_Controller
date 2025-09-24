#include "MIDI_PWM.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

// Constructors
// _____________________________________________________________________________________________
// MIDI_Pulse::MIDI_Pulse()
// {
	// _debug.debugln(8, F("Device created"));
// }

// MIDI_Pulse::~MIDI_Pulse()
// {
	// _debug.debugln(8, F("Device deleted"));	
// }

// Configuration
// _____________________________________________________________________________________________
// void MIDI_Pulse::setOutput(IOType type, int8_t outNum) 
// { 
	// _outNum = outNum;
	// if (_outNum < 0)
	// {
		// _outIO = nullptr;
		// return;
	// }
	
	// _outIO = IOF.getIO(type);
	// if(_outIO && _outIO->isValidMapping(_outNum))
	// {
		// Below taken from MIDI_Pitch logic, but starting to lean towards refactoring out duration tracking in favor of relying on idle timeout to stop playback
		// _outIO->setMaxDuration(_outNum, 0);
	// }
	// else
		// _outNum = -1;
// }

// Operation
// _____________________________________________________________________________________________
// void MIDI_Pulse::processNotes()
// {
	// if(!_outIO)
	// {
		// _debug.debugln(20, F("%d - Pulse output not setup"), _id);
		// return;
	// }
	
	// if(_outIO->getOutput(_outNum))
		// _currentDuration.addMicros(MIDI_Periods::getResolution());
// }

// void MIDI_Pulse::checkMaxDuration()
// {
	// if(_maxDuration.isZero())
		// return;
	
	// if(_maxDuration < _currentDuration)
		// stopPulse();
// }

// bool MIDI_Pulse::getOutputState()
// {
	// if(!_outIO)
	// {
		// _debug.debugln(7, F("%d - Pulse output not setup"), _id);
		// return false;
	// }
	
	// return _outIO->getOutput(_outNum);
// }

// void MIDI_Pulse::pulse()
// {
	// if(!_outIO)
	// {
		// _debug.debugln(7, F("%d - Pulse output not setup"), _id);
		// return;
	// }
	
	// _outIO->setOutput(_outNum, HIGH);
	// if(_parent)
		// _parent->noteAssigned();
// }

// void MIDI_Pulse::stopPulse()
// {
	// if(!_outIO)
	// {
		// _debug.debugln(7, F("%d - Pulse output not setup"), _id);
		// return;
	// }
	
	// _outIO->setOutput(_outNum, LOW);
	// _currentDuration.reset();
// }

// Testing/debug
// _____________________________________________________________________________________________
// void MIDI_Pulse::testOutputDirect(uint32_t duration)
// {
	// if(!_outIO)
	// {
		// _debug.debugln(7, F("%d - Pulse output not setup"), _id);
		// return;
	// }
	
	// _outIO->setOutput(_outNum, HIGH);
	// _outIO->updateOutputs();
	// delayMicroseconds(duration);
	// _outIO->setOutput(_outNum, LOW);
	// _outIO->updateOutputs();
// }

// void MIDI_Pulse::testPwmPulseDirect(uint32_t length, uint32_t period, uint8_t numPulses)
// {
	// if(!_outIO)
	// {
		// _debug.debugln(7, F("%d - Pulse output not setup"), _id);
		// return;
	// }
	
	// if(numPulses == 0)
		// numPulses = 1;
	
	// for(uint8_t x = 0; x < numPulses; x++)
	// {
		// _outIO->setOutput(_outNum, HIGH);
		// _outIO->updateOutputs();
		// delayMicroseconds(length);
		// _outIO->setOutput(_outNum, LOW);
		// _outIO->updateOutputs();
		// delayMicroseconds(period - length);
	// }
// }

// void MIDI_Pulse::testPwmPulse(uint32_t length, uint32_t period)
// {
	
// }