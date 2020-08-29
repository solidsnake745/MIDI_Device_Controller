#ifndef MIDI_Device_Factory_h
	#define MIDI_Device_Factory_h

	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Device_Controller/MIDI_Pitch.h"
	#include "../MIDI_Device_Controller/MIDI_Shift_Register.h"
	
	class MIDI_Device_Factory
	{
		//Constructor(s)
		MIDI_Device_Factory(); //Disallow creating an instance
		static MIDI_Device_Factory *_instance;
		
		bool validateIndex(uint8_t index);
		
		public:
			//Used to populate our single instance MDF for consumption
			/// @private
			static MIDI_Device_Factory &getInstance();			
			
			MIDI_Pitch *createFDD(uint8_t index, uint8_t stepPin, uint8_t dirPin);
			MIDI_Pitch *createStepper(uint8_t index, uint8_t stepPin, int8_t dirPin = -1, int32_t maxPosition = -1);
			MIDI_Pitch *createHDD(uint8_t index, uint8_t signalPin);
			MIDI_Pitch *createBuzzer(uint8_t index, uint8_t signalPin);
			MIDI_Shift_Register *createShiftRegister(uint8_t size, uint8_t startingNote, uint8_t latchPin);
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Device_Factory MDF;
#endif
