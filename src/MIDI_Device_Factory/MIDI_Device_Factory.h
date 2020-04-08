#ifndef MIDI_Device_Factory_h
	#define MIDI_Device_Factory_h

	#include "../MIDI_Device_Controller/MIDI_Device.h"
	
	//enum DeviceType_t { FDD, Stepper, HDD };	
	
	class MIDI_Device_Factory
	{
		//Constructor(s)
		MIDI_Device_Factory(); //Disallow creating an instance
		static MIDI_Device_Factory *_instance;
			
		public:
			//Used to populate our single instance MDF for consumption
			static MIDI_Device_Factory &getInstance();
			
			inline MIDI_Device *createFDD(uint8_t stepPin, uint8_t dirPin)
			{ return new MIDI_Device(stepPin, dirPin, 158); };
			
			inline MIDI_Device *createStepper(uint8_t stepPin, int8_t dirPin = -1, int32_t maxPosition = -1)
			{ return new MIDI_Device(stepPin, dirPin, maxPosition); };
			
			inline MIDI_Device *createHDD(uint8_t signalPin)
			{ return new MIDI_Device(signalPin); };
			
			inline MIDI_Device *createBuzzer(uint8_t signalPin)
			{ return new MIDI_Device(signalPin); };
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Device_Factory MDF;
#endif
