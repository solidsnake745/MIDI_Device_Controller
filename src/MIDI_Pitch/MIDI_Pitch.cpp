#include "MIDI_Pitch.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

//Constructors
//_____________________________________________________________________________________________
MIDI_Pitch::MIDI_Pitch()
{
	resetProperties(true);
	_debug.debugln(8, F("Device created"));
}

MIDI_Pitch::~MIDI_Pitch()
{
	_debug.debugln(8, F("Device deleted"));	
}

//Configuration
//_____________________________________________________________________________________________
void MIDI_Pitch::setController(MIDI_Device_Controller *controller)
{
	_belongsTo = controller;
}

void MIDI_Pitch::setStepPin(IO_DeviceEnum device, int8_t pin) 
{ 
	_stepPinMap = pin; 
	if (_stepPinMap >= 0)
	{
		_stepIO = IOF.getDevice(device);
		if(_stepIO && _stepIO->isValidMapping(_stepPinMap))
			_stepIO->setMaxDuration(_stepPinMap, 0);
		else
			_stepPinMap = -1;		
	}
}

void MIDI_Pitch::setDirPin(IO_DeviceEnum device, int8_t pin) 
{ 
	_dirPinMap = pin;
	if (_dirPinMap < 0)
		return;
	
	_dirIO = IOF.getDevice(device);
	if(_dirIO && _dirIO->isValidMapping(_dirPinMap))
		_dirIO->setMaxDuration(_dirPinMap, 0);
	else
		_dirPinMap = -1;
}

void MIDI_Pitch::printStatus()
{
	_debug.println(F("Status for device %d"), _id);
	_debug.println(F("  Is Enabled: %d"), isEnabled());
	_debug.println(F("  Step Pin: %d"), getStepPin());
	_debug.println(F("  Step State: %d"), _stepState);
	_debug.println(F("  Actual State: %d"), digitalRead(_stepPinMap));
	_debug.println(F("  Dir Pin: %d"), getDirPin());
	_debug.println(F("  Dir State: %d"), _dirState);
	_debug.println(F("  Actual State: %d"), digitalRead(_dirPinMap));
	_debug.println(F("  Current Position: %d"), getPosition());
	_debug.println(F("  Max Position: %d"), getMaxPosition());
	_debug.println(F("  Current Note: %d"), getCurrentNote());
}

//Operation
//_____________________________________________________________________________________________
uint16_t *MIDI_Pitch::_referencePeriods = MIDI_Periods::calculatedPeriods;

void MIDI_Pitch::calibratePosition()
{
	setDirState(LOW);
	_currentPosition = getMaxPosition();
	resetPosition();
}

void MIDI_Pitch::stopNote()
{
	_debug.debugln(7, F("%d - Clearing note"), _id);
	_currentNote = 0;
}

void MIDI_Pitch::playNote(uint8_t note)
{
	if(!isEnabled()) 
	{
		_debug.debugln(7, F("%d - Not enabled"), _id);
		return;
	}
	
	_currentNote = note;
	_currentPeriod = getBasePeriod();
	
	_debug.debugln(7, F("%d - Note %d (%d) assigned"), _id, _currentNote, _currentPeriod);
	
	if(_belongsTo) _belongsTo->noteAssigned();
}

void MIDI_Pitch::playPeriod(uint16_t period)
{
	if(!isEnabled()) 
	{
		_debug.debugln(7, F("%d - Not enabled"), _id);
		return;
	}
	
	_currentNote = 255;	
	_currentPeriod = period;
	
	_debug.debugln(7, F("%d - Period %d assigned"), _id, _currentPeriod);

	if(_belongsTo) _belongsTo->noteAssigned();
}

void MIDI_Pitch::bendNote(uint16_t bend, bool shiftRange)
{ 
	if (!(_currentNote > 0 && _currentNote < 256))
		return;
	
	_debug.debugln(20, F("%d - Bending by value: %d"), _id, bend);
	
	float pitchFactor = PitchBend::calculateFactor(bend, shiftRange);
	_debug.debugln(20, F("%d - Factor: %f"), _id, pitchFactor);
	
	uint16_t basePeriod = getBasePeriod();
	_debug.debugln(20, F("%d - Base: %d"), _id, basePeriod);
	
	uint16_t newPeriod = basePeriod / pitchFactor;
	_debug.debugln(20, F("%d - New Period: %d"), _id, newPeriod);

	_currentPeriod = newPeriod;
}

void MIDI_Pitch::bendNoteByFactor(float pitchFactor)
{ 
	if (!(_currentNote > 0 && _currentNote < 256))
		return;
	
	_debug.debugln(20, F("%d - Bending by factor: %f"), _id, pitchFactor);
	
	uint16_t basePeriod = getBasePeriod();
	_debug.debugln(20, F("%d - Base: %d"), _id, basePeriod);
	
	uint16_t newPeriod = basePeriod / pitchFactor;
	_debug.debugln(20, F("%d - New Period: %d"), _id, newPeriod);

	_currentPeriod = newPeriod;
}

void MIDI_Pitch::setDirection(bool direction)
{
	//Check user is actually changing the direction from the current state
	if(_dirState != direction)
	{
		//Set the desired state for the given device
		_dirState = direction;
		setDirState(_dirState);
		_debug.debugln(7, F("%d - New direction: %d"), _id, direction);

		//Update it's current position if device is tracking it
		if(isTrackingPosition())
		{
			_debug.debugln(7, F("%d - Updating position from: %d"), _id, _currentPosition);
			_currentPosition = (_maxPosition - _currentPosition);
			_debug.debugln(7, F("%d - New position: %d"), _id, _currentPosition);
		}
	}
	else
	{
		_debug.debugln(7, F("%d - Direction is already: %d"), _id, direction);
	}
}

void MIDI_Pitch::resetProperties(bool includePosition)
{
	_debug.debugln(7, F("%d - Resetting properties"), _id);

	//Set given device's step pin low
	//Used for initially setting up a device during setup
	//Purely for visual effect when calling this during processing; in case there is an LED tied to it.
	setStepState(LOW);

	//Reset device properties
	_currentNote = -1;
	_currentPeriod = 0;
	_currentTick = 0;
	_currentDuration = 0;

	if(!includePosition) return;  
	setDirState(LOW);
	zeroPosition();
}

void MIDI_Pitch::resetPosition()
{
	//Don't attempt resetting if the device isn't set to track position
	if(!isTrackingPosition()) return;
	
	setDirection(HIGH);
	setStepState(LOW);
	
	while(!isAtMaxPosition())
	{
	  toggleStep();
	  delayMicroseconds(12345);	  
	}
	
	setDirection(LOW);
	setStepState(LOW);
}

void MIDI_Pitch::setStepState(bool state)
{
	_stepState = state;
	if(_stepPinMap < 0 || !_stepIO) return;
	_stepIO->setOutput(_stepPinMap, _stepState);
}

void MIDI_Pitch::setDirState(bool state)
{
	_dirState = state;
	if(_dirPinMap < 0 || !_dirIO) return;
	_dirIO->setOutput(_dirPinMap, _dirState);
}

void MIDI_Pitch::toggleStep()
{
	//Toggle state for given channel's step pin
	setStepState(!_stepState);
	
	//Increment position
	if(isTrackingPosition())
		_currentPosition++;
}

void MIDI_Pitch::toggleDirection()
{
	//Toggle state for given device's direction pin
	_debug.debugln(7, F("%d - Direction toggle"), _id);	
	setDirState(!_dirState);
}

#pragma GCC push_options
#pragma GCC optimize("Ofast")
void MIDI_Pitch::playNotes()
{
	if(_currentNote < 0)
	{
		_debug.debugln(20, F("%d - No note"), _id);
		return;
	}
	
	if(_currentNote == 0)
	{			
		_debug.debugln(20, F("%d - Resetting properties"), _id);
		resetProperties();
		return;
	}
		
	_currentDuration += MIDI_Periods::getResolution();
	_currentTick++;
	if(_currentTick >= _currentPeriod) 
	{
		_debug.debugln(20, F("%d - Toggling step"), _id);
		toggleStep();
		_currentTick = 0;
	}
	
	if(_maxPosition > 0 && _currentPosition >= _maxPosition) 
	{
		_debug.debugln(20, F("%d - Toggling direction"), _id);
		toggleDirection();
		zeroPosition();
	}
}
#pragma GCC pop_options

void MIDI_Pitch::checkMaxDuration()
{
	if(_belongsTo->_maxDuration != 0 && _currentDuration >= _belongsTo->_maxDuration) 
	{
		_debug.debugln(20, F("%d - Reached max duration"), _id);
		_currentNote = 0;
		return;
	}
}

//Testing/debug
//_____________________________________________________________________________________________

void MIDI_Pitch::testDoReMi(uint8_t octave, uint16_t noteDuration, uint16_t noteGap)
{
	uint8_t noteShift = octave * 12;	
	
	playNote(48 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(50 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(52 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(53 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(55 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(57 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(59 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(60 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
}

void MIDI_Pitch::testStepping(uint32_t steps)
{
	uint32_t x = 0;
	while(x++ < steps)
	{		
		toggleStep();
		if(_belongsTo) _belongsTo->noteAssigned();
		if(_currentPosition >= _maxPosition)
		{
			toggleDirection();
			zeroPosition();
		}
		delayMicroseconds(5000);
	}
}

void MIDI_Pitch::testStepping(int32_t steps, bool direction)
{
	setDirection(direction);
	testStepping(steps);
}

void MIDI_Pitch::testDirect()
{
	for(int16_t i = 0; i <= 50; i++) 
	{
		testStepping(2);
		delay(50);
	}
}

void MIDI_Pitch::testMaxDirection() 
{
	//Nothing to test if no valid max position set
	if(!isTrackingPosition()) 
	{
		_debug.debugln(7, F("%d - Not tracking position"), _id);
		return;
	}
		
	while(!isAtMaxPosition()) 
	{ 
		if(_belongsTo) _belongsTo->noteAssigned();
		toggleStep(); 
		delayMicroseconds(5000);
	}

	toggleDirection();
	zeroPosition();

	delay(1000);

	while(!isAtMaxPosition()) 
	{
		if(_belongsTo) _belongsTo->noteAssigned();
		toggleStep(); 
		delayMicroseconds(5000);
	}

	toggleDirection();
	zeroPosition();	
}