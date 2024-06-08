#ifndef IO_DigitalWrite_h
	#define IO_DigitalWrite_h
	
	#include <Arduino.h>
	
	#include "../Settings.h"
	#include "../SerialDebug/SerialDebug.h"
	#include "../MIDI_Device_Controller/MIDI_Periods.h"
	#include "IO_Device.h"
	#include "../Common/byteNoteRegister.h"
	#include "../Common/noteDuration.h"
	#include <stdint.h>	
	#include <SPI.h>

	//Resolve STL dependency
	#if defined(ARDUINO_ARCH_AVR)
		#include "../../ArduinoSTLClone/map"
		#include "../../ArduinoSTLClone/vector"
	#elif defined(CORE_TEENSY)
		#include <map>
	#elif defined(ESP32)
		#include <map>
	#endif
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	///MIDI device class for pulsing microcontroller outputs via Arduino digitalWrite()
	class IO_DigitalWrite : public IO_Device
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		struct changedOutput
		{
			changedOutput(uint8_t p, bool s)
			{
				pin = p;
				state = s;
			};
			
			uint8_t pin;
			bool state;
		};
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_DIGITALIO);
		
		uint8_t _numRegisters;		
		volatile bool _outputsChanged = false;
		inline static byteNoteRegister *_registers;
		uint16_t _maxOutputs = 0;
		uint16_t _usedOutputs = 0;
		inline static std::map<uint8_t, uint8_t> _outputMap;
		inline static std::vector<changedOutput> _changedOutputs;		
		
		void updateDurations();
		void updateIO();

		void checkMaxDuration();
		
		public:
			IO_DigitalWrite(uint8_t numOutputs);

			inline uint8_t getRegisterCount() { return _numRegisters; };
			
			void addOutput(uint8_t pin);
			void deleteOutput(uint8_t pin);
			bool isValidMapping(uint8_t out);
			void setMaxDuration(uint8_t out, uint32_t us);
			void setOutput(uint8_t out, bool value);
			void stopOuts();
			void updateOuts(); //Operates the digital IO per desired MIDI output
			
			void testOutputs();
	};
#endif