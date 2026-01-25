#ifndef MIDI_Pitch_h
	#define MIDI_Pitch_h

	#include "../Common/PitchBend.h"
	#include "../Common/NoteDuration.h"
	#include "../Common/MIDI_Periods.h"
	#include "../Common/SinWave.h"
	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"

	//Forward declaration for compiling
	class MIDI_Device_Controller;

	///MIDI device class for anything needing a pitch signal (FDD, HDD, Stepper motors, etc.)
	class MIDI_Pitch
	{	
		//Give access to all private members to the below classes
		friend class MIDI_Device_Controller;		
		friend class MIDI_Pitch_Node;
		friend class Base_MIDI_Pitch_Collection;
		friend class Direct_Collection;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_MIDIPITCH);
		
		enum Effect
		{
			None = 0,
			PitchBend = 1,
			Vibrato = 2
		};
		
		//Constructors
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			MIDI_Pitch();
			~MIDI_Pitch();
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			uint8_t _id;
		
			//Designated step pin mapping where -1 indicates no pin is assigned
			int8_t _stepPinMap = -1;
			IO_Device* _stepIO = nullptr;

			//Designated direction pin mapping where -1 indicates no pin is assigned					
			int8_t _dirPinMap = -1;
			IO_Device* _dirIO = nullptr;
			
			//Max number of steps the device can take before needing to switch directions where -1 indicates it does not have a max
			//NOTE: Setting to -1 disables position tracking functionality all together
			int16_t _maxPosition = -1;
			
			MIDI_Device_Controller* _parent = nullptr;
			
			//Indicates whether a device is at or beyond it's max position    	
			inline bool isAtMaxPosition() { return _currentPosition >= _maxPosition; };
			
		public:
			//Print out this device's configuration
			void printStatus();
			
			inline uint8_t getID() { return _id; };
			inline int8_t getStepPin() { return _stepPinMap; };
			inline int8_t getDirPin() { return _dirPinMap; };
			inline int16_t getMaxPosition() { return _maxPosition; };
			
			//Indicates whether a device is available for note assignment
			inline bool isAvailable() { return _currentNote == -1; };
			
			//Indicates whether a device is tracking and changing direction
			inline bool isTrackingPosition() { return _maxPosition > 0; }; 

			//Sets the associated step pin
			void setStepPin(IOType type, int8_t pin);
			
			//Sets the associated direction pin
			void setDirPin(IOType type, int8_t pin);
			
			inline void setMaxPosition(int32_t value) { _maxPosition = value; };
			
		//Operation
		//_____________________________________________________________________________________________
		private:
			//Calculated microperiods based on the set resolution for each note
			inline static uint16_t* _referencePeriods = MIDI_Periods::calculatedPeriods;
			
			//TODO: volatile modifier may not be necessary or useful, have to research further

			//Current assigned note where -1 indicates available and 0 indicates awaiting reset
			volatile int8_t _currentNote = -1;

			//Current period that should be playing
			volatile int16_t _currentPeriod = 0;
			
			//Current tick value
			volatile uint16_t _currentTick = 0;

			//How long the device has been playing
			NoteDuration _currentDuration;
			
			//Current position value
			volatile int16_t _currentPosition = 0;
			
			Effect _currentEffect = None;
			
			//The last object to assign a note to this device
			void* _lastAssignedBy = nullptr;
			
			//Gets the base period of the note currently being played
			inline int16_t getBasePeriod() { return *(_referencePeriods + _currentNote); };
			
			//Gets the period currently being played
			inline int16_t getCurrentPeriod() { return _currentPeriod; };
			
			//Sets the state of the associated step pin
			void setStepState(bool state); 
			
			//Sets the state of the associated direction pin
			void setDirState(bool state);

			//Toggles the step pin associated with the given channel
			void toggleStep();
			
			//Toggles the direction pin associated with the given channel
			void toggleDirection();
		
			//Resets a device's operational properties
			void resetProperties(bool includePosition = false);
			
			//Operates device per desired MIDI output
			void processNotes();
			
			//Private methods for playing notes used by collections to indicate assignedBy
			void playNote(uint8_t note, void* assignedBy);
			void playPeriod(uint16_t period, void* assignedBy);
			
			//Vibrato related properties
			//Used to track when to execute the next vibrato event
			uint16_t _vibratoTick = 0;
			
			//Used to track the degree of vibrato to apply next
			uint16_t _vibratoDegree = 0;
			
			//Amount of vibrato to apply in units of pitchbend (0 to 8192)
			uint16_t _vibratoAmount = 0;
			
			//Frequency of vibrato changes in microseconds (change occurs every _vibratoRate microseconds)
			uint16_t _vibratoRate = 0;
			
			//Controls whether vibrato is automatically applied after _vibratoDelay passes
			bool _vibratoAutoStart = false;
			
			//How much to delay before applying vibrato
			NoteDuration _vibratoDelay;
			
			inline bool shouldAutoStartVibrato()
			{				
				if(!_vibratoAutoStart || _currentEffect == Vibrato)
					return false; //Not set to use auto start or already processing vibrato
				
				if(_currentDuration <= _vibratoDelay)
					return false; //Vibrato delay has not yet been reached
				
				return true;
			};
			
			inline void processVibrato()
			{
				if(_currentEffect != Vibrato)
					return; //Vibrato not being applied
				
				if(_vibratoTick < _vibratoRate)
					return; //Not yet time to increment
				
				if(_vibratoDegree > 359)
					_vibratoDegree = 0;
				
				int16_t b = _vibratoAmount * SinWave::getSinValue(_vibratoDegree++);
				_currentPeriod = getBasePeriod() / PitchBend::calculateFactor(b);
				_vibratoTick = 0;
			}
			
		public:
			bool getStepState();
			bool getDirState();
			inline int16_t getPosition() { return _currentPosition; };
			inline int8_t getCurrentNote() { return _currentNote; };
			
			inline bool isEnabled() { return (_stepPinMap >= 0); };
			
			inline void playNote(uint8_t note) { playNote(note, nullptr); };
			inline void playPeriod(uint16_t period) { playPeriod(period, nullptr); };
			void bendNote(int16_t bend, bool shiftRange = false);
			void bendNoteByFactor(float pitchFactor);
			void stopNote();
			
			//Used to set the state of the direction pin associated with a given device
			//Respects position and updates it accordingly
			void setDirection(bool direction);
			
			void resetPosition();
			
			//Used to reset devices to their 0 position
			void calibratePosition();
			
			//Sets the given device's position to 0
			inline void zeroPosition() { _currentPosition = 0; };
			
			//Sets vibrato properties: amount (of pitchbend, 0 to 8191) and rate (delay between updates in microseconds)
			inline void setVibrato(uint16_t amount, uint16_t rate) { _vibratoAmount = amount; _vibratoRate = rate; };
			
			//Sets auto start for vibrato where true enables it
			inline void setVibratoAutoStart(bool value) { _vibratoAutoStart = value; };
			
			//Sets a delay of when to apply vibrato after a note starts playing
			inline void setVibratoDelay(uint32_t us) { _vibratoDelay.reset(); _vibratoDelay.addMicros(us); }
			
			//Starts vibrato manually if setup, a note is playing, and another effect is not already in place
			inline bool startVibrato() 
			{
				if(_vibratoAmount == 0 || _vibratoRate == 0)
					return false; //Vibrato not completely setup
				
				if(_currentEffect != None)
					return false; //Some other effect in place
				
				if (!(_currentNote > 0 && _currentNote < 256))
					return false; //No note assigned
				
				_currentEffect = Vibrato;
				return true;
			};
			
			//Stops vibrato
			inline void stopVibrato()
			{ 
				if(_currentEffect != Vibrato)
					return;
				
				_currentEffect = None;
				_vibratoTick = 0;
				_vibratoDegree = 0;
			};
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Plays Do-Re-Mi scale
			void testDoReMi(uint8_t ocatve = 0, uint16_t noteDuration = 150, uint16_t noteGap = 50);

			//Tests the stepping capability of the given device
			void testStepping(uint32_t steps);

			void testStepping(int32_t steps, bool direction);
			
			//Steps a given device directly 50 times
			void testDirect();
			
			//Operates a given device to it's max position and back
			void testMaxDirection();
	};
#endif