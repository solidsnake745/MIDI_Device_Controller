#ifndef MIDI_Device_h
	#define MIDI_Device_h

	#include <Arduino.h>
	#include "MIDI_Periods.h"
	#include "../SerialDebug/SerialDebug.h"

	//Forward declaration for compiling
	class MIDI_Device_Controller;

	class MIDI_Device
	{	
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		static SerialDebug _debug;
		
		//Constructors
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			MIDI_Device(int8_t stepPin, int8_t dirPin = -1, int32_t maxPosition = -1);
			~MIDI_Device();
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			uint8_t _id;
			
			//Enabled (disabled devices will not play notes)
			bool _enabled = false;
		
			//Designated step pin mapping where -1 indicates no pin is assigned
			int8_t _stepPinMap = -1;

			//Designated direction pin mapping where -1 indicates no pin is assigned					
			int8_t _dirPinMap = -1;
			
			//Max number of steps the device can take before needing to switch directions where -1 indicates it does not have a max
			//NOTE: Setting to -1 disables position tracking functionality all together
			int16_t _maxPosition = -1;
			
			MIDI_Device_Controller *belongsTo = NULL;
			
			//Intializes an individual device
			void initialize();
			
			//Sets the associated step pin
			void setStepPin(int8_t pin);
			
			//Sets the associated direction pin
			void setDirPin(int8_t pin);		
			
			void setController(MIDI_Device_Controller *controller);
			void setID(uint8_t value);
			
		public:
			//Print out this device's configuration
			void printStatus();
			
			//Setup by specifying settings manually
			void setup(int8_t stepPin, int8_t dirPin, int32_t maxPosition);
			
			uint8_t getID();
			int8_t getStepPin();
			int8_t getDirPin();
			int16_t getMaxPosition();
			bool getEnabled(); 
			
			//Indicates whether a device is available for note assignment
			bool isAvailable();
			
			//Indicates whether a device is at or beyond it's max position    	
			bool isAtMaxPosition();
			
			//Indicates whether a device is tracking and changing direction
			bool isTrackingPosition(); 

			void setMaxPosition(int32_t value);
			void setEnabled(bool value);
			
		//Operation
		//_____________________________________________________________________________________________
		private:
			// Calculated microperiods based on the set resolution for each note
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
		
			//Sets the state of the associated step pin
			void setStepState(bool state); 
			
			//Sets the state of the associated direction pin
			void setDirState(bool state);

			//Toggles the step pin associated with the given channel
			void toggleStep();
			
			//Toggles the direction pin associated with the given channel
			void toggleDirection();
		
		public:
			bool getStepState();
			bool getDirState();
			int16_t getPosition();
			int8_t getCurrentNote();
			int16_t getBasePeriod();
			int16_t getCurrentPeriod();
			uint32_t getDuration();
			
			bool isEnabled();
			
			void assignNote(uint8_t note);		
			void assignPeriod(uint16_t period);
			
			void pitchBend(uint16_t bend);
			
			void clearNote();
			
			//Used to set the state of the direction pin associated with a given device
			//Respects position and updates it accordingly
			//TODO: rename to updateDirection
			void setDirection(bool direction);
			
			void resetPosition();
			
			//Used to reset devices to their 0 position
			void calibratePosition();
			
			//Sets the given device's position to 0
			void zeroPosition();

			//Resets a device's operational properties
			void resetProperties(bool includePosition = false);
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public: 
			//Tests the stepping capability of the given device
			void testStepping(uint32_t steps);

			void testStepping(int32_t steps, bool direction);
			
			//Steps a given device directly 50 times
			void testDeviceDirect();
			
			//Operates a given device to it's max position and back
			void testMaxDirection();

			//Runs all above tests consecutively
			void runAllTests();
	};
#endif