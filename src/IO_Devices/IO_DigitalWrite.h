#ifndef IO_DigitalWrite_h
	#define IO_DigitalWrite_h
	
	#include <Arduino.h>
	
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	#include "../Common/MIDI_Periods.h"
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
		#include <vector>
	#elif ARDUINO_ARCH_ESP32
		#include <map>
		#include <vector>
	#endif
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	///MIDI device class for pulsing microcontroller outputs via Arduino digitalWrite()
	class IO_DigitalWrite : public IO_Device
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		struct pinOut
		{
			pinOut(uint8_t p, ByteNoteRegister* r, uint8_t b)
			{
				pin = p;
				reg = r;								
				bitIndex = b;
			};
			
			uint8_t pin;
			ByteNoteRegister* reg;
			uint8_t bitIndex;
			bool shouldBeStopped = true;
		};
		
		struct changedOutput
		{
			changedOutput(pinOut* p)
			{
				pin = p->pin;
				state = p->reg->getBitValue(p->bitIndex); //getBitValue applies the invert setting
			};
			
			uint8_t pin;
			bool state;
		};
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_DIGITALIO);
		
		uint8_t _numRegisters;		
		bool _outputsChanged = false;		
		uint16_t _maxOutputs = 0;
		uint16_t _usedOutputs = 0;
		inline static ByteNoteRegister* _registers;
		inline static pinOut** _outputs;
		static std::map<uint8_t, pinOut*> _pinMap;
		static std::vector<changedOutput> _changedOutputs;
		
		//Interface implementations
		void updateOutputs(); //Operates the digital IO per desired MIDI output
		
		//Unique methods
		pinOut* findOutput(uint8_t out);
		void updateIO();
		
		public:
			//Constructors/properties
			IO_DigitalWrite(uint8_t numOutputs);

			inline uint8_t getRegisterCount() { return _numRegisters; };		
			
			//Interface implementations
			bool isValidMapping(uint8_t out);
			void setInverted(uint8_t out, bool value);
			void setShouldStop(uint8_t out, bool value);
			bool getOutput(uint8_t out);
			void setOutput(uint8_t out, bool value);
			void toggleOutput(uint8_t out);
			void testOutputs();
			void stopOutputs();
			void resetOutputs();
			
			//Unique methods
			void addPin(uint8_t pin);
			void deletePin(uint8_t pin);
	};
#endif