#ifndef MIDI_Toggle_h
	#define MIDI_Toggle_h
	
	#include "Base_MIDI_Pulse.h"

	//Forward declaration for compiling
	class MIDI_Device_Controller;

	///MIDI device class for anything needing a pulse signal (Solenoids, relays, servos (to be implemented), etc.)
	class MIDI_Toggle : public Base_MIDI_Pulse
	{	
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructors
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			MIDI_Toggle();
			~MIDI_Toggle();
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Print out this device's configuration
			//void printStatus();
			
			//Indicates whether a device is available for note assignment
			//inline bool isAvailable() { return true; };

		//Operation
		//_____________________________________________________________________________________________
		private:
			//Operates device per desired MIDI output
			void processNotes();
			
		public:
			void pulse();
			void stopPulse();
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Tests turning the associated output on and off for the given duration in microseconds via directio IO manipulation
			void testOutputDirect(uint32_t duration = 250);
	};
#endif