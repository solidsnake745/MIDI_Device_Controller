#ifndef MIDI_Unipolar_h
	#define MIDI_Unipolar_h

	#include "MIDI_SquareWave.h"
	#include "Base_MIDI_Pitch_Direction.h"
	
	enum UnipolarStepMode
	{
		None = -1,
		FullStep,
		AltFullStep,
		HalfStep
	};

	//Pitch device that generates a unipolar step sequence on the specified outputs and tracks/manages position.
	//Used for devices with a unipolar stepper motor such as older flatbed scanners
	class MIDI_Unipolar : public Base_MIDI_Pitch, public Base_MIDI_Pitch_Direction
	{
		protected:
			static constexpr uint8_t FullStepSequence[] = { 8, 4, 2, 1 };
			static constexpr uint8_t Max_FullStepIndex = (sizeof(FullStepSequence) / sizeof(FullStepSequence[0])) - 1;
			static constexpr uint8_t AltFullStepSequence[] = { 9, 12, 6, 3 };
			static constexpr uint8_t MaxAltFullStepIndex = (sizeof(AltFullStepSequence) / sizeof(AltFullStepSequence[0])) - 1;
			static constexpr uint8_t HalfStepSequence[] = { 8, 12, 4, 6, 2, 3, 1, 9 };
			static constexpr uint8_t MaxHalfStepIndex = (sizeof(HalfStepSequence) / sizeof(HalfStepSequence[0])) - 1;
		
			inline bool hasDirection() { return true; };
			
		public:
			inline Base_MIDI_Pitch_Direction* asDir() { return static_cast<Base_MIDI_Pitch_Direction*>(this); };
		
		//Constructors
		//_____________________________________________________________________________________________
		public:
			using Base_MIDI_Pitch::Base_MIDI_Pitch; //Inherit constructors
			
			inline MIDI_Unipolar(
				IOType type1, int8_t outNum1,
				IOType type2, int8_t outNum2,
				IOType type3, int8_t outNum3,
				IOType type4, int8_t outNum4,
				int16_t maxPosition = -1,
				UnipolarStepMode mode = UnipolarStepMode::None)
			{
				setBase(this);
				setOutput(1, type1, outNum1);
				setOutput(2, type2, outNum2);
				setOutput(3, type3, outNum3);
				setOutput(4, type4, outNum4);
				setMaxPosition(maxPosition);
				setMode(mode);
			};
		
		//Configuration
		//_____________________________________________________________________________________________
		private:
			int8_t _outNum1 = -1;
			IO_Device* _outIO1 = nullptr;
			
			int8_t _outNum2 = -1;
			IO_Device* _outIO2 = nullptr;
			
			int8_t _outNum3 = -1;
			IO_Device* _outIO3 = nullptr;
			
			int8_t _outNum4 = -1;
			IO_Device* _outIO4 = nullptr;
			
			bool _dirState= false;
			UnipolarStepMode _mode = UnipolarStepMode::None;
			const uint8_t* _sequence = nullptr;
			uint8_t _sequenceMaxIndex;
			uint8_t _currentIndex;

		public:
			//Print out this device's configuration
			inline void printStatus()
			{
				Base_MIDI_Pitch::printStatus();
				_debug.println(F("  Output 1 Number: %d"), _outNum1);
				_debug.println(F("  Output 2 Number: %d"), _outNum2);
				_debug.println(F("  Output 3 Number: %d"), _outNum3);
				_debug.println(F("  Output 4 Number: %d"), _outNum4);
				
				if(_outIO1)
					_debug.println(F("  Output 1 State: %d"), _outIO1->getOutput(_outNum1));
				if(_outIO2) 
					_debug.println(F("  Output 2 State: %d"), _outIO2->getOutput(_outNum2));
				if(_outIO3) 
					_debug.println(F("  Output 3 State: %d"), _outIO3->getOutput(_outNum3));
				if(_outIO4) 
					_debug.println(F("  Output 4 State: %d"), _outIO4->getOutput(_outNum4));
				
				_debug.println(F("  Dir State: %d"), _dirState);
				_debug.println(F("  Max Position: %d"), getMaxPosition());
				_debug.println(F("  Step Mode: %d"), _mode);
			};
		
			//Sets the IO and output number (from IO) for the specified output (from this device, 1-4)
			inline void setOutput(uint8_t outNum, IOType type, int8_t ioOutNum)
			{
				int8_t* _outNum;
				IO_Device** _outIO;
				
				switch(outNum)
				{
					case 1:
						_outNum = &_outNum1;
						_outIO = &_outIO1;
						break;
						
					case 2:
						_outNum = &_outNum2;
						_outIO = &_outIO2;
						break;
						
					case 3:
						_outNum = &_outNum3;
						_outIO = &_outIO3;
						break;
						
					case 4:
						_outNum = &_outNum4;
						_outIO = &_outIO4;
						break;
						
					default:
						_debug.println(F("%d - Invalid output: %d"), _id, outNum);
						return;
				};
				
				*_outNum = ioOutNum;
				if (*_outNum < 0)
				{
					_debug.println(F("%d - Output was negative"), _id);
					*_outIO = nullptr;
					return;
				}
				
				*_outIO = IOF.getIO(type);
				if(!(*_outIO) || !((*_outIO)->isValidMapping(*_outNum)))
				{
					_debug.println(F("%d - IO (%d) not found or invalid mapping (%d)"), _id, type, ioOutNum);
					_outIO = nullptr;
					*_outNum = -1;
				}
			};
		
		//Operation
		//_____________________________________________________________________________________________
		private:
			bool _outputsActive;
			
			inline uint8_t getNextSequence()
			{
				if(_mode == UnipolarStepMode::None)
					return 0;
				
				if(_dirState == false) //Forward, matches other pitch devices
					_currentIndex = _currentIndex == _sequenceMaxIndex ? 0 : _currentIndex + 1;
				else
					_currentIndex = _currentIndex == 0 ? _sequenceMaxIndex : _currentIndex - 1;
				
				return _sequence[_currentIndex];
			}
			
			inline void manualIOUpdate()
			{
				//Call updateOutputs on unique IOs
				uint8_t updateCount = 0;
				IO_Device* tmpIOs[] = { _outIO1, _outIO2, _outIO3, _outIO4 };
				for(int x = 0; x < 4; x++)
				{
					if(!tmpIOs[x]) continue; //No IO set
					callUpdateOutputs(tmpIOs[x]);
					updateCount++;
					
					//Remove duplicates from the rest of the array
					for(int y = (x + 1); y < 4; y++)
					{
						if(!tmpIOs[y]) continue; //No IO set
						if(tmpIOs[x] == tmpIOs[y])  tmpIOs[y] = nullptr; //Same IO, remove
					}
				}
				
				_debug.println(F("Updated %d IO(s)"), updateCount);
			}
			
			inline void updateOutputs(uint8_t seq)
			{
				bool o1 = ((seq >> 3) & 1);
				bool o2 = ((seq >> 2) & 1);
				bool o3 = ((seq >> 1) & 1);
				bool o4 = ((seq >> 0) & 1);
				
				_debug.debugln(7, F("New outputs: %d %d %d %d"), o1, o2, o3, o4);

				_outIO1->setOutput(_outNum1, o1);
				_outIO2->setOutput(_outNum2, o2);
				_outIO3->setOutput(_outNum3, o3);
				_outIO4->setOutput(_outNum4, o4);
				_outputsActive = seq > 0;
			}

			//Resets a device's operational properties
			inline void resetProperties(bool includeExtra = false)
			{
				Base_MIDI_Pitch::resetProperties(includeExtra);
				updateOutputs(0);
				
				if(!includeExtra) return;
				//Reset extra properties here
			};
			
			inline uint32_t getBasePeriod() { return Base_MIDI_Pitch::getBasePeriod() * 2; }; //Double it so we only change state once per period
			
			//Sets up anything necessary before note playing begins
			inline void startPlaying()
			{
				//Make sure outputs stops/don't stop when supposed to
				if(_outIO1) _outIO1->setShouldStop(_outNum1, true);
				if(_outIO2) _outIO2->setShouldStop(_outNum2, true);
				if(_outIO3) _outIO3->setShouldStop(_outNum3, true);
				if(_outIO4) _outIO4->setShouldStop(_outNum4, true);
			};
			
			public:
			inline void cycleOutput()
			{ 
				updateOutputs(getNextSequence());
				
				//Increment position if valid
				if(!isTrackingPosition())
					return;
				
				_currentPosition++; //Every state change for unipolar stepper motors will cause a step
				if(isAtMaxPosition()) //Direction update pending
				{
					_debug.debugln(20, F("%d - Toggling direction"), _id);
					toggleDirection();
					zeroPosition();
				}
			};
			
			private:
			inline bool getDirState() { return _dirState; };
			inline void setDirState(bool state) { _dirState = state; };
			inline void toggleDirection() { _dirState = !_dirState; };
			
			inline void setMode(UnipolarStepMode mode)
			{
				if(_mode == mode)
				{
					_debug.debugln(7, F("%d - Already set to mode: %d"), _id, mode);
					return;
				};
				
				switch(mode)
				{
					case FullStep:
						_sequence = FullStepSequence;
						_sequenceMaxIndex = Max_FullStepIndex;
						break;

					case AltFullStep:
						_sequence = AltFullStepSequence;
						_sequenceMaxIndex = MaxAltFullStepIndex;
						break;

					case HalfStep:
						_sequence = HalfStepSequence;
						_sequenceMaxIndex = MaxHalfStepIndex;
						break;
					default:
						_mode = UnipolarStepMode::None;
						return;
				}
				
				_mode = mode;
				_currentIndex = 0;
				updateOutputs(_sequence[_currentIndex]);
				manualIOUpdate();
				if(!mdcIsPlayingNotes())
				{
					delayMicroseconds(50);
					updateOutputs(0);
					manualIOUpdate();
					delayMicroseconds(50);
				}
			};
			
		public:
			inline bool isEnabled()
			{
				return (_outNum1 >= 0) //Are all outputs are set?
					&& (_outNum2 >= 0) 
					&& (_outNum3 >= 0) 
					&& (_outNum4 >= 0);
			};
			
			inline void stepOnce(bool includeDelay = false)
			{
				//Re-initialize to the current state
				if(!_outputsActive)
				{
					updateOutputs(_sequence[_currentIndex]);
					delayMicroseconds(50);
				}
				
				updateOutputs(getNextSequence());
				//update all IOs ->updateOutputs();
				
				if(includeDelay)
					delayMicroseconds(MANUAL_CHANGE_DELAY);
			};
			
		//Testing/debug
		//_____________________________________________________________________________________________
		public:
			//Test via the interrupt process
			// void testInterrupt();

			//Test via direct IO manipulation
			// void testDirect();
			
			//Test that operates the device to it's max position and then back to start
			inline void testMaxDirection() {};

			//Tests stepping via the interrupt process
			// void testStepInterrupt(uint32_t steps = 10, bool direction = false);

			//Tests stepping via direct IO manipulation
			// void testStepDirect(uint32_t steps = 10, bool direction = false);
	};
	
#endif