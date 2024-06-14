#ifndef IO_SN74HC595N_h
	#define IO_SN74HC595N_h
	
	#include "../Settings.h"	
	#include "../SerialDebug/SerialDebug.h"
	#include "../MIDI_Device_Controller/MIDI_Periods.h"
	#include "IO_Device.h"
	#include "../Common/byteNoteRegister.h"
	#include "../Common/noteDuration.h"
	#include <stdint.h>	
	#include <SPI.h>
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	///MIDI device class for pulsing shift register outputs via SPI
	class IO_SN74HC595N : public IO_Device
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_SN74HC595N);
		
		//Lookup table for the reverse values of 4 bits i.e. reverse of 0001 (1) is 1000 (8)
		constexpr static unsigned char reverseLookup[16] = {
			0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
			0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf };

		uint8_t reverseByte(uint8_t n);
		
		uint8_t _numRegisters;
		uint8_t _latchPin;
		volatile bool _registersChanged = false;
		ByteNoteRegister *_registers;
		uint16_t _maxOutput = 0;
		bool _reverseOutput = false;
		
		void checkMaxDuration();
		void updateOuts(); //Operates the SPI bus per desired MIDI output
		
		void updateDurations();
		void updateSN74HC595N();			
		
		//NOTE: According to tutorials, you're supposed to set the latch pin high before the transmission of data and then low afterwards
		//	However, I've found that just toggling it afterwards seems to work fine? Not sure if I'm doing something wrong so leaving it as is
		inline void latchRegisters()
		{
			digitalWrite(_latchPin, HIGH);
			digitalWrite(_latchPin, LOW);
		};
		
		public:
			IO_SN74HC595N(uint8_t numRegisters, uint8_t latchPin);

			inline uint8_t getRegisterCount() { return _numRegisters; };
			inline uint8_t getLatchPin() { return _latchPin; };
			
			inline void setLatchPin(uint8_t pin) 
			{ 
				_latchPin = pin;
				pinMode(_latchPin, OUTPUT);
				digitalWrite(_latchPin, LOW);
			};
			
			inline void setReverseOutput(bool value) { _reverseOutput = value; };			
			
			bool isValidMapping(uint8_t out);
			void setMaxDuration(uint8_t out, uint32_t us);
			void setOutput(uint8_t out, bool value);
			void stopOuts();			
			
			void testOutputs();
	};
#endif