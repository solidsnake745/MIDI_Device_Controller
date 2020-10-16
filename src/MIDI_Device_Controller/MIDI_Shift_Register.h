#ifndef MIDI_Shift_Register_h
	#define MIDI_Shift_Register_h
	
	#include "../SerialDebug/SerialDebug.h"
	#include "MIDI_Periods.h"
	#include <stdint.h>	
	#include <SPI.h>
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	///MIDI device class for pulsing shift register outputs via SPI
	class MIDI_Shift_Register
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		static SerialDebug _debug;
		
		private:
			struct byteBits
			{
			  volatile unsigned 
				b0 : 1,
				b1 : 1,
				b2 : 1,
				b3 : 1,
				b4 : 1,
				b5 : 1,
				b6 : 1,
				b7 : 1;
			  
			  bool getBit(uint8_t index)
			  {
				switch(index)
				{
				  case 0: return b0;
				  case 1: return b1;
				  case 2: return b2;
				  case 3: return b3;
				  case 4: return b4;
				  case 5: return b5;
				  case 6: return b6;
				  case 7: return b7;
				}
				
				return false;
			  }
			  
			  void setBit(uint8_t index)
			  {
				switch(index)
				{
				  case 0: b0 = 1; break;
				  case 1: b1 = 1; break;
				  case 2: b2 = 1; break;
				  case 3: b3 = 1; break;
				  case 4: b4 = 1; break;
				  case 5: b5 = 1; break;
				  case 6: b6 = 1; break;
				  case 7: b7 = 1; break;
				}
			  }

			  void clearBit(uint8_t index)
			  {
				switch(index)
				{
				  case 0: b0 = 0; break;
				  case 1: b1 = 0; break;
				  case 2: b2 = 0; break;
				  case 3: b3 = 0; break;
				  case 4: b4 = 0; break;
				  case 5: b5 = 0; break;
				  case 6: b6 = 0; break;
				  case 7: b7 = 0; break;
				}
			  }
			};

			union shiftRegister
			{
			  byte value;
			  byteBits bits;
			};

			struct registerDurations
			{
			  volatile uint32_t durations[8];

			  uint32_t getDuration(uint8_t index)
			  {
				if(index > 7) return 0;
				return durations[index];
			  }

			  void incrementDuration(uint8_t index, uint32_t resolution)
			  {
				if(index > 7) return;
				durations[index] += resolution;
			  }
			  
			  void incrementAllDurations(uint32_t resolution)
			  {
				for(int i = 0; i < 8; i++)
					incrementDuration(i, resolution);
			  }
			  
			  void resetDuration(uint8_t index)
			  {
				if(index > 7) return;
				durations[index] = 0;
			  }
			  
			  void resetAllDurations()
			  {
				for(int i = 0; i < 8; i++)
					resetDuration(i);
			  }
			};
		
		public:
			MIDI_Shift_Register(uint8_t size, uint8_t startingNote, uint8_t latchPin);

			inline uint8_t getRegisterCount() { return _numRegisters; };
			inline uint8_t getStartingNote() { return _startingNote; };
			inline uint8_t getEndingNote() { return _endingNote; };
			inline uint8_t getLatchPin() { return _latchPin; };
			inline uint32_t getDuration() { return _durationLimit; };
			
			inline void setLatchPin(uint8_t pin) 
			{ 
				_latchPin = pin;
				pinMode(_latchPin, OUTPUT);
				digitalWrite(_latchPin, LOW);
			};
			inline void setDuration(uint32_t limit) { _durationLimit = limit; };
			inline void setWriteDirection(bool value) { _writeDirection = value; };
			
			void playNote(uint8_t note);
			void stopNote(uint8_t note);
			void stopNotes();			
			
			void testRegistersDirect();
			
		private:
			uint8_t _numRegisters;
			uint8_t _startingNote;
			uint8_t _endingNote;
			uint8_t _latchPin;
			uint32_t _durationLimit = 1000;
			bool _writeDirection = true;
			volatile bool _registersChanged = false;
			uint32_t _numActiveOutputs = 0;
			shiftRegister *_registers;
			registerDurations *_durations;
			MIDI_Device_Controller *_belongsTo = NULL;
			
			bool validateNote(uint8_t note);
			void playNotes(); //Operates the SPI bus per desired MIDI output
			void updateDurations();
			void updateRegisters();
			
			inline void latchRegisters()
			{
				digitalWrite(_latchPin, HIGH);
				digitalWrite(_latchPin, LOW);
			};
			
			void setController(MIDI_Device_Controller *controller);
	};
#endif