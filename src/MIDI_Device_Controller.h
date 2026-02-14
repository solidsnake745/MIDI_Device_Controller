#ifndef MIDI_Device_Controller_h
	#define MIDI_Device_Controller_h

	#include <Arduino.h>
	#include "Settings.h"
	#include "Common/SerialDebug.h" //Also includes <Arduino.h> in case it's used separately
	#include "MDC_Extras.h"
	#include "Common/MIDI_Periods.h"
	#include "MIDI_Pitch/Base_MIDI_Pitch.h"
	#include "MIDI_Pulse/Base_MIDI_Pulse.h"	
	
	#include "IO_Factory/IO_Factory.h"
	#include "IO_Devices/IO_Device.h"

	#include "MIDI_Device_Controller/ITimer/ITimer.h"
	//Resolve timer interrupt implementation
	#if ARDUINO_ARCH_AVR || defined(CORE_TEENSY)
		#include "MIDI_Device_Controller/ITimer/TimerOne_Timer.h"
	#elif ARDUINO_ARCH_ESP32
		#include "MIDI_Device_Controller/ITimer/ESP32_Timer.h"
	#endif

	//0 - Off
	//1 - Make methods public
	//2 - Tracks and prints execution time
	#define ISR_TESTING 0
	
	//Forward declaration for compiling
	class MIDI_Pulse_Controller;

	///[MDC] Controls and manages various MIDI device objects
	class MIDI_Device_Controller
	{
		// Give Device access to all private members
		friend class Base_MIDI_Pitch;
		friend class Base_MIDI_Pulse;	
		friend class MIDI_Pulse_Controller;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_DEVICECONTROLLER);
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			MIDI_Device_Controller();
			inline static MIDI_Device_Controller* _instance = nullptr;;
			
		public:
			//Used to populate our single instance MDC for consumption
			/// @private
			static MIDI_Device_Controller& getInstance();
		
		//Device management/operation
		//_______________________________________________________________________________________________________
		private:
			inline static Base_MIDI_Pitch* _pitchDevices[MAX_PITCH_DEVICES];
			inline static Base_MIDI_Pitch* _enabledPitchDevices[MAX_PITCH_DEVICES];
			inline static uint8_t _numEnabled = 0;
			inline static Base_MIDI_Pulse* _pulseDevices[MAX_PULSE_DEVICES];			

	#if ISR_TESTING
		//Make reloadEnabledDevices public so we can call it for testing
		public:
	#else
		private:
	#endif
			uint8_t reloadEnabledDevices();
			
		public:
			///Prints status information about this controller to Serial
			void printStatus();
			
			///Adds a MIDI_Pitch device to the controller
			/*
				@param index Index to assign the device to
				@param d Device to add
			*/
			bool addPitchDevice(uint8_t index, Base_MIDI_Pitch* d);
			
			///Adds a MIDI_Pitch device to the controller at the first available index
			/*!
				@param d Device to add
			*/
			int8_t addPitchDevice(Base_MIDI_Pitch* d);
			
			///Adds a Base_MIDI_Pulse device to the controller
			/*!
				\param index Index to assign the device to
				\param d Device to add
			*/
			bool addPulseDevice(uint8_t index, Base_MIDI_Pulse* d);
			
			///Adds a Base_MIDI_Pulse device to the controller at the first available index
			/*!				
				\param d Device to add
			*/
			int8_t addPulseDevice(Base_MIDI_Pulse* d);
			
			///Retrieves a MIDI_Pitch device from the controller
			/*!
				\param index Index to retrieve the device from
			*/
			Base_MIDI_Pitch* getPitchDevice(uint8_t index);
			
			///Retrieves a Base_MIDI_Pulse device from the controller
			/*!
				\param index Index to retrieve the device from
			*/
			Base_MIDI_Pulse* getPulseDevice(uint8_t index);
			
			//A note about deleting devices: I know I haven't thoroughly vetted and built out this functionality and it's very likely that it will lead to memory leaks
			//But I'm not really worried about it because ideally users shouldn't be changing configuration at all past setup() so the delete functionality is pretty optional
			//Really I should take it out completely and just document to make sure they're setup correctly at the start
			//Not at all sure the use case of being able to delete and re-create devices for this library
			
			//Really not supporting delete at this time as it's really unnecessary
			//Commenting them out so it doesn't seem like you should
			///Deletes a MIDI_Pitch device from the controller
			/*!
				\param index Index to try deleting the device from
			*/
			// void deletePitchDevice(uint8_t index);
			
			///Deletes a Base_MIDI_Pulse device from the controller
			/*!
				\param index Index to try deleting the device from
			*/
			// void deletePulseDevice(uint8_t index);
			
			void resetDevicePositions();
			void calibrateDevicePositions();
		
			//Opting to just have users call these methods directly from the device rather than have a bunch of specific methods
			//Usually they'll just be using collections or the pulse controller anyway
			// void playDeviceNote(uint8_t index, uint8_t note);
			// void bendDeviceNote(uint8_t index, int16_t bend, bool shiftRange = false);
			// void stopDeviceNote(uint8_t index, uint8_t note);
			
		//Note Processing
		//_______________________________________________________________________________________________________
		private:
			bool _isPlayingNotes = false;
			bool _autoPlayNotes = true;

			//Resolve timer interrupt implementation
			#if ARDUINO_ARCH_AVR || defined(CORE_TEENSY)
				ITimer* _timer = new TimerOne_Timer();
				
				//Named on a whim, it's inconsequential and not visible to end users
				//Static method for interrupt to attach to
				inline static void lawl() { _instance->processNotes(); };
			#elif ARDUINO_ARCH_ESP32
				ITimer* _timer = new ESP32_Timer();
				static void lawl();
			#endif
		
			void noteAssigned();
		
	#if ISR_TESTING
		//Make processNotes public so we can call it for testing
		public:
	#else
		private:
	#endif
			//Operates devices during interrupt process
			void processNotes();
			
		public:
			//Starts the interrupt process to play notes
			bool startPlaying();
			
			//Stops the interrupt process to play notes
			void stopPlaying();
			
			//Processes reset for auto processing (needs to be called in your loop!)
			bool process();
			
			//Indicates whether note processing is on/off
			inline bool isPlayingNotes() { return _isPlayingNotes; };

		//Settings
		//_______________________________________________________________________________________________________
		private:
			uint32_t _lastAssign = 0;
			uint16_t _idleTimeout = IDLE_TIMEOUT_DEFAULT;

		public:
			//Indicates whether note processing is automatically started on note assignment
			inline bool isAutoPlayEnabled() { return _autoPlayNotes; };
			
			inline uint8_t getMaxPitchDevices() { return MAX_PITCH_DEVICES; };
			inline void setResolution(uint16_t resolution = DEFAULT_RESOLUTION) { MIDI_Periods::setResolution(resolution); };
			
			///@private
			inline void setDebugResolution() { MIDI_Periods::setDebugResolution(); };
			
			///Sets the timeout period in milliseconds
			/*!
				When idle, all processing and outputs are turned off.
				\param value Number of milliseconds
			*/
			inline void setIdleTimeout(int16_t value = IDLE_TIMEOUT_DEFAULT) { _idleTimeout = value; };		
			inline void setAutoPlay(bool value) { _autoPlayNotes = value; };

		//LED pin functionality
		//_______________________________________________________________________________________________________
		private: 
			int8_t _ledPin = -1;
			
			//Counter for MIDI clock events
			//Every 24 events is a quarter note
			int8_t _clockCount = 0;
			
		public: 
			inline void setLEDPin(int8_t pin = LED_BUILTIN)
			{
				_ledPin = pin;
				if(_ledPin > -1)
					pinMode(_ledPin, OUTPUT);
			};
			
			inline void LEDOn() { if(_ledPin > -1) digitalWrite(_ledPin, HIGH); };			
			inline void LEDOff() { if(_ledPin > -1) digitalWrite(_ledPin, LOW); };
			inline void toggleLED() { if(_ledPin > -1) digitalWrite(_ledPin, !digitalRead(_ledPin)); };
			
			inline void midiClockEvent()
			{
				if(!_isPlayingNotes)
					return;
				
				_clockCount++;
				if(_clockCount != 24) //Every quarter note
					return;
				
				toggleLED();
				_clockCount = 0;
			};
			
		//Tests/Debug 
		//_______________________________________________________________________________________________________
		//If you make any changes to this library, a good way to ensure
		//it still works is by running all of these test functions
		public:
			//Plays a test tone on a given device via our interrupt process a few times
			void testPitchDeviceInterrupt(uint8_t index);
			
			//Pitch bend test
			void testPitchBend(uint8_t index);
			
			//Basic load test to ensure we're able to play 
			void loadTest(uint8_t numDevices = MAX_PITCH_DEVICES);
			
			//Plays a sequence across all devices to test set configuration
			void playStartupSequence(uint8_t version = 0);
	};

	//Defines a global singleton instance of our class for users to consume
	inline MIDI_Device_Controller MDC = MIDI_Device_Controller::getInstance();
#endif
