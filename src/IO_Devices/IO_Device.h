#ifndef IO_Device_h
	#define IO_Device_h
	
	#include <stdint.h>
	
	//Types of IO devices available
	enum IO_DeviceEnum
	{
		DigitalWrite = 0,
		SN74HC595N = 1
	};
	
	/// @private
	class IO_Device
	{
		public:
			virtual bool isValidMapping(uint8_t out) = 0;
			virtual void setMaxDuration(uint8_t out, uint32_t us) = 0;
			virtual void setOutput(uint8_t out, bool value) = 0;
			virtual void stopOuts() = 0;
			virtual void updateOuts() = 0;
	};
	
#endif