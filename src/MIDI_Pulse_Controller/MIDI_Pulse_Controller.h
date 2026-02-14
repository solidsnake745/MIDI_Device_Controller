#ifndef MIDI_Pulse_Controller_h
	#define MIDI_Pulse_Controller_h
	
	#include "../MIDI_Device_Controller.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	#include "../MIDI_Pulse/Base_MIDI_Pulse.h"
	
	//Resolve map dependency
	#if ARDUINO_ARCH_AVR
		#include "../../ArduinoSTLClone/map"
	#elif defined(CORE_TEENSY)
		#include <map>
	#elif ARDUINO_ARCH_ESP32
		#include <map>
	#endif
			
	///[MPC] Manages mapping MIDI notes to pulse devices
	class MIDI_Pulse_Controller
	{
		inline static SerialDebug _debug = SerialDebug(DEBUG_PULSECONTROLLER);
		
		//Constructor(s)
		MIDI_Pulse_Controller(); //Disallow creating an instance
		inline static MIDI_Pulse_Controller* _instance  = nullptr;
		
		static std::map<uint8_t, Base_MIDI_Pulse*> _noteMap;
		
		// Base_MIDI_Pulse* findDevice(uint8_t note);
		
		public:
			//Used to populate our single instance MDF for consumption
			/// @private
			inline static MIDI_Pulse_Controller& getInstance()
			{
				//Single instance check, instantiation, and return
				if (_instance == nullptr) _instance = new MIDI_Pulse_Controller();
				return *_instance;
			};
			
			///Adds a mapping between note and pulse device output
			/*!
				\param note MIDI note to map
				\param type IO device to map to
				\param output Output of the device to map to
			*/
			void addMapping(uint8_t note, Base_MIDI_Pulse* d);
			
			///Removes an existing mapping for the given note
			/*!
				\param note MIDI note to unmap
			*/
			void deleteMapping(uint8_t note);
			
			///Retrieves the device mapped to the given note if there is one
			/*!
				\param note MIDI note to retrieve the device for
			*/
			Base_MIDI_Pulse* getMappedDevice(uint8_t note);
			
			void pulseNote(uint8_t note);
			void stopNote(uint8_t note);
			void stopNotes();
	};
	
	//Defines a global instance of our class for users to consume
	inline MIDI_Pulse_Controller MPC = MIDI_Pulse_Controller::getInstance();
#endif
