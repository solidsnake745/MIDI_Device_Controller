#ifndef IO_74HC595_h
	#define IO_74HC595_h
	
	#include "../Settings.h"	
	#include "../Common/SerialDebug.h"
	#include "../MIDI_Device_Controller/MIDI_Periods.h"
	#include "IO_Device.h"
	#include "../Common/byteNoteRegister.h"
	#include "../Common/noteDuration.h"
	#include <stdint.h>	
	#include <SPI.h>
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	///MIDI device class for pulsing shift register outputs via SPI
	class IO_74HC595 : public IO_Device
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_74HC595);
		
		//Lookup table for the reverse values of 4 bits
		//Index is the value you want to reverse
		//Example: Reverse of 0001 (1) is 1000 (8) so index 1 has the value 8
		constexpr static uint8_t reverseLookup[16] = {0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf};
		
		uint8_t _numRegisters;
		uint8_t _latchPin;
		volatile bool _registersChanged = false;
		ByteNoteRegister *_registers;
		uint16_t _maxOutput = 0;
		bool _reverseOutput = false;
		
		//Interface implementations
		void checkMaxDuration();
		void updateOutputs(); //Operates the SPI bus per desired MIDI output
		
		//Unique methods
		void updateDurations();
		void update74HC595();
		uint8_t reverseByte(uint8_t n);
		
		//NOTE: According to tutorials, you're supposed to set the latch pin high before the transmission of data and then low afterwards
		//	However, I've found that just toggling it afterwards seems to work fine? Not sure if I'm doing something wrong so leaving it as is
		inline void latchRegisters()
		{
			digitalWrite(_latchPin, HIGH);
			digitalWrite(_latchPin, LOW);
		};
		
		public:
			//Constructors/properties
			IO_74HC595(uint8_t numRegisters, uint8_t latchPin);

			inline uint8_t getRegisterCount() { return _numRegisters; };
			inline uint8_t getLatchPin() { return _latchPin; };
			inline void setReverseOutput(bool value) { _reverseOutput = value; };
			
			//Interface implementations
			bool isValidMapping(uint8_t out);
			void setMaxDuration(uint8_t out, uint32_t us);
			void setOutputInverted(uint8_t out, bool value);
			void setOutput(uint8_t out, bool value);
			void stopOutputs();
			
			//Unique methods
			inline void setLatchPin(uint8_t pin) 
			{ 
				_latchPin = pin;
				pinMode(_latchPin, OUTPUT);
				digitalWrite(_latchPin, LOW);
			};

			void testOutputs();
	};
#endif