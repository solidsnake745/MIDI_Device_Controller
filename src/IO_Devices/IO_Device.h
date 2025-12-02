#ifndef IO_Device_h
	#define IO_Device_h

	#include <stdint.h>

	//Types of IO devices available
	//Enum values have to begin with a letter, so using IO prefix
	//Also can't match the class name so underscore is not included
	enum IOType
	{
		IODigital = 0,
		IO74HC595 = 1
	};

	//Forward declaration for compiling
	//class MIDI_Pitch;
	class MIDI_Device_Controller;

	/// @private
	class IO_Device
	{
		friend class MIDI_Pitch;
		// friend class Base_MIDI_Pulse;
		friend class MIDI_Pulse;
		friend class MIDI_Toggle;
		friend class Base_MIDI_SoftPWM;
		friend class MIDI_Device_Controller;
		
		virtual void updateOutputs() = 0;
		
		public:
			virtual bool isValidMapping(uint8_t out) = 0;
			virtual void setInverted(uint8_t out, bool value) = 0;
			virtual void setShouldStop(uint8_t out, bool value) = 0;
			virtual bool getOutput(uint8_t out) = 0;
			virtual void setOutput(uint8_t out, bool value) = 0;
			virtual void toggleOutput(uint8_t out) = 0;			
			virtual void testOutputs() = 0;
			virtual void stopOutputs() = 0;
			virtual void resetOutputs() = 0;
	};
#endif