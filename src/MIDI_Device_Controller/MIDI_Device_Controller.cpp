#include "../MIDI_Device_Controller.h"

SerialDebug MIDI_Device_Controller::_debug(DEBUG_MIDIDEVICE_CONTROLLER);

//Constructors and instance management
//_______________________________________________________________________________________________________

//Global singleton instance
MIDI_Device_Controller MDC = MIDI_Device_Controller::getInstance();

MIDI_Device_Controller *MIDI_Device_Controller::_instance = NULL;

MIDI_Device_Controller::MIDI_Device_Controller()
{
	//Initialize device collection with nulls
	for(int i = 0;i < MAX_PITCH_DEVICES;i++) _pitchDevices[i] = NULL;
	
	//Initialize resolution and periods to the default
	setResolution();
}

MIDI_Device_Controller &MIDI_Device_Controller::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new MIDI_Device_Controller();
	return *_instance;
}

	
//Device management
//_______________________________________________________________________________________________________
MIDI_Pitch *MIDI_Device_Controller::_pitchDevices[MAX_PITCH_DEVICES];
MIDI_Pitch *MIDI_Device_Controller::_enabledPitchDevices[MAX_PITCH_DEVICES];
uint8_t MIDI_Device_Controller::_numEnabled = 0;

uint8_t MIDI_Device_Controller::reloadEnabledDevices()
{
	_debug.debugln(1, F("Reloading enabled pitch devices"));
	
	//Clear out current references
	for(int i = 0;i < MAX_PITCH_DEVICES;i++) _enabledPitchDevices[i] = NULL;
	
	//Look for populated devices that are enabled
	uint8_t x = 0;
	for(int i = 0;i < MAX_PITCH_DEVICES;i++)
	{
		if(!_pitchDevices[i]) continue;
		if(_pitchDevices[i]->isEnabled())
		{
			_debug.debugln(2, F("Device %d added"), i);
			_enabledPitchDevices[x++] = _pitchDevices[i];
		}
		else
		{
			_debug.debugln(2, F("Device %d not added"), i);
		}
	}
	
	_numEnabled = x;
	_debug.debugln(1, F("%d pitch device(s) loaded"), _numEnabled);
	return _numEnabled;
}

MIDI_Shift_Register *MIDI_Device_Controller::_MSR_instance = NULL;

MIDI_Shift_Register *MIDI_Device_Controller::getShiftRegister()
{
	if (_MSR_instance == NULL)
		_debug.debugln(1, F("Shift register hasn't been initialized yet"));
	return _MSR_instance;
}

void MIDI_Device_Controller::printStatus()
{
	int i = 0;
	while(i != MAX_PITCH_DEVICES)
	{
		_debug.println(F("Pitch device Slot %d"), i);
		if(_pitchDevices[i])
		{
			_debug.debugln(5, F("Populated"));
			_pitchDevices[i]->printStatus();
		}
		else
		{
			_debug.println(F("Empty"));
		}
		
		_debug.println(F(""));
		delay(10);
		i++;
	}
}

void MIDI_Device_Controller::addDevice(uint8_t index, MIDI_Pitch *d)
{
	if(!d)
	{
		_debug.println(F("NULL pitch device passed in"));
		return;
	}
	
	if(index > MAX_PITCH_DEVICES - 1)
	{
		_debug.println(F("Can't add pitch device at index %d; max index is %d"), index, MAX_PITCH_DEVICES - 1);
		return;
	}

	if(_pitchDevices[index] != NULL)
	{		
		_debug.println(F("Pitch device already exists at index %d"), index);
		return;
	}
	
	d->setController(this);
	d->setID(index);
	_pitchDevices[index] = d;
}

void MIDI_Device_Controller::addDevices(MIDI_Pitch *devices[], uint8_t numDevices)
{
	if(numDevices > MAX_PITCH_DEVICES)
		numDevices = MAX_PITCH_DEVICES;
		
	_debug.debugln(5, F("Attempting to add %d device(s)"), numDevices);
	
	uint8_t i = 0;
	while(i < numDevices)
	{
		addDevice(i, devices[i]);
		i++;
	}
}

MIDI_Pitch *MIDI_Device_Controller::getDevice(uint8_t index)
{
	if(index > MAX_PITCH_DEVICES - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_PITCH_DEVICES - 1);
		return NULL;
	}
	
	return _pitchDevices[index];
}

void MIDI_Device_Controller::deleteDevice(uint8_t index)
{
	if(index > MAX_PITCH_DEVICES - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_PITCH_DEVICES - 1);
		return;
	}
	
	if(_pitchDevices[index])
	{
		_debug.debugln(2, F("Removing device at %d"), index);
		delete _pitchDevices[index];
		_pitchDevices[index] = NULL;
		return;
	}
	else
	{
		_debug.debugln(2, F("No device at %d"), index);
	}
}

void MIDI_Device_Controller::initializeShiftRegister(uint8_t size, uint8_t startingNote, uint8_t latchPin)
{
	if(_MSR_instance != NULL)
	{
		_debug.debugln(1, F("Shift register device already initialized"));
		return;
	}
	
	_MSR_instance = new MIDI_Shift_Register(size, startingNote, latchPin);
	_MSR_instance->setController(this);
}

//TODO: Verify logic
void MIDI_Device_Controller::resetDevicePositions()
{
	uint8_t numEnabled = reloadEnabledDevices();
	
	int i = 0;
	while(i < numEnabled)
	{
		MIDI_Pitch *d = _enabledPitchDevices[i++];
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
			MIDI_Pitch *d = _enabledPitchDevices[i];
			
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
		MIDI_Pitch *d = _enabledPitchDevices[i++];
		
		d->setDirection(LOW);
		d->setStepState(LOW);
	}
}

void MIDI_Device_Controller::calibrateDevicePositions()
{
	uint8_t numEnabled = reloadEnabledDevices();
	
	int i = 0;
	while(i < numEnabled)
	{
		MIDI_Pitch *d = _enabledPitchDevices[i++];

		d->setDirState(HIGH);
		d->_currentPosition = d->getMaxPosition();
	}
	
	resetDevicePositions();
}

void MIDI_Device_Controller::playDeviceNote(int8_t index, uint8_t note)
{
	_debug.debugln(8, F("Is processing: %d"), _isPlayingNotes);
	_debug.debugln(8, F("Auto processing: %d"), _autoPlayNotes);
	
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	d->playNote(note);
}

void MIDI_Device_Controller::bendDeviceNote(int8_t index, uint16_t bend)
{
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	d->bendNote(bend);
}

void MIDI_Device_Controller::stopDeviceNote(int8_t index, uint8_t note)
{
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	if(d->getCurrentNote() == note) d->stopNote();
}

void MIDI_Device_Controller::playRegisterNote(uint8_t note)
{
	if(_MSR_instance == NULL)
	{
		_debug.debugln(1, F("Shift register device need to be initialized"));
		return;
	}
	
	_debug.debugln(2, F("Playing %d on register"), note);
	_MSR_instance->playNote(note);
}

void MIDI_Device_Controller::stopRegisterNote(uint8_t note)
{
	if(_MSR_instance == NULL)
	{
		_debug.debugln(1, F("Shift register device need to be initialized"));
		return;
	}
	
	_debug.debugln(2, F("Stopping %d on register"), note);
	_MSR_instance->stopNote(note);
}

//Note Processing
//_______________________________________________________________________________________________________	

void MIDI_Device_Controller::lawl() { _instance->processNotes(); };

void MIDI_Device_Controller::noteAssigned()
{
	if(_autoPlayNotes)
	{
		_debug.debugln(8, F("Auto playing..."));
		_lastAssign = millis();
		if(!_isPlayingNotes) startPlaying();
	}
}

bool MIDI_Device_Controller::startPlaying() 
{
	if(_isPlayingNotes)
	{
		_debug.debugln(5, F("Already processing"));
		return false;
	}
	
	_isPlayingNotes = true;
	
	_debug.debugln(1, F("Starting note processing"));
	_debug.debugln(2, F("Resolution set to %d"), MIDI_Periods::getResolution());
	
	reloadEnabledDevices();
	if(_autoPlayNotes) _lastAssign = millis();
	
	Timer1.initialize();
	Timer1.attachInterrupt(MIDI_Device_Controller::lawl);
	Timer1.setPeriod(MIDI_Periods::getResolution());
	Timer1.start();
	
	LEDOn();
	return true;
}

void MIDI_Device_Controller::stopPlaying()
{
	_debug.debugln(5, F("Stopping processing"));
	
	//Silence all pitch devices and reset them to an initial state
	int i = 0;
	while(i != MAX_PITCH_DEVICES)
	{
		if(_pitchDevices[i]) _pitchDevices[i]->stopNote();
		i++;
	}
	
	delay(5); //Give the interrupt process some time to rest those devices

	Timer1.stop();
	
	if(_MSR_instance)
		_MSR_instance->stopNotes();

	_isPlayingNotes = false;
	LEDOff();
}

// #if DEBUG_MODE >= 3
	// int lastDebugValue = -1;
// #endif

bool MIDI_Device_Controller::process()
{
	if(!_isPlayingNotes) return false;
	
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
		stopPlaying();
		return true; //Indicates this method stopped processing
	}
	
	return false;
}

bool MIDI_Device_Controller::isPlayingNotes() { return _isPlayingNotes; }


//Settings
//_______________________________________________________________________________________________________
bool MIDI_Device_Controller::isAutoPlayEnabled() { return _autoPlayNotes; }
uint8_t MIDI_Device_Controller::getMaxPitchDevices() { return MAX_PITCH_DEVICES; }
uint32_t MIDI_Device_Controller::getMaxDuration() { return _maxDuration; }
void MIDI_Device_Controller::setMaxDuration(uint32_t value) { _maxDuration = value; }
void MIDI_Device_Controller::setIdleTimeout(int16_t value) { _idleTimeout = value; }
void MIDI_Device_Controller::setAutoPlay(bool value) { _autoPlayNotes = value; }
void MIDI_Device_Controller::setResolution(uint16_t resolution) { MIDI_Periods::setResolution(resolution); }
void MIDI_Device_Controller::setDebugResolution() { MIDI_Periods::setDebugResolution(); }

//LED pin functionality
//_______________________________________________________________________________________________________
void MIDI_Device_Controller::LEDOn()
{
	if(_ledPin > -1) digitalWrite(_ledPin, HIGH);
}

void MIDI_Device_Controller::LEDOff()
{
	if(_ledPin > -1) digitalWrite(_ledPin, LOW);
}

void MIDI_Device_Controller::setLEDPin(int8_t pin) 
{
	_ledPin = pin;
	if(_ledPin > -1) pinMode(_ledPin, OUTPUT);
}


//Tests/Debug
//_______________________________________________________________________________________________________
void MIDI_Device_Controller::testPitchDeviceInterrupt(uint8_t index) 
{
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	
	if(!_isPlayingNotes) startPlaying();
	for(int16_t i = 0; i <= 5; i++)
	{
		d->playNote(50);
		delay(200);
		d->stopNote();
		delay(200);
	}
	stopPlaying();
}

void MIDI_Device_Controller::testRegisterDeviceInterrupt()
{
	MIDI_Shift_Register *r = getShiftRegister();
	if(!r) return;
	
	uint32_t currentDuration = r->getDuration();	
	r->setDuration(60000);
	
	if(!_isPlayingNotes) startPlaying();
	for(int16_t i = r->getStartingNote(); i <= r->getEndingNote(); i++)
	{
		r->playNote(i);
		delay(50);
	}
	
	delay(500);
	stopPlaying();
	r->setDuration(currentDuration);
}

void MIDI_Device_Controller::testPitchBend(uint8_t index)
{
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	
	bool currentSetting = _autoPlayNotes;		
	setAutoPlay(true);	
	
	d->playNote(50);
	d->bendNote(1);
	
	//Note processing occurs every (_resolution) microseconds
	//Thus effectively creating a delay between bendNote calls
	//TODO: Verify above comment again
	for(int16_t i = 2; i <= 16383; i+=1200) 
	{
		d->bendNote(i);
		delay(50);
	}
	
	d->stopNote();
	stopPlaying();
	setAutoPlay(currentSetting);
}

void MIDI_Device_Controller::loadTest(uint8_t numDevices)
{
	//Temporarily enable auto process and set idle timeout to 5 seconds
	bool currentSetting = _autoPlayNotes;	
	uint16_t currentTimeout = _idleTimeout;
	setAutoPlay(true);	
	setIdleTimeout(5);
	
	//Restrict parameter to the upper bound of the array
	if(numDevices > MAX_PITCH_DEVICES)
		numDevices = MAX_PITCH_DEVICES;
	
	//Start the test - Assign notes across active devices
	for(int16_t i = 0; i < numDevices; i++) 
	{
		MIDI_Pitch *d = _pitchDevices[i];
		if(!d) continue;
		
		delay(250); //Staggers note assignments
		d->playNote(50);	  
	}
	
	//Hold here until the test is over (idle timeout of 5 seconds)
	_debug.println(F("Waiting 5 seconds"));
	while(!process()) {;}
	
	//Revert back to user settings
	setAutoPlay(currentSetting);
	setIdleTimeout(currentTimeout);
}

void MIDI_Device_Controller::playStartupSequence(uint8_t version)
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
				MIDI_Pitch *d = _enabledPitchDevices[i++];
				
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
					_enabledPitchDevices[i++]->toggleStep();
				
				delayMicroseconds(12345);
				
				i = 0;
				while(i < numEnabled)
					_enabledPitchDevices[i++]->toggleStep();
			}
			
			_debug.println(F("Pause and reset"));
			delay(500);	
			resetDevicePositions();
		}
		break;
		
		default: break;
	}
	
	LEDOff();
}