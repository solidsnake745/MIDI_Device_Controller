#ifndef Base_MIDI_SoftPWM_h
	#define Base_MIDI_SoftPWM_h
	
	#include "Base_MIDI_Pulse.h"
	#include "../Common/NoteDuration.h"
	// #include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

	//Forward declaration for compiling
	class MIDI_Device_Controller;

	///Base device class for anything needing a PWM signal
	class Base_MIDI_SoftPWM : public Base_MIDI_Pulse
	{	
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructors
		//_____________________________________________________________________________________________
		private:
			//Nothing here
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			
		public:
			//Print out this device's configuration
			// void printStatus();
			
			inline void setMaxDuration(uint32_t us)
			{ 
				_maxDuration.reset();
				_maxDuration.addMicros(us);
			};
		
		//Operation
		//_____________________________________________________________________________________________
		protected:
			enum PWMState
			{ 
				Starting, //Flag to start the PWM pulse
				Running, //Specified pulse is being generated
				Stopping, //Flag to stop the PWM pulse
				Stopped //Pulse is stopped
			};
		
			//Current state of generating PWM pulses
			PWMState _currentPWMState = Stopped;
			
			//Current tick value - represents how long the current state has been active in microseconds
			uint16_t _currentTick = 0;
			
			//Current value of how long a pulse should be on in microseconds
			uint16_t _currentLength = 1000;
			
			//Current total period of a PWN pulse
			uint16_t _currentPeriod = 20000;
			
			//Current state of the PWM pulse output (HIGH/LOW)
			bool _currentState = 0;
			
			//How long the current device has been playing
			NoteDuration _currentDuration;
			
			//Max duration the output should be pulsed
			NoteDuration _maxDuration = NoteDuration(0, 0, 10);
			
			//Resets a device's operational properties
			// void resetProperties();
			
			//Operates device per desired MIDI output
			inline void processNotes()
			{
				if(_currentPWMState == Stopped)
					return;
				
				if(!_outIO)
				{
					_debug.debugln(50, F("%d - Pulse output not setup"), _id);
					return;
				}
				
				//We want the interrupt to handle starting the pulse so that we can accurately track the duration of the signal
				if(_currentPWMState == Starting)
				{
					_currentPWMState = Running;
					_currentState = HIGH; //HIGH == true (1)
					_outIO->setOutput(_outNum, _currentState);
					return;
				}
				
				//Wait until the current pulse is finished and then stop
				if(_currentPWMState == Stopping && !_currentState)
				{
					_currentPWMState = Stopped;
					_currentTick = 0;
					_currentLength = 0;
					_currentDuration.reset();
					return;
				}
				
				_currentTick += MIDI_Periods::getResolution();
				_currentDuration.addMicros(MIDI_Periods::getResolution());
				
				if(isPastMaxDuration())
					stopPulse();
				
				if(_currentState && _currentTick < _currentLength)
					return;
				
				if(!_currentState && _currentTick < (_currentPeriod - _currentLength))
					return;
				
				_currentTick = 0;
				_currentState = !_currentState;
				_outIO->setOutput(_outNum, _currentState);
			};
			
			//Checks if device is past the max duration
			inline bool isPastMaxDuration()
			{
				if(_maxDuration.isZero())
					return false;
				
				if(_maxDuration <= _currentDuration)
					return true;
				
				return false;
			};
			
			//Overridable method for derived classes to define how the PWM length/duty cycle is assigned
			virtual void setLengthOnPulse() {};
			
		public:
			//Indicates whether a device is available for note assignment
			inline bool isAvailable() { return _currentLength == 0; };
			
			void pulse();
			
			inline void stopPulse()
			{
				if(!_outIO)
				{
					_debug.debugln(7, F("%d - Pulse output not setup"), _id);
					return;
				}
				
				// _currentLength = 0;
				// _currentTick = 0;
				// _currentState = LOW;
				// _outIO->setOutput(_outNum, _currentState);
				// _currentDuration.reset();
				
				if(_currentPWMState != Stopped || _currentPWMState != Stopping)
					_currentPWMState = Stopping;
			};
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			//Tests generating a number of PWM pulses with the given length and period in microseconds via direct IO manipulation
			inline void testDirect(uint32_t length = 1000, uint32_t period = 20000, uint8_t numPulses = 5)
			{
				if(!_outIO)
				{
					_debug.debugln(7, F("%d - Pulse output not setup"), _id);
					return;
				}
				
				if(numPulses == 0)
					numPulses = 1;
				
				for(uint8_t x = 0; x < numPulses; x++)
				{
					_outIO->setOutput(_outNum, HIGH);
					_outIO->updateOutputs();
					delayMicroseconds(length);
					_outIO->setOutput(_outNum, LOW);
					_outIO->updateOutputs();
					delayMicroseconds(period - length);
				}
			};
			
			//Tests generating PWM pulses with the given length and period in microseconds via interrupt process for the specified duration in milliseconds
			void testInterrupt(uint32_t length = 1000, uint32_t period = 20000, uint16_t duration = 500);
	};
	
#endif