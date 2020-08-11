#include "MIDI_Pitch.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

SerialDebug MIDI_Pitch::_debug(DEBUG_DEVICE);

//Constructors
//_____________________________________________________________________________________________
MIDI_Pitch::MIDI_Pitch(int8_t stepPin, int8_t dirPin, int32_t maxPosition)
{	
	setup(stepPin, dirPin, maxPosition);	
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

void MIDI_Pitch::setup(int8_t stepPin, int8_t dirPin, int32_t maxPosition)
{
	_debug.debugln(7, F("%d - Setting up device"), _id);
	
	//Set properties from inputs
	setStepPin(stepPin);
	setDirPin(dirPin);
	setMaxPosition(maxPosition);
	
	initialize();
}

void MIDI_Pitch::initialize()
{
	//Initialize/reset device properties to an initial state
	resetProperties(true);

	//Setup mapped step pin
	if (_stepPinMap >= 0)
	{
		pinMode(_stepPinMap, OUTPUT);
		setStepState(LOW);
		_enabled = true;
	}
	else
	{
		//If no valid pin setup for step, disable the device
		_enabled = false;
	}

	//Setup mapped direction pin
	if (_dirPinMap >= 0)
	{
		pinMode(_dirPinMap, OUTPUT);
		setDirState(LOW);
	}
	else
	{
		//If no valid pin setup for direction, disable position tracking
		_maxPosition = -1;
	}
}

void MIDI_Pitch::setStepPin(int8_t pin) { _stepPinMap = pin; }

void MIDI_Pitch::setDirPin(int8_t pin) { _dirPinMap = pin; }

void MIDI_Pitch::printStatus()
{
	_debug.println(F("Status for device %d"), _id);
	_debug.println(F("  Enabled: %d"), getEnabled());
	_debug.println(F("  Is Enabled: %d"), isEnabled());
	_debug.println(F("  Step Pin: %d"), getStepPin());
	_debug.println(F("  Step State: %d"), getStepState());
	_debug.println(F("  Actual State: %d"), digitalRead(_stepPinMap));
	_debug.println(F("  Dir Pin: %d"), getDirPin());
	_debug.println(F("  Dir State: %d"), getDirState());
	_debug.println(F("  Actual State: %d"), digitalRead(_dirPinMap));
	_debug.println(F("  Current Position: %d"), getPosition());
	_debug.println(F("  Max Position: %d"), getMaxPosition());
	_debug.println(F("  Current Note: %d"), getCurrentNote());
}

uint8_t MIDI_Pitch::getID() { return _id; }
void MIDI_Pitch::setID(uint8_t value) { _id = value; }

int8_t MIDI_Pitch::getStepPin() { return _stepPinMap; }
int8_t MIDI_Pitch::getDirPin() { return _dirPinMap; }

void MIDI_Pitch::setMaxPosition(int32_t value) { _maxPosition = value; }
void MIDI_Pitch::setEnabled(bool value) { _enabled = value; }

bool MIDI_Pitch::getEnabled() { return _enabled; }

//Operation
//_____________________________________________________________________________________________
uint16_t *MIDI_Pitch::_referencePeriods = MIDI_Periods::calculatedPeriods;

void MIDI_Pitch::calibratePosition()
{
	setDirState(LOW);
	_currentPosition = getMaxPosition();
	resetPosition();
}

void MIDI_Pitch::zeroPosition() { _currentPosition = 0; }

void MIDI_Pitch::stopNote()
{
	_debug.debugln(7, F("%d - Clearing note"), _id);
	_currentNote = 0;
}

bool MIDI_Pitch::getStepState() { return _stepState; }
bool MIDI_Pitch::getDirState() { return _dirState; }
int16_t MIDI_Pitch::getPosition() { return _currentPosition; }
int16_t MIDI_Pitch::getMaxPosition() { return _maxPosition; }
int8_t MIDI_Pitch::getCurrentNote() { return _currentNote; }
int16_t MIDI_Pitch::getBasePeriod() { return *(_referencePeriods + _currentNote); }
int16_t MIDI_Pitch::getCurrentPeriod() { return _currentPeriod; }
uint32_t MIDI_Pitch::getDuration() { return _currentDuration; }
bool MIDI_Pitch::isAvailable() { return _currentNote == -1; }
bool MIDI_Pitch::isAtMaxPosition() { return _currentPosition >= _maxPosition; }
bool MIDI_Pitch::isTrackingPosition() { return _maxPosition > 0; }

bool MIDI_Pitch::isEnabled() { return (_enabled && _stepPinMap >= 0); }

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
	if(!isEnabled()) return;
	
	_currentNote = 255;	
	_currentPeriod = period;
	
	_debug.debugln(7, F("%d - Period %d assigned"), _id, _currentPeriod);

	if(_belongsTo) _belongsTo->noteAssigned();
}

void MIDI_Pitch::bendNote(uint16_t bend)
{ 
	if (_currentNote > 0 && _currentNote < 255) 
	{
		_debug.debugln(7, F("%d - Bending: %d"), _id, bend);
		
		int16_t basePeriod = getBasePeriod();
		_debug.debugln(7, F("%d - Base: %d"), _id, basePeriod);		
		
		float pitchFactor = pow(2.0, (bend - 8192.0) / 8192.0);
		_debug.debugln(7, F("%d - Factor: %d"), _id, pitchFactor);
		
		int16_t newPeriod = basePeriod / pitchFactor;
		_debug.debugln(7, F("%d - New Period: %d"), _id, newPeriod);

		_currentPeriod = newPeriod;
	}
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
	digitalWrite(_stepPinMap, _stepState);
}

void MIDI_Pitch::setDirState(bool state)
{
	_dirState = state;
	digitalWrite(_dirPinMap, _dirState);
}

void MIDI_Pitch::toggleStep()
{
	//Toggle state for given channel's step pin
	_stepState = !_stepState;
	digitalWrite(_stepPinMap, _stepState);
	
	//Increment position
	if(isTrackingPosition())
		_currentPosition++;
}

void MIDI_Pitch::toggleDirection()
{
	//Toggle state for given device's direction pin
	_debug.debugln(7, F("%d - Direction toggle"), _id);
	_dirState = !_dirState;
	setDirState(_dirState);
}

void MIDI_Pitch::playNotes()
{
	if(_currentNote < 0)
	{
		_debug.debugln(20, "%d - No note", _id);
		return;
	}
	
	if(_currentNote == 0)
	{			
		_debug.debugln(20, "%d - Resetting properties", _id);
		resetProperties();
		return;
	}
	
	uint32_t resolution = MIDI_Periods::getResolution();		
	_currentDuration += resolution;
	
	if(_belongsTo->_maxDuration != 0 && _currentDuration >= _belongsTo->_maxDuration) 
	{
		_debug.debugln(20, "%d - Reached max duration", _id);
		_currentNote = 0;
		return;
	}
	
	_currentTick++;
	if(_currentTick >= _currentPeriod) 
	{
		_debug.debugln(20, "%d - Toggling step", _id);
		toggleStep();
		_currentTick = 0;
	}
	
	if(_maxPosition > 0 && _currentPosition >= _maxPosition) 
	{
		_debug.debugln(20, "%d - Toggling direction", _id);
		toggleDirection();
		zeroPosition();
	}
}

//Testing/debug
//_____________________________________________________________________________________________
void MIDI_Pitch::testStepping(uint32_t steps)
{
	uint32_t x = 0;
	while(x++ < steps)
	{
		toggleStep();
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
		toggleStep(); 
		delayMicroseconds(5000);
	}

	toggleDirection();
	zeroPosition();

	delay(1000);

	while(!isAtMaxPosition()) 
	{
		toggleStep(); 
		delayMicroseconds(5000);
	}

	toggleDirection();
	zeroPosition();	
}

void MIDI_Pitch::runAllTests()
{
	testDirect(); delay(250);
	calibratePosition(); delay(250);
	testMaxDirection(); delay(250);
	calibratePosition();
}