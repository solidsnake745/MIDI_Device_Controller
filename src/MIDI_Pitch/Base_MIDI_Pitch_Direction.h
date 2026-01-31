#ifndef Base_MIDI_Pitch_Direction_h
	#define Base_MIDI_Pitch_Direction_h

	#include "Base_MIDI_Pitch.h"
	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"

	///Component class to Base_MIDI_Pitch that represents position tracking
	class Base_MIDI_Pitch_Direction
	{
		friend class MIDI_Device_Controller;
		
		protected:
			SerialDebug* _parentDebug = nullptr;
		
		//Constructors
		//_____________________________________________________________________________________________
		protected:
			//Base_MIDI_Pitch_Direction();
		
		//Configuration
		//_____________________________________________________________________________________________
		protected:
			Base_MIDI_Pitch* _base = nullptr;
			
			inline void setBase(Base_MIDI_Pitch* base) 
			{ 
				_base = base;
				_parentDebug = &(_base->_debug);
			};
			
			//Max number of steps the device can take before needing to switch directions where -1 indicates it does not have a max
			//NOTE: Setting to -1 disables position tracking functionality all together
			int16_t _maxPosition = -1;
			
		public:
			inline int16_t getMaxPosition() { return _maxPosition; };
			inline void setMaxPosition(int16_t value) { _maxPosition = value; };
			
		//Operation
		//_____________________________________________________________________________________________
		protected:
			//Current position value
			volatile int16_t _currentPosition = 0;
			
			virtual bool getDirState() = 0;
			virtual void setDirState(bool direction) = 0;
			
		public:
			//Method that defines how the device can step/advance one position
			//stepOnce should operate motor for one increment and should not increment _currentPosition
			virtual void stepOnce(bool includeDelay = false) = 0;
			
			//Indicates whether a device is tracking and changing direction
			inline bool isTrackingPosition() { return _maxPosition > 0; };
			
			//Indicates whether a device is at or beyond it's max position    	
			inline bool isAtMaxPosition() { return _currentPosition >= _maxPosition; };
			
			//Used to set the state of the direction pin associated with a given device
			//Respects position and updates it accordingly
			inline void setDirection(bool direction)
			{
				//Check user is actually changing the direction from the current state
				if(getDirState() == direction)
				{
					_parentDebug->debugln(7, F("%d - Direction is already: %d"), _base->_id, direction);
					return;
				}
				
				//Set the desired state for the given device
				setDirState(direction);
				_parentDebug->debugln(7, F("%d - New direction: %d"), _base->_id, direction);

				//Update it's current position if device is tracking it
				if(isTrackingPosition())
				{
					_parentDebug->debugln(7, F("%d - Updating position from: %d"), _base->_id, _currentPosition);
					_currentPosition = (_maxPosition - _currentPosition);
					_parentDebug->debugln(7, F("%d - New position: %d"), _base->_id, _currentPosition);
				}
			};
			
			inline void resetPosition()
			{
				//Don't attempt resetting if the device isn't set to track position
				if(!isTrackingPosition())
					return;
				
				setDirection(HIGH);
				
				while(!isAtMaxPosition())
				{
				  stepOnce(true);
				  _currentPosition++;
				  delayMicroseconds(MANUAL_CHANGE_DELAY);
				}
				
				setDirection(LOW);
			};
			
			//Used to reset devices to their 0 position
			inline void calibratePosition()
			{
				setDirection(LOW);
				_currentPosition = getMaxPosition();
				resetPosition();
			};
			
			//Sets the given device's position to 0
			inline void zeroPosition() { _currentPosition = 0; };
			
		//Testing/debug
		//_____________________________________________________________________________________________
		public:
			//Operates a given device to it's max position and back
			virtual void testMaxDirection() = 0;
	};
	
#endif