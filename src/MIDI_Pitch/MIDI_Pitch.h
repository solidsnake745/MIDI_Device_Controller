#ifndef MIDI_Pitch_h
	#define MIDI_Pitch_h

	#include <Arduino.h>
	#include "../Common/PitchBend.h"
	#include "../MIDI_Device_Controller/MIDI_Periods.h"
	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"

	//Forward declaration for compiling
	class MIDI_Device_Controller;

	///MIDI device class for anything needing a pitch signal (FDD, HDD, Stepper motors, etc.)
	class MIDI_Pitch
	{	
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_MIDIPITCH);
		
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
			IO_Device* _stepIO = NULL;

			//Designated direction pin mapping where -1 indicates no pin is assigned					
			int8_t _dirPinMap = -1;
			IO_Device* _dirIO = NULL;
			
			//Max number of steps the device can take before needing to switch directions where -1 indicates it does not have a max
			//NOTE: Setting to -1 disables position tracking functionality all together
			int16_t _maxPosition = -1;
			
			MIDI_Device_Controller *_belongsTo = NULL;
			
			void setController(MIDI_Device_Controller *controller);
			inline void setID(uint8_t value) { _id = value; };
			
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
			void setStepPin(IO_DeviceEnum device, int8_t pin);
			
			//Sets the associated direction pin
			void setDirPin(IO_DeviceEnum device, int8_t pin);
			
			inline void setMaxPosition(int32_t value) { _maxPosition = value; };
			
		//Operation
		//_____________________________________________________________________________________________
		private:
			//Calculated microperiods based on the set resolution for each note
			static uint16_t *_referencePeriods;
		
			//Current step pin state
			volatile bool _stepState = LOW;
			
			//Current direction pin state
			volatile bool _dirState = LOW;
		
			//Current assigned note where -1 indicates available and 0 indicates awaiting reset
			volatile int8_t _currentNote = -1;

			//Current period that should be playing
			volatile int16_t _currentPeriod = 0;
			
			//Current tick value
			volatile uint16_t _currentTick = 0;

			//How long the current note has been playing in microseconds
			volatile uint32_t _currentDuration = 0;
			
			//Current position value
			volatile int16_t _currentPosition = 0;
			
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
			void playNotes();
			
			//Checks if deivce is past the max duration and stops playing the current note if so
			void checkMaxDuration();
			
		public:
			bool getStepState();
			bool getDirState();
			inline int16_t getPosition() { return _currentPosition; };
			inline int8_t getCurrentNote() { return _currentNote; };
			
			inline bool isEnabled() { return (_stepPinMap >= 0); };
			
			void playNote(uint8_t note);		
			void playPeriod(uint16_t period);
			void bendNote(int bend, bool shiftRange = false);
			void bendNoteByFactor(float pitchFactor);
			void stopNote();
			
			//Used to set the state of the direction pin associated with a given device
			//Respects position and updates it accordingly
			//TODO: rename to updateDirection
			void setDirection(bool direction);
			
			void resetPosition();
			
			//Used to reset devices to their 0 position
			void calibratePosition();
			
			//Sets the given device's position to 0
			inline void zeroPosition() { _currentPosition = 0; };
			
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