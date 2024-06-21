#include "../MIDI_Device_Controller.h"

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
	for(int i = 0; i < MAX_PITCH_DEVICES; i++) _enabledPitchDevices[i] = NULL;
	
	//Look for populated devices that are enabled
	uint8_t x = 0;
	for(int i = 0; i < MAX_PITCH_DEVICES; i++)
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

MIDI_Pitch* MIDI_Device_Controller::getDevice(uint8_t index)
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

//TODO: Verify logic
void MIDI_Device_Controller::resetDevicePositions()
{
	uint8_t numEnabled = reloadEnabledDevices();
	startPlaying();
	
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
			
			//This specific delay makes it sounds cool. That is all.
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
	startPlaying();
	
	int i = 0;
	while(i < numEnabled)
	{
		MIDI_Pitch *d = _enabledPitchDevices[i++];

		d->setDirState(HIGH);
		d->_currentPosition = d->getMaxPosition();
	}
	
	resetDevicePositions();
}

void MIDI_Device_Controller::playDeviceNote(uint8_t index, uint8_t note)
{
	_debug.debugln(8, F("Is processing: %d"), _isPlayingNotes);
	_debug.debugln(8, F("Auto processing: %d"), _autoPlayNotes);
	
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	d->playNote(note);
}

void MIDI_Device_Controller::bendDeviceNote(uint8_t index, uint16_t bend)
{
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	d->bendNote(bend);
}

void MIDI_Device_Controller::stopDeviceNote(uint8_t index, uint8_t note)
{
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	if(d->getCurrentNote() == note) d->stopNote();
}

//Note Processing
//_______________________________________________________________________________________________________	

#pragma GCC push_options
#pragma GCC optimize("Ofast")
void MIDI_Device_Controller::processNotes()
{
	_debug.debugln(20, F("Process start"));

#if ISR_TESTING >= 2
	uint32_t startTime = micros();
	uint32_t endTime = 0;
#endif

	//Process MIDI_Pitch devices
	int i = 0;
	while(i < _numEnabled && _numEnabled > 0)
	{
		MIDI_Pitch *d = _enabledPitchDevices[i++];
		if(!d) continue;
		d->playNotes();
	}
	
#if ISR_TESTING >= 2
	endTime = micros();
	_debug.println(F("Pitch device processing time: %u"), (endTime - startTime));
	startTime = micros();
#endif
	
	//Update IO devices
	for(i = 0; i < MAX_IO_DEVICES; i++)
	{
		if(IO_Factory::_ioDevices[i])
			IO_Factory::_ioDevices[i]->updateOutputs();
	}

#if ISR_TESTING >= 2
	endTime = micros();
	_debug.println(F("IO processing time: %u"), (endTime - startTime));
#endif
	
	_debug.debugln(20, F("Process end"));
}
#pragma GCC pop_options

void MIDI_Device_Controller::lawl() { _instance->processNotes(); };

void MIDI_Device_Controller::noteAssigned()
{
	_lastAssign = millis();
	
	if(_autoPlayNotes && !_isPlayingNotes)
	{
		_debug.debugln(8, F("Auto playing..."));
		startPlaying();
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
	
	int i = 0;
	int numEnabled = reloadEnabledDevices();
	while(i < numEnabled)
	{
		MIDI_Pitch *d = _enabledPitchDevices[i++];
		
		//Make sure pitch devices are managing their own duration
		if(d->_stepIO)
			d->_stepIO->setMaxDuration(d->_stepPinMap, 0);
		if(d->_dirIO)
			d->_dirIO->setMaxDuration(d->_dirPinMap, 0);
	}
	
	_timer->setupOnce(MIDI_Periods::getResolution(), MIDI_Device_Controller::lawl);
	_timer->start();
	
	LEDOn();
	return true;
}

void MIDI_Device_Controller::stopPlaying()
{
	_debug.debugln(5, F("Stopping processing"));
	
	//Stop the timer interrupt so it doesn't interfere
	_timer->stop();
	
	//Silence all pitch devices and reset them to an initial state
	int i = 0;
	int numEnabled = reloadEnabledDevices();
	while(i < numEnabled)
	{
		MIDI_Pitch *d = _enabledPitchDevices[i++];
		d->resetProperties();
	}
	
	for(i = 0; i < MAX_IO_DEVICES; i++)
	{
		IO_Device *io = IO_Factory::_ioDevices[i];
		if(io)
			io->stopOutputs();
	}
	
	_isPlayingNotes = false;
	LEDOff();
	
	_debug.debugln(5, F("Stopped processing"));
}

bool MIDI_Device_Controller::process()
{	
	if(!_isPlayingNotes)
		return false;
	
	//Stop playing if past the idle timeout
	uint32_t timeSinceLastAssign = (millis() - _lastAssign);	
	if(timeSinceLastAssign >= _idleTimeout * 1000) 
	{
		stopPlaying();
		
		//Exit out as max durations don't matter if we've stopped playing
		return true; //Indicates this method stopped processing
	}
	
	//Stop any pitch devices over their max duration
	int i = 0;	
	while(i < _numEnabled)
	{
		MIDI_Pitch *d = _enabledPitchDevices[i++];
		d->checkMaxDuration();
	}
	
	//Stop any IO outputs over their max duration
	for(i = 0; i < MAX_IO_DEVICES; i++)
	{
		IO_Device *io = IO_Factory::_ioDevices[i];
		if(io)
			io->checkMaxDuration();
	}
	
	return false;
}

//LED pin functionality
//_______________________________________________________________________________________________________
void MIDI_Device_Controller::LEDOn()
{
	if(_ledPin > -1)
		digitalWrite(_ledPin, HIGH);
}

void MIDI_Device_Controller::LEDOff()
{
	if(_ledPin > -1)
		digitalWrite(_ledPin, LOW);
}

void MIDI_Device_Controller::setLEDPin(int8_t pin) 
{
	_ledPin = pin;
	if(_ledPin > -1)
		pinMode(_ledPin, OUTPUT);
}

//Tests/Debug
//_______________________________________________________________________________________________________
void MIDI_Device_Controller::testPitchDeviceInterrupt(uint8_t index) 
{
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	
	startPlaying();
	for(int16_t i = 0; i <= 5; i++)
	{
		d->playNote(50);
		delay(200);
		d->stopNote();
		delay(200);
	}
	stopPlaying();
}

void MIDI_Device_Controller::testPitchBend(uint8_t index)
{
	MIDI_Pitch *d = getDevice(index);
	if(!d) return;
	
	bool currentSetting = _autoPlayNotes;
	setAutoPlay(true);	
	
	d->playNote(48);
	d->bendNote(0);
	delayMicroseconds(500);
	
	for(int16_t i = 0; i <= 16383; i++) 
	{
		d->bendNote(i, true);
		delayMicroseconds(500);
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
			int magicValue = 4500; //I don't know how I got this number and other values just don't work well
			startPlaying();
			while(i < numEnabled)
			{
				MIDI_Pitch *d = _enabledPitchDevices[i++];				
				
				_debug.println(F("Single device sequence on %d"), d->_id);
				for(uint8_t y = 0; y <= 15; y++) 
				{
					d->toggleStep();
					delayMicroseconds(magicValue);
				}
			}

			_debug.println(F("Parallel device sequence"));
			for(int16_t x = 0; x <= 25; x++) {
				i = 0;
				while(i < numEnabled)
					_enabledPitchDevices[i++]->toggleStep();
				
				delayMicroseconds(magicValue);
			}
			
			_debug.println(F("Pause and reset"));
			delay(500);	
			resetDevicePositions();
			stopPlaying();
		}
		break;
		
		default:
			break;
	}
	
	LEDOff();
}