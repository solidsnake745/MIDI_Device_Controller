#ifndef IO_Factory_h
	#define IO_Factory_h

	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Devices/IO_DigitalWrite.h"
	#include "../IO_Devices/IO_SN74HC595N.h"
	
	#define MAX_IO_DEVICES 2
	
	//Forward declaration for compiling
	class MIDI_Device_Controller;
	
	///[IOF] Simplifies creating IO device objects and associating them to other objects
	class IO_Factory
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_Device_Controller;
		
		//Constructor(s)
		IO_Factory(); //Disallow creating an instance
		static IO_Factory *_instance;
		static IO_Device *_ioDevices[MAX_IO_DEVICES];
		
		public:
			//Used to populate our single instance MDF for consumption
			/// @private
			static IO_Factory &getInstance();			
			
			IO_Device *getIO(IOType type);
			
			///Creates a new digital IO device and adds it to the controller
			/*!
				\param size Number of digital outputs
			*/
			IO_DigitalWrite *createDigitalIO(uint8_t numOutputs);
			
			///Creates a new shift register device and adds it to the controller
			/*!
				\param numRegisters Number of shift registers
				\param latchPin Pin to use for latching the registers
			*/
			IO_74HC595 *create74HC595(uint8_t numRegisters, uint8_t latchPin);
	};
	
	//Defines a global instance of our class for users to consume
	extern IO_Factory IOF;
#endif
