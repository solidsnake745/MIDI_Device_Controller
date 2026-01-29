#include "../MIDI_Device_Controller.h"

//Constructors and instance management
//_______________________________________________________________________________________________________
MIDI_Device_Controller::MIDI_Device_Controller()
{
	//Initialize device collection with nulls
	for(int i = 0; i < MAX_PITCH_DEVICES; i++)
		_pitchDevices[i] = nullptr;
	
	for(int i = 0; i < MAX_PULSE_DEVICES; i++)
		_pulseDevices[i] = nullptr;
	
	//Initialize resolution and periods to the default
	setResolution();
}

MIDI_Device_Controller& MIDI_Device_Controller::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == nullptr)
		_instance = new MIDI_Device_Controller();
	return *_instance;
}

//Device management
//_______________________________________________________________________________________________________
uint8_t MIDI_Device_Controller::reloadEnabledDevices()
{
	_debug.debugln(1, F("Reloading enabled pitch devices"));
	
	//Clear out current references
	for(int i = 0; i < MAX_PITCH_DEVICES; i++)
		_enabledPitchDevices[i] = nullptr;
	
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
		_debug.println(F("Pitch device slot %d"), i);
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

bool MIDI_Device_Controller::addPitchDevice(uint8_t index, Base_MIDI_Pitch* d)
{
	if(!d)
	{
		_debug.println(F("NULL pitch device passed in"));
		return false;
	}
	
	if(index > MAX_PITCH_DEVICES - 1)
	{
		_debug.println(F("Can't add pitch device at index %d; max index is %d"), index, MAX_PITCH_DEVICES - 1);
		return false;
	}

	if(_pitchDevices[index] != nullptr)
	{		
		_debug.println(F("Pitch device already exists at index %d"), index);
		return false;
	}
	
	d->_parent = this;
	d->_id = index;
	_pitchDevices[index] = d;
	return true;
}

int8_t MIDI_Device_Controller::addPitchDevice(Base_MIDI_Pitch* d)
{
	int i = 0;
	while(i < MAX_PITCH_DEVICES)
	{
		if(!_pitchDevices[i])
			return addPitchDevice(i, d) ? i : -1;
		i++;
	}
	
	_debug.println(F("No available indexes; Pitch device not added"));
	return -1;
}

bool MIDI_Device_Controller::addPulseDevice(uint8_t index, Base_MIDI_Pulse* d)
{
	if(!d)
	{
		_debug.println(F("NULL pulse device passed in"));
		return false;
	}
	
	if(index > MAX_PULSE_DEVICES - 1)
	{
		_debug.println(F("Can't add pulse device at index %d; max index is %d"), index, MAX_PULSE_DEVICES - 1);
		return false;
	}

	if(_pulseDevices[index] != nullptr)
	{		
		_debug.println(F("Pulse device already exists at index %d"), index);
		return false;
	}
	
	d->_parent = this;
	d->_id = index;
	_pulseDevices[index] = d;
	return true;
}

int8_t MIDI_Device_Controller::addPulseDevice(Base_MIDI_Pulse* d)
{
	int i = 0;
	while(i < MAX_PULSE_DEVICES)
	{
		if(!_pulseDevices[i])
			return addPulseDevice(i, d) ? i : -1;
		i++;
	}
	
	_debug.println(F("No available indexes; Pulse device not added"));
	return -1;
}

Base_MIDI_Pitch* MIDI_Device_Controller::getPitchDevice(uint8_t index)
{
	if(index > MAX_PITCH_DEVICES - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_PITCH_DEVICES - 1);
		return nullptr;
	}
	
	return _pitchDevices[index];
}

Base_MIDI_Pulse* MIDI_Device_Controller::getPulseDevice(uint8_t index)
{
	if(index > MAX_PULSE_DEVICES - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_PULSE_DEVICES - 1);
		return nullptr;
	}
	
	return _pulseDevices[index];
}

void MIDI_Device_Controller::deletePitchDevice(uint8_t index)
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
		_pitchDevices[index] = nullptr;
		return;
	}
	else
	{
		_debug.debugln(2, F("No device at %d"), index);
	}
}

void MIDI_Device_Controller::deletePulseDevice(uint8_t index)
{
	if(index > MAX_PULSE_DEVICES - 1)
	{
		_debug.debugln(3, F("Max index is %d"), MAX_PULSE_DEVICES - 1);
		return;
	}
	
	if(_pulseDevices[index])
	{
		_debug.debugln(2, F("Removing device at %d"), index);
		delete _pulseDevices[index];
		_pulseDevices[index] = nullptr;
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
		
	int x = -1; //Track max index for _dirDevices
	Base_MIDI_Pitch_Direction* _dirDevices[MAX_PITCH_DEVICES];
	Base_MIDI_Pitch_Direction* dd;
	
	int y = 0;
	while(y < numEnabled)
	{
		Base_MIDI_Pitch* d = _enabledPitchDevices[y++];
		if(!d || !d->hasDirection()) break;
		
		dd = d->asDir();
		dd->setDirection(HIGH);
		_dirDevices[++x] = dd;
	}

	if(x == -1)
		return;

	//Filter out devices not tracking position
	int16_t resetDeviceCount = 0;
	for(y = 0; y <= x; y++)
	{
		if(!dd->isTrackingPosition())
		{
			resetDeviceCount++;
			dd->setDirection(LOW);
			_dirDevices[y] = nullptr;
		}
	}
	
	while(resetDeviceCount != x + 1)
	{
		for(y = 0; y <= x; y++)
		{
			dd = _dirDevices[y];
			if(!dd)
				continue;
			
			if(dd->isAtMaxPosition())
			{
				resetDeviceCount++;
				dd->setDirection(LOW);
				_dirDevices[y] = nullptr;
			}
			else
			{
				dd->stepOnce();
				dd->_currentPosition++;
			}
		}
		
		delayMicroseconds(12500);
	}
}

void MIDI_Device_Controller::calibrateDevicePositions()
{
	uint8_t numEnabled = reloadEnabledDevices();
	startPlaying();
	
	int i = 0;
	while(i < numEnabled)
	{
		Base_MIDI_Pitch_Direction* dd = _enabledPitchDevices[i++]->asDir();
		if(dd)
		{
			dd->setDirOutputState(HIGH);
			dd->_currentPosition = dd->getMaxPosition();
		}		
	}
	
	resetDevicePositions();
}

void MIDI_Device_Controller::playDeviceNote(uint8_t index, uint8_t note)
{
	_debug.debugln(8, F("Is processing: %d"), _isPlayingNotes);
	_debug.debugln(8, F("Auto processing: %d"), _autoPlayNotes);
	
	Base_MIDI_Pitch* d = getPitchDevice(index);
	if(!d) return;
	d->playNote(note);
}

void MIDI_Device_Controller::bendDeviceNote(uint8_t index, int16_t bend, bool shiftRange)
{
	Base_MIDI_Pitch* d = getPitchDevice(index);
	if(!d) return;
	d->bendNote(bend, shiftRange);
}

void MIDI_Device_Controller::stopDeviceNote(uint8_t index, uint8_t note)
{
	Base_MIDI_Pitch* d = getPitchDevice(index);
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
	_debug.debugln(20, F("Pitch device processing start"));	
	int i = 0;
	while(i < _numEnabled && _numEnabled > 0)
	{
		Base_MIDI_Pitch* d1 = _enabledPitchDevices[i++];
		if(!d1) continue;
		d1->processNotes();
	}
	_debug.debugln(20, F("Pitch device processing end"));
	
#if ISR_TESTING >= 2
	endTime = micros();
	_debug.println(F("Pitch device processing time: %u"), (endTime - startTime));
	startTime = micros();
#endif

	//Process Base_MIDI_Pulse devices
	_debug.debugln(20, F("Pulse device processing start"));	
	i = 0;
	while(i < MAX_PULSE_DEVICES)
	{
		Base_MIDI_Pulse* d2 = _pulseDevices[i++];
		if(!d2) continue;
		d2->processNotes();
	}
	_debug.debugln(20, F("Pulse device processing end"));
	
#if ISR_TESTING >= 2
	endTime = micros();
	_debug.println(F("Pulse device processing time: %u"), (endTime - startTime));
	startTime = micros();
#endif

	//Update IO devices
	_debug.debugln(20, F("IO processing start"));
	for(i = 0; i < MAX_IO_DEVICES; i++)
	{
		if(IO_Factory::_ioDevices[i])
			IO_Factory::_ioDevices[i]->updateOutputs();
	}
	_debug.debugln(20, F("IO processing end"));
	
#if ISR_TESTING >= 2
	endTime = micros();
	_debug.println(F("IO processing time: %u"), (endTime - startTime));
#endif
	
	_debug.debugln(20, F("Process end"));
}
#pragma GCC pop_options

#if ARDUINO_ARCH_ESP32
	void IRAM_ATTR MIDI_Device_Controller::lawl() { _instance->processNotes(); };
#endif

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
	_lastAssign = millis();
	
	_debug.debugln(1, F("Starting note processing"));
	_debug.debugln(2, F("Resolution set to %d"), MIDI_Periods::getResolution());
	
	int i = 0;
	int numEnabled = reloadEnabledDevices();
	while(i < numEnabled)
		_enabledPitchDevices[i++]->startPlaying();
	
	_debug.debugln(8, F("Starting interrupt process"));
	_timer->setupOnce(MIDI_Periods::getResolution(), MIDI_Device_Controller::lawl);
	_timer->start();
	
	_clockCount = 0; //Reset counter for MIDI clock events
	LEDOn();
	_debug.debugln(8, F("Started note processing"));
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
		_enabledPitchDevices[i++]->resetProperties();
	
	for(i = 0; i < MAX_IO_DEVICES; i++)
	{
		IO_Device* io = IO_Factory::_ioDevices[i];
		if(io)
			io->stopOutputs();
	}
	
	#ifdef MIDI_Collection_Controller_h	
		MCC.resetAll();
	#endif
	
	i = 0;
	while(i < numEnabled)
		_enabledPitchDevices[i++]->stopPlaying();
	
	_isPlayingNotes = false;
	LEDOff();
	
	_debug.debugln(5, F("Stopped processing"));
}

bool MIDI_Device_Controller::process()
{	
	if(!_isPlayingNotes)
		return false;
	
	//Process vibrato
	uint8_t i = 0;
	while(i < _numEnabled)
	{
		Base_MIDI_Pitch* d = _enabledPitchDevices[i++];		
		if(d->shouldAutoStartVibrato())
			d->startVibrato();
		d->processVibrato();
	}
	
	//Stop playing if past the idle timeout
	uint32_t timeSinceLastAssign = (millis() - _lastAssign);	
	if(timeSinceLastAssign >= _idleTimeout * 1000) 
	{
		stopPlaying();
		return true; //Indicates this method stopped processing
	}
	
	return false;
}

//Tests/Debug
//_______________________________________________________________________________________________________
void MIDI_Device_Controller::testPitchDeviceInterrupt(uint8_t index) 
{
	Base_MIDI_Pitch* d = getPitchDevice(index);
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
	Base_MIDI_Pitch* d = getPitchDevice(index);
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
		Base_MIDI_Pitch* d = _pitchDevices[i];
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
			int magicValue = 6250; //I don't know how I got this number and other values just don't work well
			startPlaying();
			while(i < numEnabled)
			{
				Base_MIDI_Pitch* d = _enabledPitchDevices[i++];
				
				_debug.println(F("Single device sequence on %d"), d->_id);
				for(uint8_t y = 0; y <= 15; y++) 
				{
					d->cycleOutput();
					delayMicroseconds(magicValue);
				}
			}

			_debug.println(F("Parallel device sequence"));
			for(int16_t x = 0; x <= 25; x++) 
			{
				i = 0;
				while(i < numEnabled)
					_enabledPitchDevices[i++]->cycleOutput();
				
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