#ifndef MIDI_Servo_h
	#define MIDI_Servo_h
	
	#include "Base_MIDI_SoftPWM.h"
	#include "../Common/NoteDuration.h"

	///MIDI device class for anything needing a pulse signal (Solenoids, relays, servos, etc.)
	class MIDI_Servo : public Base_MIDI_SoftPWM
	{	
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructors
		//_____________________________________________________________________________________________
		public:
			using Base_MIDI_SoftPWM::Base_MIDI_SoftPWM; //Inherit constructors
			
			inline MIDI_Servo(uint8_t numPositions = 2, uint16_t minLength = 1000, uint16_t maxLength = 2000, uint16_t maxAngle = 180)
			{
				setup(numPositions, minLength, maxLength, maxAngle);
			};
			
			inline MIDI_Servo(IOType type, int8_t outNum, uint8_t numPositions = 2, uint16_t minLength = 1000, uint16_t maxLength = 2000, uint16_t maxAngle = 180) 
			: Base_MIDI_SoftPWM(type, outNum)
			{
				setup(numPositions, minLength, maxLength, maxAngle);
			};
			
		private:
			inline void setup(uint8_t numPositions = 2, uint16_t minLength = 1000, uint16_t maxLength = 2000, uint16_t maxAngle = 180)
			{
				if(numPositions == 0)
					numPositions = 1;
				
				_maxPositions = numPositions;
				_positions = new uint16_t[_maxPositions];
				
				//For most servos, apparently the expected range is 1000 to 2000
				_minLength = minLength;
				_maxLength = maxLength;
				_maxAngle = maxAngle;
				
				//Default positions to even increments across the servo's range
				if(_maxPositions == 1)
				{
					setPositionByAngle(0, (_maxAngle / 2));
				}
				else
				{
					int16_t increment = _maxAngle / (_maxPositions - 1);
					for(int x = 0; x < _maxPositions; x++)
						setPositionByAngle(x, (x * increment));
				}
			};
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			
		public:
			//Print out this device's configuration
			// void printStatus();
			
			//Defines the lowest pulse length the servo accepts
			//inline void setMinLength(uint16_t length) { _minLength = length; };
			
			//Defines the highest pulse length the servo accepts
			//inline void setMaxLength(uint16_t length) { _maxLength = length; };
			
			//Defines the max angle the servo accepts
			//inline void setMaxAngle(uint16_t angle) { maxAngle = maxAngle; };
			
			//Sets the pulse length (servo position) for the given position (zero indexed)
			inline void setPosition(uint8_t index, uint16_t length) 
			{
				if(index >= _maxPositions)
				{
					_debug.println(F("%d - Can't set position at index %d; max is %d"), _id, index, _maxPositions);
					return;
				}
					
				
				_positions[index] = length;
			};
		
			inline void setPositionByAngle(uint8_t index, uint16_t angle)
			{	
				if(index >= _maxPositions)
				{
					_debug.println(F("%d - Can't set position at index %d; max is %d"), _id, index, _maxPositions);
					return;
				}
				
				if(angle > _maxAngle)
				{
					_debug.println(F("%d - Can't set angle %d; max is %d"), _id, angle, _maxAngle);
					angle = _maxAngle;
				}
					
				
				uint16_t length = map(angle, 0, _maxAngle, _minLength, _maxLength);
				_positions[index] = length;
			}
		
		//Operation
		//_____________________________________________________________________________________________
		private:
			//Current position index 
			uint8_t _currentPosition = 0;
			
			//Max amount of positions to toggle between on note pulse
			uint8_t _maxPositions = 2;
			
			//Positions to toggle between
			uint16_t* _positions;
			
			//Lower limit of pulse length (lowest position of the servo)
			uint16_t _minLength;
			
			//Upper limit of pulse length (highest position of the servo)
			uint16_t _maxLength;
			
			uint16_t _maxAngle;
			
			//Resets a device's operational properties
			// void resetProperties();
			
		public:
			inline void setLengthOnPulse()
			{
				if(_currentPosition >= _maxPositions)
					_currentPosition = 0;
				_currentLength = _positions[_currentPosition++];
			};
			
		//Testing/debug
		//_____________________________________________________________________________________________
		private:
			//Nothing here
			
		public:
			inline void testMinMax(uint32_t min = 1000, uint32_t max = 2000, uint32_t period = 20000)
			{
				if(!_outIO)
				{
					_debug.println(F("%d - Pulse output not setup"), _id);
					return;
				}
				
				testDirect(min, period, 10);
				delay(1000);
				testDirect(max, period, 10);
				
				// uint8_t numPulses = 1;
				// for(uint32_t x = min; x <= max; x++)
				// {
					// for(uint8_t y = 0; y < numPulses; y++)
					// {
						// _outIO->setOutput(_outNum, HIGH);
						// _outIO->updateOutputs();
						// delayMicroseconds(x);
						// _outIO->setOutput(_outNum, LOW);
						// _outIO->updateOutputs();
						// delayMicroseconds(period - x);
					// }
				// }
			};
	};
	
#endif