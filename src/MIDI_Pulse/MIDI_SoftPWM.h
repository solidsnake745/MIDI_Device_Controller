#ifndef MIDI_SoftPWM_h
	#define MIDI_SoftPWM_h
	
	#include "Base_MIDI_SoftPWM.h"
	#include "../Common/NoteDuration.h"
	
	//Forward declaration for compiling
	// class MIDI_Device_Controller;

	///MIDI device class for anything needing a PWM signal
	class MIDI_SoftPWM : public Base_MIDI_SoftPWM
	{	
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructors
		//_____________________________________________________________________________________________
		public:
			using Base_MIDI_SoftPWM::Base_MIDI_SoftPWM; //Inherit constructors
			
		//Configuration
		//_____________________________________________________________________________________________
		private:
			uint16_t _lengthOnPulse = 0;
			
		public:
			inline void setLengthOnPulse(uint16_t length)
			{
				//TODO: Add validation
				_lengthOnPulse = length;
			};
			
			inline void setDutyCycle(uint8_t percentage)
			{
				if(percentage > 100)
					percentage = 100;
				
				//If percentage is 100, this class basically behaves like MIDI_Pulse that consumes more processing power
				//Unless you're changing duty cycle in other calls, you're better off using switching to that device
				
				//Approximate pulse length based on the total period
				//float pulseLength = ((float) percentage / 100.0) * _currentPeriod;
				//_lengthOnPulse = (uint16_t) pulseLength;
				
				//Arduino map is perfect for this!
				_lengthOnPulse = map(percentage, 0, 100, 0, _currentPeriod);
			};
			
		//Operation
		//_____________________________________________________________________________________________
		protected:
			
		public:			
			inline void setLengthOnPulse() { _currentLength = _lengthOnPulse; };
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:

	};
	
#endif