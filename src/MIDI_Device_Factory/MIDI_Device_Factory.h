#ifndef MIDI_Device_Factory_h
	#define MIDI_Device_Factory_h

	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Device_Controller/MIDI_Pitch.h"
	#include "../MIDI_Device_Controller/MIDI_Shift_Register.h"
	
	///[MDF] Simplifies creating MIDI device objects and associating them to the device controller
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
			
			///Creates a new FDD device and adds it to the controller
			/*!
				\param index Index to assign the device to
				\param stepPin Pin to use for stepping
				\param dirPin Pin to use for toggling direction
			*/
			MIDI_Pitch *createFDD(uint8_t index, uint8_t stepPin, uint8_t dirPin);
			
			///Creates a new stepper motor device and adds it to the controller
			/*!
				\param index Index to assign the device to
				\param stepPin Pin to use for stepping
				\param dirPin (Optional) Pin to use if toggling direction
				\param maxPosition (Optional) Max position when toggling direction
			*/
			MIDI_Pitch *createStepper(uint8_t index, uint8_t stepPin, int8_t dirPin = -1, int32_t maxPosition = -1);
			
			///Creates a new HDD device and adds it to the controller
			/*!
				\param index Index to assign the device to
				\param signal Pin to use for the signal
			*/
			MIDI_Pitch *createHDD(uint8_t index, uint8_t signalPin);
			
			///Creates a new piezo buzzer device and adds it to the controller
			/*!
				\param index Index to assign the device to
				\param signal Pin to use for the signal
			*/
			MIDI_Pitch *createBuzzer(uint8_t index, uint8_t signalPin);
			
			///Creates a new shift register device and adds it to the controller
			/*!
				\param size Number of shift registers
				\param startingNote The first MIDI note to associate to the first output of the shift registers
				\param latchPin Pin to use for latching the registers
			*/
			MIDI_Shift_Register *createShiftRegister(uint8_t size, uint8_t startingNote, uint8_t latchPin);
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Device_Factory MDF;
#endif
