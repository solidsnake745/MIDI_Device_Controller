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
	
	/// @brief Class to manage microcontroller outputs that use Arduino digitalWrite()
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
		
		//Unique methods
		pinOut* findOutput(uint8_t out);
		
		//Interface implementations
		
		/// @brief Operates the digital IO per desired MIDI output
		/// @details Want to use inline here as these are called in the ISR, but seem to be working fine without it for now.
		///	Will cause size bloat when used outside of it: setInverted(), testOutputs(), stopOutputs(), resetOutputs().
		/// TODO: Think of another solution to reduce calling this method or prevent inline bloat
		void updateOutputs();
		
		public:
			/// @brief Main constructor
			/// @param numOutputs Max number of outputs to control
			IO_DigitalWrite(uint8_t numOutputs);
			
			//Unique methods
			
			/// @brief Gets the number of registers configured
			inline uint8_t getRegisterCount() { return _numRegisters; };
			
			/// @brief Adds a pin to be managed
			/// @param pin Pin to add
			void addPin(uint8_t pin);
			
			/// @brief Removes a pin from being managed
			/// @param pin Pin to remove
			void deletePin(uint8_t pin);
			
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