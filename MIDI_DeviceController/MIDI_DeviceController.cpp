#include "../MIDI_DeviceController.h"

SerialDebug MIDI_DeviceController::_debug(DEBUG_MIDIDEVICE_CONTROLLER);

//Constructors and instance management
//_______________________________________________________________________________________________________

//Global singleton instance
MIDI_DeviceController MDC = MIDI_DeviceController::getInstance();

MIDI_DeviceController *MIDI_DeviceController::_instance = NULL;

MIDI_DeviceController::MIDI_DeviceController()
{
	//Initialize device collection with nulls
	for(int i = 0;i < MAX_DEVICES;i++) _devices[i] = NULL;
	
	//Initialize resolution and periods to the default
	setResolution();
}

MIDI_DeviceController &MIDI_DeviceController::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_DeviceController();
	return *_instance;
}

	
//Device management
//_______________________________________________________________________________________________________
MIDI_Device *MIDI_DeviceController::_devices[MAX_DEVICES];
MIDI_Device *MIDI_DeviceController::_enabledDevices[MAX_DEVICES];

uint8_t MIDI_DeviceController::reloadEnabledDevices()
{
	_debug.debugln(1, F("Reloading enabled devices"));
	
	//Clear out current references
	for(int i = 0;i < MAX_DEVICES;i++) _enabledDevices[i] = NULL;
	
	//Look for populated devices that are enabled
	uint8_t x = 0;
	for(int i = 0;i < MAX_DEVICES;i++)
	{
		if(!_devices[i]) continue;
		if(_devices[i]->isEnabled())
		{
			_debug.debugln(2, "Device %d added", i);
			_enabledDevices[x++] = _devices[i];
		}
		else
		{
			_debug.debugln(2, "Device %d not added", i);
		}
	}
	
	_debug.debugln(1, "%d device(s) loaded", x);
	return x;
}

void MIDI_DeviceController::printStatus()
{
	int i = 0;
	while(i != MAX_DEVICES)
	{
		_debug.println(F("Device Slot %d"), i);
		if(_devices[i])
		{
			_debug.debugln(5, F("Populated"));
			_devices[i]->printStatus();
		}
		else
		{
			_debug.println(F("Empty"));
		}
		
		_debug.println("");
		delay(10);
		i++;
	}
}

void MIDI_DeviceController::addDevice(uint8_t index, MIDI_Device *d)
{
	if(index > MAX_DEVICES - 1)
	{
		_debug.println(F("Can't add device at index %d"), index);
		_debug.debugln(5, F("Max device index is %d"), MAX_DEVICES - 1);
		return;
	}

	if(_devices[index] != NULL)
	{		
		_debug.println(F("Device already exists at index %d"), index);
		return;
	}
	
	d->setController(this);
	d->setID(index);
	_devices[index] = d;
}

void MIDI_DeviceController::addDevices(MIDI_Device devices[], uint8_t numDevices)
{
	if(numDevices > MAX_DEVICES)
		numDevices = MAX_DEVICES;
		
	_debug.debugln(5, F("Attempting to add %d device(s)"), numDevices);
	
	uint8_t i = 0;
	while(i < numDevices)
	{
		addDevice(i, &devices[i]);
		i++;
	}
}

MIDI_Device *MIDI_DeviceController::getDevice(uint8_t index)
{
	if(index > MAX_DEVICES - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_DEVICES - 1);
		return NULL;
	}
	
	return _devices[index];
}

void MIDI_DeviceController::deleteDevice(uint8_t index)
{
	if(index > MAX_DEVICES - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_DEVICES - 1);
		return;
	}
	
	if(_devices[index])
	{
		_debug.debugln(2, F("Removing device at %d"), index);
		delete _devices[index];
		_devices[index] = NULL;
		return;
	}
	else
	{
		_debug.debugln(2, F("No device at %d"), index);
	}
}

//TODO: Verify logic
void MIDI_DeviceController::resetPositions()
{
	uint8_t numEnabled = reloadEnabledDevices();
	
	int i = 0;
	while(i < numEnabled)
	{
		MIDI_Device *d = _enabledDevices[i++];
		if(!d) break;
		
		d->setDirection(HIGH);
		d->setStepState(LOW);
	}

	int16_t resetDeviceCount = 0;
	while(resetDeviceCount != numEnabled)
	{
		resetDeviceCount = 0;
		for(i = 0; i < numEnabled; i++)
		{
			MIDI_Device *d = _enabledDevices[i];
			
			//TODO: refactor out check on isTrackingPosition
			if(!d->isTrackingPosition() || d->isAtMaxPosition())
				resetDeviceCount++;
			else
				d->toggleStep();
			
			delayMicroseconds(1530);
		}
	}
	
	i = 0;
	while(i < numEnabled)
	{
		MIDI_Device *d = _enabledDevices[i++];
		
		d->setDirection(LOW);
		d->setStepState(LOW);
	}
}

void MIDI_DeviceController::calibratePositions()
{
	uint8_t numEnabled = reloadEnabledDevices();
	
	int i = 0;
	while(i < numEnabled)
	{
		MIDI_Device *d = _enabledDevices[i++];

		d->setDirState(HIGH);
		d->_currentPosition = d->getMaxPosition();
	}
	
	resetPositions();
}

void MIDI_DeviceController::assignNote(int8_t index, uint8_t note)
{
	_debug.debugln(8, F("Is processing: %d"), _isProcessing);
	_debug.debugln(8, F("Auto processing: %d"), _autoProcessing);
	
	MIDI_Device *d = getDevice(index);
	if(!d) return;
	d->assignNote(note);
}

void MIDI_DeviceController::pitchBend(int8_t index, uint16_t bend)
{
	MIDI_Device *d = getDevice(index);
	if(!d) return;
	d->pitchBend(bend);
}

void MIDI_DeviceController::clearNote(int8_t index, uint8_t note)
{
	MIDI_Device *d = getDevice(index);
	if(!d) return;
	if(d->getCurrentNote() == note) d->clearNote();
}

//Note Processing
//_______________________________________________________________________________________________________	

void MIDI_DeviceController::lawl() { _instance->processNotes(); };

void MIDI_DeviceController::noteAssigned()
{
	if(_autoProcessing)
	{
		_debug.debugln(8, F("Auto process"));
		_lastAssign = millis();
		if(!_isProcessing) startProcessing();
	}
}

bool MIDI_DeviceController::startProcessing() 
{
	if(_isProcessing)
	{
		_debug.debugln(5, F("Already processing"));
		return false;
	}
	
	_isProcessing = true;
	
	//TODO: Redesign and implement debugging the interrupt process
	// #if DEBUG_MODE >= 5
		// MIDI_Periods::_currentResolution = 1000000; //1 second interval
		// MIDI_Periods::calculatedPeriods[50] = 3;		
	// #endif
	
	_debug.debugln(1, F("Starting note processing"));
	_debug.debugln(2, F("Resolution set to %d"), MIDI_Periods::getResolution());
	
	reloadEnabledDevices();
	if(_autoProcessing) _lastAssign = millis();
	
	Timer1.initialize();
	Timer1.attachInterrupt(MIDI_DeviceController::lawl);
	Timer1.setPeriod(MIDI_Periods::getResolution());
	Timer1.start();
	
	LEDOn();
	return true;
}

void MIDI_DeviceController::stopProcessing()
{
	_debug.debugln(5, F("Stopping processing"));
	
	//Silence all devices and reset them to an initial state
	int i = 0;
	while(i != MAX_DEVICES)
	{
		if(_devices[i]) _devices[i]->clearNote();
		i++;
	}
	
	delay(5); //Give the interrupt process some time to rest our devices

	Timer1.stop();

	_isProcessing = false;
	LEDOff();
}

// #if DEBUG_MODE >= 3
	// int lastDebugValue = -1;
// #endif

bool MIDI_DeviceController::resetProcessing()
{
	if(!_isProcessing) return false;
	
	uint32_t timeSinceLastAssign = (millis() - _lastAssign);
	
	// #if DEBUG_MODE >= 3
		// if(timeSinceLastAssign % 1000 == 0)
		// {
			// int approxSeconds = timeSinceLastAssign / 1000;
			// if(approxSeconds > lastDebugValue)
			// {
				// lastDebugValue = approxSeconds;
				// DEBUG2(F("Seconds since last assign: "), approxSeconds)
			// }
		// }
	// #endif
	
	if(timeSinceLastAssign >= _idleTimeout * 1000) 
	{
		// #if DEBUG_MODE >= 3
			// lastDebugValue = -1;
		// #endif
		stopProcessing();
		return true; //Indicates this method stopped processing
	}
	
	return false;
}

bool MIDI_DeviceController::isProcessing()
{
	return _isProcessing;
}


//Settings
//_______________________________________________________________________________________________________
uint8_t MIDI_DeviceController::getMaxDevices() { return MAX_DEVICES; }
uint32_t MIDI_DeviceController::getMaxDuration() { return _maxDuration; }
void MIDI_DeviceController::setMaxDuration(uint32_t value) { _maxDuration = value; }
void MIDI_DeviceController::setIdleTimeout(int16_t value) { _idleTimeout = value; }
void MIDI_DeviceController::setAutoProcess(bool value) { _autoProcessing = value; }
void MIDI_DeviceController::setResolution(uint16_t resolution) { MIDI_Periods::setResolution(resolution); }


//LED pin functionality
//_______________________________________________________________________________________________________
void MIDI_DeviceController::LEDOn()
{
	if(_ledPin > -1) digitalWrite(_ledPin, HIGH);
}

void MIDI_DeviceController::LEDOff()
{
	if(_ledPin > -1) digitalWrite(_ledPin, LOW);
}

void MIDI_DeviceController::setLEDPin(int8_t pin) 
{
	_ledPin = pin;
	if(_ledPin > -1) pinMode(_ledPin, OUTPUT);
}


//Tests/Debug
//_______________________________________________________________________________________________________
void MIDI_DeviceController::testDeviceInterrupt(uint8_t index) 
{
	MIDI_Device *d = getDevice(index);
	if(!d) return;
	
	if(!_isProcessing) startProcessing();
	for(int16_t i = 0; i <= 5; i++)
	{
		d->assignNote(50);
		delay(200);
		d->clearNote();
		delay(200);
	}
	stopProcessing();
}

void MIDI_DeviceController::testPitchBend(uint8_t index)
{
	MIDI_Device *d = getDevice(index);
	if(!d) return;
	
	bool currentSetting = _autoProcessing;		
	setAutoProcess(true);	
	
	d->assignNote(50);
	d->pitchBend(1);
	
	//Note processing occurs every (_resolution) microseconds
	//Thus effectively creating a delay between pitchBend calls
	//TODO: Verify above comment again
	for(int16_t i = 2; i <= 16383; i+=2500) 
	{
		d->pitchBend(i);
		//delayMicroseconds(5000);
	}
	
	d->clearNote();
	stopProcessing();
	setAutoProcess(currentSetting);
}

void MIDI_DeviceController::loadTest(uint8_t numDevices)
{
	//Temporarily enable auto process and set idle timeout to 5 seconds
	bool currentSetting = _autoProcessing;	
	uint16_t currentTimeout = _idleTimeout;
	setAutoProcess(true);	
	setIdleTimeout(5);
	
	//Restrict parameter to the upper bound of the array
	if(numDevices > MAX_DEVICES)
		numDevices = MAX_DEVICES;
	
	//Start the test - Assign notes across active devices
	for(int16_t i = 0; i < numDevices; i++) 
	{
		MIDI_Device *d = _devices[i];
		if(!d) continue;
		
		delay(250); //Staggers note assignments
		d->assignNote(50);	  
	}
	
	//Hold here until the test is over (idle timeout of 5 seconds)
	_debug.println(F("Waiting 5 seconds"));
	while(!resetProcessing()) {;}
	
	//Revert back to user settings
	setAutoProcess(currentSetting);
	setIdleTimeout(currentTimeout);
}

void MIDI_DeviceController::playStartupSequence(uint8_t version)
{
	uint8_t numEnabled = reloadEnabledDevices();
	
	LEDOn();	
	switch(version)
	{
		case 0:
		{
			int i = 0;
			while(i < numEnabled)
			{
				MIDI_Device *d = _enabledDevices[i++];
				
				_debug.println(F("Single device sequence on %d"), d->_id);
				for(uint8_t y = 0; y <= 9; y++) 
				{
					d->toggleStep();
					delayMicroseconds(12345);
					d->toggleStep();
				}
			}

			_debug.println(F("Parallel device sequence"));
			for(int16_t x = 0; x <= 10; x++) {
				i = 0;
				while(i < numEnabled)
					_enabledDevices[i++]->toggleStep();
				
				delayMicroseconds(12345);
				
				i = 0;
				while(i < numEnabled)
					_enabledDevices[i++]->toggleStep();
			}
			
			_debug.println(F("Pause and reset"));
			delay(500);	
			resetPositions();
		}
		break;
		
		default: break;
	}
	
	LEDOff();
}