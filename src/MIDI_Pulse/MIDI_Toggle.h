#ifndef MIDI_Toggle_h
	#define MIDI_Toggle_h
	
	#include "Base_MIDI_Pulse.h"

	///MIDI device class for anything needing a pulse signal (Solenoids, relays, servos (to be implemented), etc.)
	class MIDI_Toggle : public Base_MIDI_Pulse
	{	
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructors
		//_____________________________________________________________________________________________
		public:
			using Base_MIDI_Pulse::Base_MIDI_Pulse; //Inherit constructors
			
		//Configuration
		//_____________________________________________________________________________________________		
		public:
			//Nothing here
			
		//Operation
		//_____________________________________________________________________________________________
		private:
			//Operates device per desired MIDI output
			inline void processNotes() {}; //Nothing to do here
			
		public:
			inline void pulse() 
			{
				if(!_outIO)
				{
					_debug.println(F("%d - Pulse output not setup"), _id);
					return;
				}
				
				_outIO->toggleOutput(_outNum);
				noteAssigned();
			};
			
			inline void stopPulse() {}; //Nothing to do here
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Tests turning the associated output on and off for the given duration in microseconds via direction IO manipulation
			inline void testOutputDirect(uint32_t duration = 250)
			{
				if(!_outIO)
				{
					_debug.println(F("%d - Pulse output not setup"), _id);
					return;
				}
				
				_outIO->setOutput(_outNum, HIGH);
				callUpdateOutputs(_outIO);
				delayMicroseconds(duration);
				_outIO->setOutput(_outNum, LOW);
				callUpdateOutputs(_outIO);
			};
	};
	
#endif