#ifndef IO_74HC595_h
	#define IO_74HC595_h
	
	#include "../Settings.h"	
	#include "../Common/SerialDebug.h"
	#include "../Common/MIDI_Periods.h"
	#include "IO_Device.h"
	#include "../Common/byteNoteRegister.h"
	#include "../Common/noteDuration.h"
	#include <stdint.h>	
	#include <SPI.h>
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	/// @brief Class to manage a chain of shift registers outputs
	/// @details Currently utilizes the SPI bus to operate shift registers.<br>
	/// Developed using 74HC585 shift registers, but should theoretically work for similar chips.<br>
	/// TODO: Update to accept an SPI object so a different bus can be used.<br>
	/// TODO: Update to allow straight shiftOut like usage.<br>
	/// TODO: Update to allow multiple instances for separate chains of shift registers.
	class IO_74HC595 : public IO_Device
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_74HC595);
		
		//Lookup table for the reverse values of 4 bits
		//Index is the value you want to reverse
		//Example: Reverse of 0001 (1) is 1000 (8) so index 1 has the value 8
		inline constexpr static uint8_t reverseLookup[16] = {0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf};
		
		//Represents individual register outputs (8 per chip)
		struct regOut
		{
			regOut(ByteNoteRegister* r, uint8_t b)
			{
				reg = r;
				bitIndex = b;
			};
			
			ByteNoteRegister* reg;
			uint8_t bitIndex;
			bool shouldBeStopped = true;
		};
		
		uint8_t _numRegisters;
		uint8_t _latchPin;
		bool _reverseOutput = false;
		bool _registersChanged = false;
		uint16_t _maxOutputs = 0;
		inline static ByteNoteRegister* _registers;
		inline static regOut** _outputs;
		
		//Interface implementations		
		
		/// @brief Operates the SPI bus per desired MIDI output
		/// @details Want to use inline here as these are called in the ISR, but seem to be working fine without it for now.
		///	Will cause size bloat when used outside of it: setInverted(), testOutputs(), stopOutputs(), resetOutputs().
		/// TODO: Think of another solution to reduce calling this method or prevent inline bloat
		void updateOutputs() override;
		
		//Unique methods
		inline uint8_t reverseByte(uint8_t n)
		{ 
			//Taken from https://stackoverflow.com/a/2603254
			//Reverse the top and bottom nibble then swap them
			return (reverseLookup[n&0b1111] << 4) | reverseLookup[n>>4];
		};
		
		//NOTE: According to tutorials, you're supposed to set the latch pin high before the transmission of data and then low afterwards
		//	However, I've found that just toggling it afterwards seems to work fine also
		//	If there's something wrong with that or something else needs to change, let me know
		inline void latchRegisters()
		{
			digitalWrite(_latchPin, HIGH);
			digitalWrite(_latchPin, LOW);
		};
		
		public:
			/// @brief Main constructor
			/// @param numRegisters Number of registers in the chain
			/// @param latchPin Pin to use for latching
			IO_74HC595(uint8_t numRegisters, uint8_t latchPin);
			
			/// @brief Gets the number of registers configured
			inline uint8_t getRegisterCount() { return _numRegisters; };
			
			/// @brief Gets the pin being used for latching
			inline uint8_t getLatchPin() { return _latchPin; };
			
			/// @brief Sets the pin being used for latching
			/// @param pin Pin to use
			inline void setLatchPin(uint8_t pin)
			{ 
				_latchPin = pin;
				pinMode(_latchPin, OUTPUT);
				digitalWrite(_latchPin, LOW);
			};
			
			/// @brief Sets whether to reverse the output or not
			/// @param value Value to set where true will reverse the output
			inline void setReverseOutput(bool value) { _reverseOutput = value; };
			
			//Interface implementations
			bool isValidMapping(uint8_t out) override;
			void setInverted(uint8_t out, bool value) override;
			void setShouldStop(uint8_t out, bool value) override;
			bool getOutput(uint8_t out) override;
			void setOutput(uint8_t out, bool state) override;
			void toggleOutput(uint8_t out) override;
			void testOutputs() override;
			void stopOutputs() override;
			void resetOutputs() override;
	};
	
#endif