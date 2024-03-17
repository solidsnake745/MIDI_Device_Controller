#ifndef IO_SN74HC595N_h
	#define IO_SN74HC595N_h
	
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
		
		static SerialDebug _debug;
		
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
			
			inline void setWriteDirection(bool value) { _writeDirection = value; };			
			
			bool isValidMapping(uint8_t out);
			void setMaxDuration(uint8_t out, uint32_t us);
			void setOutput(uint8_t out, bool value);
			void stopOuts();
			void updateOuts(); //Operates the SPI bus per desired MIDI output
			
			void testOutputs();
			
		private:
			constexpr static unsigned char lookup[16] = {
				0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
				0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf };

			uint8_t _numRegisters;
			uint8_t _latchPin;
			volatile bool _registersChanged = false;
			byteNoteRegister *_registers;
			uint16_t _maxOutput = 0;
			bool _writeDirection = false;	
			
			uint8_t reverse(uint8_t n);
			void updateDurations();
			void updateSN74HC595N();
			
			inline void latchRegisters()
			{
				digitalWrite(_latchPin, HIGH);
				digitalWrite(_latchPin, LOW);
			};
	};
#endif