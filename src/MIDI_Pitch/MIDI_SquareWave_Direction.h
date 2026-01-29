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
			inline MIDI_SquareWave_Direction(
				IOType type1, int8_t outNum1, 
				IOType type2 = NoValue, int8_t outNum2 = -1, 
				int16_t maxPosition = -1) 
					: MIDI_SquareWave(type1, outNum1) 
			{
				setBase(this);				
				setDirOutput(type2, outNum2);
				setMaxPosition(maxPosition);
			};
		
		//Configuration
		//_____________________________________________________________________________________________
		public:
			//Print out this device's configuration
			inline void printStatus()
			{
				MIDI_SquareWave::printStatus();
				_debug.println(F("  Dir Output Number: %d"), getDirOutputNum());
				_debug.println(F("  Dir Output State: %d"), getDirOutputState());
			};
			
			inline bool hasDirection() { return true; };
			inline Base_MIDI_Pitch_Direction* asDir() { return static_cast<Base_MIDI_Pitch_Direction*>(this); };
			
		//Operation
		//_____________________________________________________________________________________________
		private:
			inline void stepOnce(bool includeDelay = false)
			{
				//Initial state should be off, set it if not
				if(_outIO->getOutput(_outNum))
				{
					_outIO->setOutput(_outNum, LOW);
					delayMicroseconds(50);
				}
				
				_outIO->toggleOutput(_outNum); //Toggle on (step)
				_outIO->updateOutputs();
				
				if(includeDelay)
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				
				_outIO->toggleOutput(_outNum); //Toggle back off
				_outIO->updateOutputs();
			};
		
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
						_debug.debugln(20, F("%d - Toggling direction"), _id);
						toggleDirection();
						zeroPosition();
					}
				}
			};
			
		public:
			
		//Testing/debug
		//_____________________________________________________________________________________________
		public:
			//Test via the interrupt process
			void testInterrupt();

			//Test via direct IO manipulation
			void testDirect();
			
			//Test that operates the device to it's max position and then back to start
			void testMaxDirection();

			//Tests stepping via the interrupt process
			void testStepInterrupt(uint32_t steps = 10, bool direction = false);

			//Tests stepping via direct IO manipulation
			void testStepDirect(uint32_t steps = 10, bool direction = false);
	};
	
#endif