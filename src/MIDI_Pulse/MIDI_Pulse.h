#ifndef MIDI_Pulse_h
	#define MIDI_Pulse_h
	
	#include "Base_MIDI_Pulse.h"
	#include "../Common/NoteDuration.h"

	//Forward declaration for compiling
	// class MIDI_Device_Controller;

	///MIDI device class for anything needing a pulse signal (Solenoids, relays, servos (to be implemented), etc.)
	class MIDI_Pulse : public Base_MIDI_Pulse
	{	
		//Give MIDI_Device_Controller access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructors
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			MIDI_Pulse();
			~MIDI_Pulse();
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Print out this device's configuration
			//void printStatus();
			
			//Indicates whether a device is available for note assignment
			//inline bool isAvailable() { return true; };
			
			inline void setMaxDuration(uint32_t us)
			{ 
				_maxDuration.reset(); 
				_maxDuration.addMicros(us); 
			};

		//Operation
		//_____________________________________________________________________________________________
		private:
			//How long the device has been playing/on
			NoteDuration _currentDuration;
			
			//How long the device can be playing/on
			NoteDuration _maxDuration = NoteDuration(0, 50, 0);
			
			//Operates device per desired MIDI output
			void processNotes();
			
			//Checks if device is past the max duration and stops playing the current note if so
			void checkMaxDuration();
			
		public:			
			void pulse();
			void stopPulse();
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Tests turning the associated output on and off for the given duration in microseconds via direct IO manipulation
			void testOutputDirect(uint32_t duration = 250);
	};
#endif