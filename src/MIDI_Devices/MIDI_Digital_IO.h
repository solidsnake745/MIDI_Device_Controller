#ifndef MIDI_Digital_IO_h
	#define MIDI_Digital_IO_h
	
	#include "../SerialDebug/SerialDebug.h"
	#include "../MIDI_Device_Controller/MIDI_Periods.h"
	#include "IPulseNotes.h"
	#include "byteNoteRegister.h"
	#include "noteDurationTracker.h"
	#include <stdint.h>	
	#include <SPI.h>

	//Resolve STL dependency
	#if defined(ARDUINO_ARCH_AVR)
		#include "ArduinoSTL.h"
		#include "map"
	#elif defined(CORE_TEENSY)
		#include <map>
	#elif defined(ESP32)
		#include <map>
	#endif
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	///MIDI device class for pulsing microcontroller outputs via Arduino digitalWrite()
	class MIDI_Digital_IO : public IPulseNotes
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		static SerialDebug _debug;
		
		public:
			MIDI_Digital_IO(uint8_t numOutputs);

			inline uint8_t getRegisterCount() { return _numRegisters; };
			
			inline void setDuration(uint32_t limit) 
			{
				_maxDuration.reset();
				_maxDuration.addMicroseconds(limit);
			};
			
			void addOutput(uint8_t pin);
			void deleteOutput(uint8_t pin);
			
			bool isValidMapping(uint8_t out);
			void pulseOutput(uint8_t out);
			void stopOutput(uint8_t out);
			void stopOutputs();
			void playNotes(); //Operates the digital IO per desired MIDI output
			
			void testOutputsDirect();
			
			//Plays the configured range of notes on the shift register via our interrupt process
			void testOutputsInterrupt();
			
		private:
			uint8_t _numRegisters;
			noteDuration _maxDuration = noteDuration(1000);
			bool _outputsChanged = false;
			byteNoteRegister *_registers;
			uint16_t _maxOutputs = 0;
			uint16_t _usedOutputs = 0;
			MIDI_Device_Controller *_belongsTo = NULL;
			std::map<uint8_t, uint8_t> _outputMap;
			
			void updateDurations();
			void updateIO();
			
			void setController(MIDI_Device_Controller *controller);
	};
#endif