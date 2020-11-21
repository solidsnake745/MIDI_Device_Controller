#ifndef MIDI_SN74HC595N_h
	#define MIDI_SN74HC595N_h
	
	#include "../SerialDebug/SerialDebug.h"
	#include "../MIDI_Device_Controller/MIDI_Periods.h"
	#include "IPulseNotes.h"
	#include "byteNoteRegister.h"
	#include "noteDurationTracker.h"
	#include <stdint.h>	
	#include <SPI.h>
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	///MIDI device class for pulsing shift register outputs via SPI
	class MIDI_SN74HC595N : public IPulseNotes
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		static SerialDebug _debug;
		
		public:
			MIDI_SN74HC595N(uint8_t numRegisters, uint8_t latchPin);

			inline uint8_t getRegisterCount() { return _numRegisters; };
			inline uint8_t getLatchPin() { return _latchPin; };
			
			inline void setLatchPin(uint8_t pin) 
			{ 
				_latchPin = pin;
				pinMode(_latchPin, OUTPUT);
				digitalWrite(_latchPin, LOW);
			};
			
			inline void setDuration(uint32_t limit)
			{
				_maxDuration.reset();
				_maxDuration.addMicroseconds(limit);
			};
			
			inline void setWriteDirection(bool value) { _writeDirection = value; };			
			
			bool isValidMapping(uint8_t out);
			void pulseOutput(uint8_t out);
			void stopOutput(uint8_t out);
			void stopOutputs();
			void playNotes(); //Operates the SPI bus per desired MIDI output
			
			void testRegistersDirect();
			
			//Plays the configured range of notes on the shift register via our interrupt process
			void testRegistersInterrupt();
			
		private:
			uint8_t _numRegisters;
			uint8_t _latchPin;
			noteDuration _maxDuration = noteDuration(1000);
			bool _writeDirection = true;
			volatile bool _registersChanged = false;
			byteNoteRegister *_registers;
			uint16_t _maxOutput = 0;
			MIDI_Device_Controller *_belongsTo = NULL;
			
			void updateDurations();
			void updateSN74HC595N();
			
			inline void latchRegisters()
			{
				digitalWrite(_latchPin, HIGH);
				digitalWrite(_latchPin, LOW);
			};
			
			void setController(MIDI_Device_Controller *controller);
	};
#endif