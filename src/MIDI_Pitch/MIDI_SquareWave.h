#ifndef MIDI_SquareWave_h
	#define MIDI_SquareWave_h

	#include "Base_MIDI_Pitch.h"	

	//Pitch device that generates a square wave on the specified output. Used for devices like HDDs or piezo buzzers.
	class MIDI_SquareWave : public Base_MIDI_Pitch
	{	
		//Give access to all private members to the below classes
		// friend class MIDI_Device_Controller;
		// friend class MIDI_Pitch_Node;
		// friend class Base_MIDI_Pitch_Collection;
		
		//Constructors
		//_____________________________________________________________________________________________
		public:
			inline MIDI_SquareWave(IOType type, int8_t outNum) : Base_MIDI_Pitch() 
			{ 
				setOutput(type, outNum);
				//resetProperties(true);
			};
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			//Designated output mapping where -1 indicates nothing is assigned
			int8_t _outNum = -1;
			IO_Device* _outIO = nullptr;
		
		public:
			//Print out this device's configuration
			inline void printStatus()
			{
				Base_MIDI_Pitch::printStatus();
				_debug.println(F("  Output Number: %d"), getOutputNum());
				_debug.println(F("  Output State: %d"), getOutputState());
			};

			//Sets the mapping to the output
			inline void setOutput(IOType type, int8_t outNum) 
			{ 
				_outNum = outNum;
				if (_outNum < 0)
				{		
					_outIO = nullptr;
					return;
				}
				
				_outIO = IOF.getIO(type);
				if(!_outNum || !_outIO->isValidMapping(_outNum))
					_outNum = -1;
			}

			inline int8_t getOutputNum() { return _outNum; };
			
			inline void setOutputState(bool state)
			{
				if(_outNum < 0 || !_outIO) return;
				_outIO->setOutput(_outNum, state);
			}

			inline bool getOutputState() 
			{
				if(_outNum < 0 || !_outIO)
					return LOW; //Have to return something
				
				return _outIO->getOutput(_outNum);
			};
			
		//Operation
		//_____________________________________________________________________________________________
		private:
			//Resets a device's operational properties
			inline virtual void resetProperties(bool includeExtra = false)
			{
				Base_MIDI_Pitch::resetProperties(includeExtra);
				
				//Purely for visual effect when calling this during processing; in case there is an LED tied to it.
				setOutputState(LOW);
				
				//if(!includeExtra) return;
				//Reset extra properties here
			};
			
			//Gets the base period of the note currently being played
			inline virtual int32_t getBasePeriod() { return *(_referencePeriods + _currentNote); };
			
			//Sets up anything necessary before note playing begins
			inline void startPlaying()
			{
				//Make sure output stops when supposed to
				if(_outIO)
					_outIO->setShouldStop(_outNum, true);
			};
			
			//Operates device per desired MIDI output
			inline void processNotes()
			{
				if(_currentNote < 0)
				{
					_debug.debugln(20, F("%d - No note"), _id);
					return;
				}
				
				if(_currentNote == 0)
				{			
					_debug.debugln(20, F("%d - Resetting properties"), _id);
					resetProperties();
					return;
				}
					
				_currentTick++;
				if(_currentTick >= _currentPeriod)
				{
					_debug.debugln(20, F("%d - Toggling step"), _id);
					_outIO->toggleOutput(_outNum);
					_currentTick = 0;
				}
				
				_currentDuration.addMicros(MIDI_Periods::getResolution());
				
				if(_currentEffect == Vibrato)
					_vibratoTick += MIDI_Periods::getResolution();
			};
			
		public:
			inline bool isEnabled() { return _outNum >= 0; };
			
		//Testing/debug
		//_____________________________________________________________________________________________
		public:
			//Plays Do-Re-Mi scale via the interrupt process
			//virtual void testDoReMi(uint8_t ocatve = 0, uint16_t noteDuration = 150, uint16_t noteGap = 50);

			//Tests stepping via the interrupt process
			//virtual void testStepInterrupt(uint32_t steps, bool direction);

			//Tests stepping via direct IO manipulation
			//virtual void testStepDirect(int32_t steps, bool direction);
	};
#endif