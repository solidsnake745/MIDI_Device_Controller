#ifndef MIDI_Pulse_h
	#define MIDI_Pulse_h
	
	#include "Base_MIDI_Pulse.h"
	#include "../Common/NoteDuration.h"

	///MIDI device class for anything needing a pulse signal (Solenoids, relays, servos (to be implemented), etc.)
	class MIDI_Pulse : public Base_MIDI_Pulse
	{	
		//Give MIDI_Device_Controller access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructors
		//_____________________________________________________________________________________________
		public:
			using Base_MIDI_Pulse::Base_MIDI_Pulse; //Inherit constructors
			
		//Configuration
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
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
			inline void processNotes()
			{
				if(!_outIO)
				{
					_debug.debugln(ISR, F("%d - Pulse output not setup"), _id);
					return;
				}
				
				if(_outIO->getOutput(_outNum))
					_currentDuration.addMicros(MIDI_Periods::getResolution());
				
				checkMaxDuration();
			};
			
			//Checks if device is past the max duration and stops playing the current note if so
			inline void checkMaxDuration()
			{
				if(_maxDuration.isZero())
					return;
				
				if(_maxDuration <= _currentDuration)
					stopPulse();
			};
			
		public:			
			inline void pulse()
			{
				if(!_outIO)
				{
					_debug.println(F("%d - Pulse output not setup"), _id);
					return;
				}
				
				_outIO->setOutput(_outNum, HIGH);
				noteAssigned();
			};
			
			inline void stopPulse()
			{
				if(!_outIO)
				{
					_debug.println(F("%d - Pulse output not setup"), _id);
					return;
				}
				
				_outIO->setOutput(_outNum, LOW);
				_currentDuration.reset();
			};
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Tests turning the associated output on and off for the given duration in microseconds via direct IO manipulation
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