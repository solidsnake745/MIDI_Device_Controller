#ifndef IO_Device_h
	#define IO_Device_h

	#include <stdint.h>

	/// @brief Types of IO devices available
	/// @details Enum values have to begin with a letter, so using IO prefix.
	/// Also can't match the class name so underscore is not included.
	enum IOType
	{
		/// Default representing invalid/no value
		NoValue = -1,
		/// IO_DigitalWrite
		IODigital = 0,
		/// IO_74HC595
		IO74HC595 = 1
	};

	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	/// @brief Interface that represents an IO device MDC knows how to use
	class IO_Device
	{
		friend class Base_MIDI_Pitch;
		friend class Base_MIDI_Pulse;
		friend class MIDI_Device_Controller;
		
		virtual void updateOutputs() = 0;
		
		public:
			/// @brief Determines if the specified output is a valid mapping
			/// @param out Output to check
			/// @retval true if valid
			virtual bool isValidMapping(uint8_t out) = 0;
			
			/// @brief Sets the specified output's inverted setting
			/// @param out Output to set
			/// @param value Value to set it to
			virtual void setInverted(uint8_t out, bool value) = 0;
			
			/// @brief Sets the specified output's inverted setting
			/// @param out Output to set
			/// @param value Value to set it to
			virtual void setShouldStop(uint8_t out, bool value) = 0;
			
			/// @brief Sets the specified output's inverted setting
			/// @param out Output to set
			/// @param value Value to set it to
			virtual bool getOutput(uint8_t out) = 0;
			
			/// @brief Sets the specified output's state
			/// @param out Output to set
			/// @param state State to set it to
			virtual void setOutput(uint8_t out, bool state) = 0;
			
			/// @brief Toggles the specified output's state
			/// @param out Output to toggle
			virtual void toggleOutput(uint8_t out) = 0;
			
			/// @brief Tests each of the configured outputs on this IO
			/// @details TODO: check implementations are consistent, describe pattern here
			virtual void testOutputs() = 0;
			
			/// @brief Stops each of the configured outputs on this IO
			/// @details Respects the shouldStop setting of each output.
			/// Outputs where shouldStop is true will be reset to their initial state (based on inverted setting).
			/// This is automatically called through the framework (see MIDI_Device_Controller::stopPlaying()).
			virtual void stopOutputs() = 0;
			
			/// @brief Stops each of the configured outputs on this IO
			/// @details Outputs are reset to their initial state (based on inverted setting) regardless of their shouldStop setting.
			virtual void resetOutputs() = 0;
	};
	
#endif