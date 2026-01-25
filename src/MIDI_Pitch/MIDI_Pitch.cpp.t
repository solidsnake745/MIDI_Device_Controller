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
void MIDI_Pitch::setStepPin(IOType type, int8_t pin) 
{ 
	_stepPinMap = pin;
	if (_stepPinMap < 0)
	{		
		_stepIO = nullptr;
		return;
	}
	
	_stepIO = IOF.getIO(type);
	if(_stepIO && _stepIO->isValidMapping(_stepPinMap))
	{
		//This is being handled during MDC.startPlaying() now
		//_stepIO->setShouldStop(_dirPinMap, true);
	}			
	else
		_stepPinMap = -1;
}

void MIDI_Pitch::setDirPin(IOType type, int8_t pin) 
{ 
	_dirPinMap = pin;
	if (_dirPinMap < 0)
	{		
		_dirIO = nullptr;
		return;
	}
	
	_dirIO = IOF.getIO(type);
	if(_dirIO && _dirIO->isValidMapping(_dirPinMap))
	{
		//This is being handled during MDC.startPlaying() now
		//_dirIO->setShouldStop(_dirPinMap, false);
	}
	else
		_dirPinMap = -1;
}

void MIDI_Pitch::printStatus()
{
	_debug.println(F("Status for device %d"), _id);
	_debug.println(F("  Is Enabled: %d"), isEnabled());
	_debug.println(F("  Step Out: %d"), getStepPin());
	_debug.println(F("  Step State: %d"), getStepState());	
	_debug.println(F("  Dir Out: %d"), getDirPin());
	_debug.println(F("  Dir State: %d"), getDirState());
	_debug.println(F("  Current Position: %d"), getPosition());
	_debug.println(F("  Max Position: %d"), getMaxPosition());
	_debug.println(F("  Current Note: %d"), getCurrentNote());
}

//Operation
//_____________________________________________________________________________________________
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

void MIDI_Pitch::playNote(uint8_t note, void* assignedBy)
{
	if(!isEnabled()) 
	{
		_debug.debugln(7, F("%d - Not enabled"), _id);
		return;
	}
	
	_currentNote = note;
	_currentPeriod = getBasePeriod();
	_lastAssignedBy = assignedBy;
	
	_debug.debugln(7, F("%d - Note %d (%d) assigned"), _id, _currentNote, _currentPeriod);
	
	if(_parent) _parent->noteAssigned();	
}

void MIDI_Pitch::playPeriod(uint16_t period, void* assignedBy)
{
	if(!isEnabled()) 
	{
		_debug.debugln(7, F("%d - Not enabled"), _id);
		return;
	}
	
	_currentNote = 255;	
	_currentPeriod = period;
	_lastAssignedBy = assignedBy;
	
	_debug.debugln(7, F("%d - Period %d assigned"), _id, _currentPeriod);

	if(_parent) _parent->noteAssigned();
}

void MIDI_Pitch::bendNote(int16_t bend, bool shiftRange)
{
	if (!(_currentNote > 0 && _currentNote < 256))
		return;
	
	//If an effect other than pitchbend is in place, don't continue
	if(_currentEffect != None && _currentEffect != PitchBend)
		return;
	
	//Set the current effect so it's not interfered with
	//Or clear it if the bend is 0 (bend back to original note)
	_currentEffect = bend == 0 ? None : PitchBend;
	
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
	
	//If an effect other than pitchbend is in place, don't continue
	if(_currentEffect != None && _currentEffect != PitchBend)
		return;
	
	//Set the current effect so it's not interfered with
	//Or clear it if the factor is 1 (bend back to original note)
	_currentEffect = pitchFactor == 1.0 ? None : PitchBend;
	
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
	if(_dirIO && _dirIO->getOutput(_dirPinMap) != direction)
	{
		//Set the desired state for the given device
		setDirState(direction);
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
	_currentEffect = None;
	_vibratoTick = 0;
	_vibratoDegree = 0;

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
	if(_stepPinMap < 0 || !_stepIO) return;
	_stepIO->setOutput(_stepPinMap, state);
}

void MIDI_Pitch::setDirState(bool state)
{
	if(_dirPinMap < 0 || !_dirIO) return;
	_dirIO->setOutput(_dirPinMap, state);
}

void MIDI_Pitch::toggleStep()
{
	//Toggle step pin state
	if(_stepIO)
		_stepIO->toggleOutput(_stepPinMap);
	
	//Increment position
	if(isTrackingPosition())
		_currentPosition++;
}

void MIDI_Pitch::toggleDirection()
{
	//Toggle state for given device's direction pin
	_debug.debugln(7, F("%d - Direction toggle"), _id);	
	if(_dirIO)
		_dirIO->toggleOutput(_dirPinMap);
}

#pragma GCC push_options
#pragma GCC optimize("Ofast")
void MIDI_Pitch::processNotes()
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
	
	_currentDuration.addMicros(MIDI_Periods::getResolution());
	
	if(_currentEffect == Vibrato)
		_vibratoTick += MIDI_Periods::getResolution();
}
#pragma GCC pop_options

bool MIDI_Pitch::getStepState()
{
	if(_stepPinMap < 0 || !_stepIO)
		return LOW; //Have to return something
	
	return _stepIO->getOutput(_stepPinMap);
}

bool MIDI_Pitch::getDirState()
{
	if(_dirPinMap < 0 || !_dirIO) 
		return LOW; //Have to return something
	
	return _dirIO->getOutput(_dirPinMap);
}

//Testing/debug
//_____________________________________________________________________________________________

void MIDI_Pitch::testDoReMi(uint8_t octave, uint16_t noteDuration, uint16_t noteGap)
{
	uint8_t noteShift = octave * 12;	
	
	_debug.debugln(7, F("%d - Starting DoReMi test"), _id);
	playNote(48 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(50 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(52 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(53 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	_debug.debugln(7, F("%d - Half point DoReMi test"), _id);
	playNote(55 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(57 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(59 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	
	playNote(60 + noteShift); delay(noteDuration);
	stopNote(); delay(noteGap);
	_debug.debugln(7, F("%d - Finished DoReMi test"), _id);
}

void MIDI_Pitch::testStepping(uint32_t steps)
{
	if(!_stepIO)
	{
		_debug.debugln(7, F("%d - Step output not setup"), _id);
		return;
	}

	uint32_t x = 0;
	while(x++ < steps)
	{
		if(_parent) _parent->noteAssigned();
		toggleStep();
		delayMicroseconds(5000);
		toggleStep();
		
		if(_currentPosition >= _maxPosition)
		{
			toggleDirection();
			zeroPosition();
		}
	}
}

void MIDI_Pitch::testStepping(int32_t steps, bool direction)
{
	setDirection(direction);
	testStepping(steps);
}

void MIDI_Pitch::testDirect()
{
	if(!_stepIO)
	{
		_debug.debugln(7, F("%d - Step output not setup"), _id);
		return;
	}
		
	for(int16_t i = 0; i <= 50; i++)
	{
		toggleStep();
		_stepIO->updateOutputs();
		delayMicroseconds(5000);
		toggleStep();
		_stepIO->updateOutputs();
		
		if(_currentPosition >= _maxPosition)
		{
			toggleDirection();
			zeroPosition();
		}
	}
}

void MIDI_Pitch::testMaxDirection() 
{
	//Nothing to test if no valid step output set
	if(!_stepIO)
	{
		_debug.debugln(7, F("%d - Step output not setup"), _id);
		return;
	}
	
	//Nothing to test if no valid max position set
	if(!isTrackingPosition()) 
	{
		_debug.debugln(7, F("%d - Not tracking position"), _id);
		return;
	}
	
	_debug.debugln(7, F("%d - Testing max direction part 1"), _id);
	while(!isAtMaxPosition()) 
	{ 
		_debug.debugln(7, F("%d - Testing max direction part 1a"), _id);
		if(_parent) _parent->noteAssigned();
		_debug.debugln(7, F("%d - Testing max direction part 1b"), _id);
		toggleStep();
		delayMicroseconds(5000);
	}

	_debug.debugln(7, F("%d - Testing max direction part 2"), _id);
	toggleDirection();
	zeroPosition();

	delay(1000);
	
	_debug.debugln(7, F("%d - Testing max direction part 3"), _id);
	while(!isAtMaxPosition()) 
	{
		if(_parent) _parent->noteAssigned();
		toggleStep(); 
		delayMicroseconds(5000);
	}

	toggleDirection();
	zeroPosition();
	if(_parent) _parent->stopPlaying();
}