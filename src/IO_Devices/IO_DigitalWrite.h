#ifndef IO_DigitalWrite_h
	#define IO_DigitalWrite_h
	
	#include <Arduino.h>
	
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	#include "../MIDI_Device_Controller/MIDI_Periods.h"
	#include "IO_Device.h"
	#include "../Common/ByteNoteRegister.h"
	#include "../Common/noteDuration.h"
	#include <stdint.h>	
	#include <SPI.h>

	//Resolve STL dependency
	#if ARDUINO_ARCH_AVR
		#include "../../ArduinoSTLClone/map"
		#include "../../ArduinoSTLClone/vector"
	#elif defined(CORE_TEENSY)
		#include <map>
	#elif ARDUINO_ARCH_ESP32
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
		inline static ByteNoteRegister *_registers;
		uint16_t _maxOutputs = 0;
		uint16_t _usedOutputs = 0;
		static std::map<uint8_t, uint8_t> _outputMap;
		static std::vector<changedOutput> _changedOutputs;		
		
		//Interface implementations
		void checkMaxDuration();
		void updateOutputs(); //Operates the digital IO per desired MIDI output
		
		//Unique methods
		void updateDurations();
		void updateIO();
		
		public:
			//Constructors/properties
			IO_DigitalWrite(uint8_t numOutputs);

			inline uint8_t getRegisterCount() { return _numRegisters; };		
			
			//Interface implementations
			bool isValidMapping(uint8_t out);
			void setMaxDuration(uint8_t out, uint32_t us);
			void setOutputInverted(uint8_t out, bool value);
			void setOutput(uint8_t out, bool value);
			void stopOutputs();
			
			//Unique methods
			void addOutput(uint8_t pin);
			void deleteOutput(uint8_t pin);
			void testOutputs();
	};
#endif