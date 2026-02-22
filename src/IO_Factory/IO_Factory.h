#ifndef IO_Factory_h
	#define IO_Factory_h

	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Devices/IO_DigitalWrite.h"
	#include "../IO_Devices/IO_74HC595.h"
	
	#define MAX_IO_DEVICES 2
	
	/// @brief Types of IO devices available
	/// @details Enum values have to begin with a letter, so using IO prefix.<br>
	/// Also can't match the class name so underscore is not included.
	enum IOType
	{
		/// @brief Default representing invalid/no value
		NoValue = -1,
		/// @brief IO_DigitalWrite
		IODigital = 0,
		/// @brief IO_74HC595
		IO74HC595 = 1
	};
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	/// @brief Creates, stores, and provides IO_Device instances
	/// @details Used to setup which IOs to connect to for operation.
	class IO_Factory
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructor(s)
		IO_Factory(); //Disallow creating an instance
		inline static IO_Factory* _instance = nullptr;
		inline static IO_Device* _ioDevices[MAX_IO_DEVICES];
		
		public:
			/// @private
			/// @brief Used to populate our single instance IOF for consumption
			static IO_Factory& getInstance();			
			
			/// @brief Gets the specified IO instance if populated
			/// @return The specified IO_device
			/// @retval nullptr if not yet created
			IO_Device* const getIO(IOType type);
			
			/// @brief Creates a new digital IO device
			/// @param numOutputs Max number of outputs to control
			IO_DigitalWrite* const createDigitalIO(uint8_t numOutputs);
			
			/// @brief Creates a new shift register IO device
			/// @param numRegisters Number of shift registers in the chain
			/// @param latchPin Pin to use for latching the registers in this chain
			IO_74HC595* const create74HC595(uint8_t numRegisters, uint8_t latchPin);
	};
	
	/// @brief Global singleton instance of IO_Factory
	inline static IO_Factory IOF = IO_Factory::getInstance();
	
#endif
