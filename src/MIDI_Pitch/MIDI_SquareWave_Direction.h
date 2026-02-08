#ifndef MIDI_SquareWave_Direction_h
	#define MIDI_SquareWave_Direction_h

	#include "MIDI_SquareWave.h"
	#include "Base_MIDI_Pitch_Direction.h"
	
	//Pitch device that generates a square wave on the specified output and tracks/manages position.
	//Used for devices like bipolar stepper motors, FDDs, etc.
	class MIDI_SquareWave_Direction : public MIDI_SquareWave, public Base_MIDI_Pitch_Direction
	{	
		//Constructors
		//_____________________________________________________________________________________________
		public:
			using MIDI_SquareWave::MIDI_SquareWave; //Inherit constructors
			
			inline MIDI_SquareWave_Direction(
				IOType type1, int8_t outNum1, 
				IOType type2 = NoValue, int8_t outNum2 = -1, 
				int16_t maxPosition = -1) : MIDI_SquareWave(type1, outNum1) 
			{
				setBase(this);				
				setDirOutput(type2, outNum2);
				setMaxPosition(maxPosition);
				resetProperties(true);
			};
		
		inline bool hasDirection() { return true; };
		inline Base_MIDI_Pitch_Direction* asDir() { return static_cast<Base_MIDI_Pitch_Direction*>(this); };
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			//Designated output mapping where -1 indicates nothing is assigned
			int8_t _dirOutNum = -1;
			IO_Device* _dirOutIO = nullptr;
			
		public:
			//Print out this device's configuration
			inline void printStatus()
			{
				MIDI_SquareWave::printStatus();
				_debug.println(F("  Dir Output Number: %d"), getDirOutputNum());
				_debug.println(F("  Dir Output State: %d"), getDirState());
				_debug.println(F("  Max Position: %d"), getMaxPosition());
			};
			
			//Sets the mapping to the output
			inline void setDirOutput(IOType type, int8_t outNum)
			{
				_dirOutNum = outNum;
				if (_dirOutNum < 0)
				{		
					_dirOutIO = nullptr;
					return;
				}
				
				_dirOutIO = IOF.getIO(type);
				if(!_dirOutIO || !_dirOutIO->isValidMapping(_dirOutNum))
					_dirOutNum = -1;
			};
			
			inline int8_t getDirOutputNum() { return _dirOutNum; };
			
		//Operation
		//_____________________________________________________________________________________________
		private:
			//Resets a device's operational properties
			inline void resetProperties(bool includeExtra = false)
			{
				Base_MIDI_Pitch::resetProperties(includeExtra);
				
				//Purely for visual effect when calling this during processing; in case there is an LED tied to it.
				setOutputState(LOW);
				
				if(!includeExtra) return;
				//Reset extra properties here
			};
			
			//Sets up anything necessary before note playing begins
			inline void startPlaying()
			{
				//Make sure outputs stops/don't stop when supposed to
				if(_outIO)
					_outIO->setShouldStop(_outNum, true);
				
				if(_dirOutIO)
					_dirOutIO->setShouldStop(_dirOutNum, false);
			};
			
			inline void cycleOutput() 
			{ 
				_outIO->toggleOutput(_outNum);

				//Increment position if valid
				if(isTrackingPosition() && getOutputState()) //Update position on the rising edge of the output
				{
					_currentPosition++;
					if(isAtMaxPosition()) //Direction update pending
					{
						_debug.debugln(ISR, F("%d - Toggling direction"), _id);
						toggleDirection();
						zeroPosition();
					}
				}
			};
			
			inline bool getDirState()
			{
				if(_dirOutNum < 0 || !_dirOutIO)
					return LOW; //Have to return something
				
				return _dirOutIO->getOutput(_dirOutNum);
			};
			
			inline void setDirState(bool state)
			{
				if(_dirOutNum < 0 || !_dirOutIO) return;
				_dirOutIO->setOutput(_dirOutNum, state);
			}
			
			//Toggle state of direction output
			inline void toggleDirection()
			{				
				if(_dirOutIO)
					_dirOutIO->toggleOutput(_dirOutNum);
			};
			
		public:
			inline void stepOnce(bool includeDelay = false)
			{
				//Initial state should be off, set it if not
				if(_outIO->getOutput(_outNum))
				{
					_outIO->setOutput(_outNum, LOW);
					delayMicroseconds(50);
				}
				
				_outIO->toggleOutput(_outNum); //Toggle on (step)
				callUpdateOutputs(_outIO);
				
				if(includeDelay)
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				
				_outIO->toggleOutput(_outNum); //Toggle back off
				callUpdateOutputs(_outIO);
			};
			
		//Testing/debug
		//_____________________________________________________________________________________________
		public:
			//Test via the interrupt process
			inline void testInterrupt()
			{
				//Nothing to test if no valid step output set
				if(!_outIO)
				{
					_debug.println(F("%d - Output not setup"), _id);
					return;
				}
				
				mdcStartPlaying();

				//Initial state should be off, set it if not
				if(_outIO->getOutput(_outNum))
				{
					_outIO->setOutput(_outNum, LOW);
					delayMicroseconds(50);
				}
				
				uint32_t t = millis();
				while(millis() - t < 3000)
				{		
					cycleOutput(); //Toggle on (step)
					delayMicroseconds(MANUAL_CHANGE_DELAY);
					cycleOutput(); //Toggle back off
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}
				
				mdcStopPlaying();
			};

			//Test via direct IO manipulation
			inline void testDirect()
			{
				//Nothing to test if no valid step output set
				if(!_outIO)
				{
					_debug.println(F("%d - Output not setup"), _id);
					return;
				}
				
				//Initial state should be off, set it if not
				if(_outIO->getOutput(_outNum))
				{
					_outIO->setOutput(_outNum, LOW);
					delayMicroseconds(50);
				}
				
				uint32_t t = millis();
				while(millis() - t < 3000)
				{		
					cycleOutput(); //Toggle on (step)
					callUpdateOutputs(_outIO);
					if(_dirOutIO && _dirOutIO != _outIO) callUpdateOutputs(_dirOutIO);
					delayMicroseconds(MANUAL_CHANGE_DELAY);
					
					cycleOutput(); //Toggle back off
					callUpdateOutputs(_outIO);
					if(_dirOutIO && _dirOutIO != _outIO) callUpdateOutputs(_dirOutIO);
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}
			};
			
			//Test that operates the device to it's max position and then back to start
			inline void testMaxDirection()
			{
				//Nothing to test if no valid step output set
				if(!_outIO)
				{
					_debug.println(F("%d - Output not setup"), _id);
					return;
				}
				
				//Nothing to test if no valid max position set
				if(!isTrackingPosition()) 
				{
					_debug.println(F("%d - Not tracking position"), _id);
					return;
				}
				
				_debug.debugln(TRACE, F("%d - Testing max direction part 1"), _id);
				mdcStartPlaying();
				while(!isAtMaxPosition()) 
				{
					_debug.debugln(TRACE, F("%d - Testing max direction part 1a"), _id);
					stepOnce(true);
					_currentPosition++;
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}

				_debug.debugln(TRACE, F("%d - Testing max direction part 2"), _id);
				toggleDirection();
				zeroPosition();

				delay(1000);
				
				_debug.debugln(TRACE, F("%d - Testing max direction part 3"), _id);
				while(!isAtMaxPosition()) 
				{
					stepOnce(true);
					_currentPosition++;
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}

				toggleDirection();
				zeroPosition();
				mdcStopPlaying();
			};

			//Tests stepping via the interrupt process
			inline void testStepInterrupt(uint32_t steps = 10, bool direction = false)
			{
				//Nothing to test if no valid step output set
				if(!_outIO)
				{
					_debug.println(F("%d - Output not setup"), _id);
					return;
				}
				
				setDirection(direction);
				mdcStartPlaying();

				//Initial state should be off, set it if not
				if(_outIO->getOutput(_outNum))
				{
					_outIO->setOutput(_outNum, LOW);
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}
				
				while(steps-- > 0)
				{		
					_outIO->toggleOutput(_outNum); //Toggle on (step)
					delayMicroseconds(MANUAL_CHANGE_DELAY);
					_outIO->toggleOutput(_outNum); //Toggle back off
					
					if(isTrackingPosition())
					{
						_currentPosition++;
						if(isAtMaxPosition())
						{
							toggleDirection();
							zeroPosition();
						}
					}
					
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}
				
				mdcStopPlaying();
			};

			//Tests stepping via direct IO manipulation
			inline void testStepDirect(uint32_t steps = 10, bool direction = false)
			{
				// Nothing to test if no valid step output set
				if(!_outIO)
				{
					_debug.println(F("%d - Output not setup"), _id);
					return;
				}
				
				setDirection(direction);
				while(steps-- > 0)
				{
					stepOnce(true);
					
					if(isTrackingPosition())
					{
						_currentPosition++;
						if(isAtMaxPosition())
						{
							toggleDirection();
							zeroPosition();
						}
					}
					
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}
			};
	};
	
#endif