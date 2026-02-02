#ifndef MIDI_SquareWave_h
	#define MIDI_SquareWave_h

	#include "Base_MIDI_Pitch.h"

	//Pitch device that generates a square wave on the specified output. Used for devices like HDDs or piezo buzzers
	class MIDI_SquareWave : public Base_MIDI_Pitch
	{
		//Constructors
		//_____________________________________________________________________________________________
		public:
			using Base_MIDI_Pitch::Base_MIDI_Pitch; //Inherit constructors
		
			inline MIDI_SquareWave(IOType type, int8_t outNum)
			{ 
				setOutput(type, outNum);
				resetProperties(true);
			};
		
		//Configuration
		//_____________________________________________________________________________________________
		protected:
			//Designated output mapping where -1 indicates nothing is assigned
			int8_t _outNum = -1;
			IO_Device* _outIO = nullptr;
		
			inline void setOutputState(bool state)
			{
				if(_outNum < 0 || !_outIO) return;
				_outIO->setOutput(_outNum, state);
			}
			
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
					_debug.debugln(7, F("%d - Output was negative"), _id);
					_outIO = nullptr;
					return;
				}
				
				_outIO = IOF.getIO(type);
				if(!_outIO || !_outIO->isValidMapping(_outNum))
				{
					_debug.debugln(7, F("%d - IO (%d) not found or invalid mapping (%d)"), _id, type, _outNum);
					_outIO = nullptr;
					_outNum = -1;
				}
			}

			inline int8_t getOutputNum() { return _outNum; };

			inline bool getOutputState() 
			{
				if(_outNum < 0 || !_outIO)
					return LOW; //Have to return something
				
				return _outIO->getOutput(_outNum);
			};
			
		//Operation
		//_____________________________________________________________________________________________
		protected:
			//Resets a device's operational properties
			inline virtual void resetProperties(bool includeExtra = false)
			{
				Base_MIDI_Pitch::resetProperties(includeExtra);
				
				//Purely for visual effect when calling this during processing; in case there is an LED tied to it.
				setOutputState(LOW);
				
				//if(!includeExtra) return;
				//Reset extra properties here
			};
			
			//Sets up anything necessary before note playing begins
			inline void startPlaying()
			{
				//Make sure output stops when supposed to
				if(_outIO)
					_outIO->setShouldStop(_outNum, true);
			};
			
			inline void cycleOutput() { _outIO->toggleOutput(_outNum); };
			
		public:
			inline bool isEnabled() { return _outNum >= 0; };
			
		//Testing/debug
		//_____________________________________________________________________________________________
		public:
			//Test via the interrupt process
			inline virtual void testInterrupt()
			{
				//Nothing to test if no valid step output set
				if(!_outIO)
				{
					_debug.debugln(7, F("%d - Output not setup"), _id);
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
					_outIO->toggleOutput(_outNum); //Toggle on (step)
					delayMicroseconds(MANUAL_CHANGE_DELAY);
					_outIO->toggleOutput(_outNum); //Toggle back off
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}
				
				mdcStopPlaying();
			};

			//Test via direct IO manipulation
			inline virtual void testDirect()
			{
				//Nothing to test if no valid step output set
				if(!_outIO)
				{
					_debug.debugln(7, F("%d - Output not setup"), _id);
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
					_outIO->toggleOutput(_outNum); //Toggle on (step)
					callUpdateOutputs(_outIO);
					delayMicroseconds(MANUAL_CHANGE_DELAY);
					
					_outIO->toggleOutput(_outNum); //Toggle back off
					callUpdateOutputs(_outIO);
					delayMicroseconds(MANUAL_CHANGE_DELAY);
				}
			};
	};
	
#endif