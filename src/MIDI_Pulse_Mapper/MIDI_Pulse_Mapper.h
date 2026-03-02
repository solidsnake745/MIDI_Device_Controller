#ifndef MIDI_Pulse_Mapper_h
	#define MIDI_Pulse_Mapper_h
	
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
	
	/// @brief Stores and provides Base_MIDI_Pulse instances mapped to MIDI notes
	/// @details Used to setup and manage which Base_MIDI_Pulse are triggered based on MIDI note.
	class MIDI_Pulse_Mapper
	{
		inline static SerialDebug _debug = SerialDebug(DEBUG_PULSEMAPPER);
		
		//Constructor(s)
		MIDI_Pulse_Mapper(); //Disallow creating an instance
		inline static MIDI_Pulse_Mapper* _instance  = nullptr;
		
		static std::map<uint8_t, Base_MIDI_Pulse*> _noteMap;
		
		public:
			/// @private
			/// @brief Used to populate our single instance IOF for consumption
			inline static MIDI_Pulse_Mapper& getInstance()
			{
				//Single instance check, instantiation, and return
				if (_instance == nullptr) _instance = new MIDI_Pulse_Mapper();
				return *_instance;
			};
			
			/// @brief Adds a mapping between note and Base_MIDI_Pulse output
			/// @param note MIDI note to map
			/// @param note Base_MIDI_Pulse instance to associate
			void addMapping(uint8_t note, Base_MIDI_Pulse* d);
			
			/// @brief Removes an existing mapping for the given note if it exists
			/// @param note MIDI note to remove the mapping from
			void deleteMapping(uint8_t note);
			
			/// @brief Gets the device mapped to the given note if there is one
			/// @param note MIDI note to get the device for
			Base_MIDI_Pulse* const getMappedDevice(uint8_t note);
			
			/// @brief Pulses the Base_MIDI_Pulse instanced mapped to the note if there is one
			/// @param note MIDI note to attempt pulsing
			void pulseNote(uint8_t note);
			
			/// @brief Stops the Base_MIDI_Pulse instanced mapped to the note if there is one
			/// @param note MIDI note to attempt stopping
			void stopNote(uint8_t note);
			
			/// @brief Stops all mapped notes
			void stopNotes();
	};
	
	/// @brief Global singleton instance of MIDI_Pulse_Mapper
	inline MIDI_Pulse_Mapper MPM = MIDI_Pulse_Mapper::getInstance();
	
#endif
