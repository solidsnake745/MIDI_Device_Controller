#include "Device.h"
#include "MIDI_DeviceController.h" //Need the definition of noteAssigned()

#if Device_h_DEBUG > 0
	#include "SerialDebug.h"
	#define DEVICE_PRINT(x) SDBG.println(x);
	#define DEVICE_PRINT(x, args...) SDBG.println(x, args);
	#define DEVICE_DEBUG(x, y, args...) SDBG.debugln(x, y, args);	
#else
	#define DEVICE_PRINT(x)
	#define DEVICE_PRINT(x, args...)
	#define DEVICE_DEBUG(x, y, args...)
#endif

//Constructors
//_______________________________________________________________________________________________________

Device::Device(bool fromDefaults)
{
	_id = _nextId++;
	if(fromDefaults) setupFromDefaults();	
	DEVICE_DEBUG(8, F("%d - Device created"), _id);
}

Device::Device(int8_t stepPin, int8_t dirPin, int32_t maxPosition)
{
	_id = _nextId++;
	setup(stepPin, dirPin, maxPosition);	
	DEVICE_DEBUG(8, F("%d - Device created"), _id);
}

Device::~Device()
{
	DEVICE_DEBUG(8, F("%d - Device deleted"), _id);	
}

//Configuration
//_______________________________________________________________________________________________________

uint8_t Device::_nextId = 0;

void Device::setController(MIDI_DeviceController *controller)
{
	belongsTo = controller;
}

void Device::setup(int8_t stepPin, int8_t dirPin, int32_t maxPosition)
{
	DEVICE_DEBUG(7, F("%d - Setting up device"), _id);
	
	//Set properties from inputs
	setStepPin(stepPin);
	setDirPin(dirPin);
	setMaxPosition(maxPosition);
	
	initialize();
}

void Device::setupFromDefaults()
{
	DEVICE_DEBUG(7, F("%d - Setting up device from defaults"), _id);	
	
	//Set properties from defaults
	setStepPin(Defaults::getStepDefault(_id));
	setDirPin(Defaults::getDirDefault(_id));
	setMaxPosition(Defaults::getMaxPosDefault(_id));
	
	initialize();
}

void Device::initialize()
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

void Device::setStepPin(int8_t pin) { _stepPinMap = pin; }

void Device::setDirPin(int8_t pin) { _dirPinMap = pin; }

void Device::printStatus()
{
	DEVICE_PRINT(F("Status for device %d"), _id);
	DEVICE_PRINT(F("  Enabled: %d"), getEnabled());
	DEVICE_PRINT(F("  Is Enabled: %d"), isEnabled());
	DEVICE_PRINT(F("  Step Pin: %d"), getStepPin());
	DEVICE_PRINT(F("  Step State: %d"), getStepState());
	DEVICE_PRINT(F("  Actual State: %d"), digitalRead(_stepPinMap));
	DEVICE_PRINT(F("  Dir Pin: %d"), getDirPin());
	DEVICE_PRINT(F("  Dir State: %d"), getDirState());
	DEVICE_PRINT(F("  Actual State: %d"), digitalRead(_dirPinMap));
	DEVICE_PRINT(F("  Current Position: %d"), getPosition());
	DEVICE_PRINT(F("  Max Position: %d"), getMaxPosition());
	DEVICE_PRINT(F("  Current Note: %d"), getCurrentNote());
}

uint8_t Device::getID() { return _id; }

int8_t Device::getStepPin() { return _stepPinMap; }
int8_t Device::getDirPin() { return _dirPinMap; }

void Device::setMaxPosition(int32_t value) { _maxPosition = value; }
void Device::setEnabled(bool value) { _enabled = value; }

bool Device::getEnabled() { return _enabled; }

//Operation
//_______________________________________________________________________________________________________

uint16_t *Device::_referencePeriods = MIDI_Periods::calculatedPeriods;

void Device::calibratePosition()
{
	setDirState(LOW);
	_currentPosition = getMaxPosition();
	resetPosition();
}

void Device::zeroPosition() { _currentPosition = 0; }

void Device::assignNote(uint8_t note, uint8_t channel)
{
	_lastNoteSource = channel;
	assignNote(note);
}

void Device::assignPeriod(uint16_t period, uint8_t channel)
{
	_lastNoteSource = channel;
	assignPeriod(period);
}

void Device::clearNote()
{
	DEVICE_DEBUG(7, F("%d - Clearing note"), _id);
	_currentNote = 0;
}

bool Device::getStepState() { return _stepState; }
bool Device::getDirState() { return _dirState; }
int16_t Device::getPosition() { return _currentPosition; }
int16_t Device::getMaxPosition() { return _maxPosition; }
int8_t Device::getCurrentNote() { return _currentNote; }
int16_t Device::getBasePeriod() { return *(_referencePeriods + _currentNote); }
int16_t Device::getCurrentPeriod() { return _currentPeriod; }
uint32_t Device::getDuration() { return _currentDuration; }

bool Device::isEnabled() { return (_enabled && _stepPinMap >= 0); }
bool Device::isAvailable() { return _currentNote == -1; }
bool Device::isAtMaxPosition() { return _currentPosition >= _maxPosition; }
bool Device::isTrackingPosition() { return _maxPosition > 0; }

void Device::resetProperties(bool includePosition)
{
	DEVICE_DEBUG(7, F("%d - Resetting properties"), _id);

	//Set given device's step pin low
	//Used for initially setting up a device during setup
	//Purely for visual effect when calling this during processing; in case there is an LED tied to it.
	setStepState(LOW);

	//Reset device properties
	_currentNote = -1;
	_currentPeriod = 0;
	_currentTick = 0;
	_currentDuration = 0;
	_lastNoteSource = -1;

	if(!includePosition) return;  
	setDirState(LOW);
	zeroPosition();
}

void Device::resetPosition()
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

void Device::setDirection(bool direction)
{
	//Check user is actually changing the direction from the current state
	if(_dirState != direction)
	{
		//Set the desired state for the given device
		_dirState = direction;
		setDirState(_dirState);
		DEVICE_DEBUG(7, F("%d - New direction: %d"), _id, direction);

		//Update it's current position if device is tracking it
		if(isTrackingPosition())
		{
			DEVICE_DEBUG(7, F("%d - Updating position from: %d"), _id, _currentPosition);
			_currentPosition = (_maxPosition - _currentPosition);
			DEVICE_DEBUG(7, F("%d - New position: %d"), _id, _currentPosition);
		}
	}
	else
	{
		DEVICE_DEBUG(7, F("%d - Direction is already: %d"), _id, direction);
	}
}

void Device::assignNote(uint8_t note)
{
	if(!isEnabled()) 
	{
		DEVICE_DEBUG(7, F("%d - Not enabled"), _id);
		return;
	}
	
	_currentNote = note;
	_currentPeriod = getBasePeriod();
	
	DEVICE_DEBUG(7, F("%d - Note %d (%d) assigned"), _id, _currentNote, _currentPeriod);
		
	if(belongsTo) belongsTo->noteAssigned();
}

void Device::assignPeriod(uint16_t period)
{
	if(!isEnabled()) return;
	
	_currentNote = 255;	
	_currentPeriod = period;
	
	DEVICE_DEBUG(7, F("%d - Period %d assigned"), _id, _currentPeriod);

	if(belongsTo) belongsTo->noteAssigned();
}

void Device::pitchBend(uint16_t bend)
{ 
	if (_currentNote > 0 && _currentNote < 255) 
	{
		DEVICE_DEBUG(7, F("%d - Bending: %d"), _id, bend);
		
		int16_t basePeriod = getBasePeriod();
		DEVICE_DEBUG(7, F("%d - Base: %d"), _id, basePeriod);		
		
		float pitchFactor = pow(2.0, (bend - 8192.0) / 8192.0);
		DEVICE_DEBUG(7, F("%d - Factor: %d"), _id, pitchFactor);
		
		int16_t newPeriod = basePeriod / pitchFactor;
		DEVICE_DEBUG(7, F("%d - New Period: %d"), _id, newPeriod);

		_currentPeriod = newPeriod;
	}
}

void Device::setStepState(bool state)
{
	_stepState = state;
	digitalWrite(_stepPinMap, _stepState);
}

void Device::setDirState(bool state)
{
	_dirState = state;
	digitalWrite(_dirPinMap, _dirState);
}

void Device::toggleStep()
{
	//Toggle state for given channel's step pin
	_stepState = !_stepState;
	digitalWrite(_stepPinMap, _stepState);
	
	//Increment position
	if(isTrackingPosition())
		_currentPosition++;
}

void Device::toggleDirection()
{
	//Toggle state for given device's direction pin
	DEVICE_DEBUG(7, F("%d - Direction toggle"), _id);
	_dirState = !_dirState;
	setDirState(_dirState);
}

//Testing/debug
//_______________________________________________________________________________________________________

void Device::testStepping(uint32_t steps)
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

void Device::testStepping(int32_t steps, bool direction)
{
	setDirection(direction);
	testStepping(steps);
}

void Device::testDeviceDirect()
{
	for(int16_t i = 0; i <= 50; i++) 
	{
		testStepping(2);	
		delay(50);
	}
}

void Device::testMaxDirection() 
{
	//Nothing to test if no valid max position set
	if(!isTrackingPosition()) 
	{
		DEVICE_DEBUG(7, F("%d - Not tracking position"), _id);
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

void Device::runAllTests()
{
	testDeviceDirect(); delay(250);
	calibratePosition(); delay(250);
	testMaxDirection(); delay(250);
	calibratePosition();
}