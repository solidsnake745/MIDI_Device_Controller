#ifndef MIDI_PWM_h
	#define MIDI_PWM_h
	
	#include "Base_MIDI_Pulse.h"
	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	#include "../Common/NoteDuration.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"

	//Forward declaration for compiling
	class MIDI_Device_Controller;

	//enum PulseMode { Normal, Toggle, PWM };

	///MIDI device class for anything needing a pulse signal (Solenoids, relays, servos (to be implemented), etc.)
	class MIDI_PWM //: public Base_MIDI_Pulse
	{	
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructors
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			// MIDI_Pulse();
			// ~MIDI_Pulse();
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			//Designated output mapping where -1 indicates no out is assigned
			// int8_t _outNum = -1;
			// IO_Device* _outIO = nullptr;
			
			//PulseMode _mode = Normal;
			
			// Max number of steps the device can take before needing to switch directions where -1 indicates it does not have a max
			// NOTE: Setting to -1 disables position tracking functionality all together
			// int16_t _maxPosition = -1;
			
						
			
		public:
			//Print out this device's configuration
			// void printStatus();
			
			// inline uint8_t getID() { return _id; };
			// inline int8_t getOutputNum() { return _outNum; };
			
			//Indicates whether a device is available for note assignment
			//inline bool isAvailable() { return true; };

			//Sets the associated output
			// void setOutput(IOType type, int8_t num);
			
			// inline void setMaxDuration(uint32_t us)
			// { 
				// _maxDuration.reset(); 
				// _maxDuration.addMicros(us); 
			// };
			
			//Sets the operation mode
			//void setMode(PulseMode mode);
			//void setPWM(uint16_t length, uint16_t period);
			
		//Operation
		//_____________________________________________________________________________________________
		private:
			//Current tick value
			//volatile uint16_t _currentTick = 0;

			//How long the current device has been playing
			// NoteDuration _currentDuration;
			
			// NoteDuration _maxDuration = NoteDuration(0, 10, 0);
			
			//Used to set the max length of the pulse in microseconds; can also be considered max duration
			// volatile uint32_t _pulseLength
			
			//Sets the state of the associated output
			// void setOutputState(bool state);
		
			//Resets a device's operational properties
			// void resetProperties();
			
			//Operates device per desired MIDI output
			// void processNotes();
			
			//Checks if deivce is past the max duration and stops playing the current note if so
			// void checkMaxDuration();
			
		public:
			// bool getOutputState();
			// void pulse();
			// void stopPulse();
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Tests turning the associated output on and off for the given duration in microseconds via directio IO manipulation
			// void testOutputDirect(uint32_t duration = 250);	
			
			//Tests generating a number of PWM pulses with the given length and period in microseconds via direct IO manipulation
			// void testPwmPulseDirect(uint32_t length = 1000, uint32_t period = 20000, uint8_t numPulses = 5);
			
			//Tests generating PWM pulses with the given length and period in microseconds via interrupt process
			// void testPwmPulse(uint32_t length = 1000, uint32_t period = 20000);
	};
#endif