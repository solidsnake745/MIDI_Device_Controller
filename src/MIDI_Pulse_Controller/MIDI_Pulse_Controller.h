#ifndef MIDI_Pulse_Controller_h
	#define MIDI_Pulse_Controller_h
	
	#include "../MIDI_Device_Controller.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	#include "../MIDI_Device_Controller.h"
	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	
	//Resolve map dependency
	#if ARDUINO_ARCH_AVR
		#include "../../ArduinoSTLClone/map"
	#elif defined(CORE_TEENSY)
		#include <map>
	#elif ARDUINO_ARCH_ESP32
		#include <map>
	#endif
			
	///[MPC] Manages mapping MIDI notes to the outputs of pulse devices
	class MIDI_Pulse_Controller
	{
		inline static SerialDebug _debug = SerialDebug(DEBUG_PULSECONTROLLER);
		
		//Constructor(s)
		MIDI_Pulse_Controller(); //Disallow creating an instance
		static MIDI_Pulse_Controller *_instance;
		
		struct mapEntry
		{
			mapEntry() {};
			mapEntry(IO_Device* d, uint8_t o) 
			{
				device = d;
				out = o;
			};
			
			IO_Device* device = NULL;
			uint8_t out;
		};
	
		static std::map<uint8_t, mapEntry*> _noteMap;
		
		uint32_t _defaultDuration = 5000;
		
		public:
			//Used to populate our single instance MDF for consumption
			/// @private
			static MIDI_Pulse_Controller &getInstance();			
			
			inline void setDefaultDuration(uint32_t limit) { _defaultDuration = limit; };
			
			///Adds a mapping between note and pulse device output
			/*!
				\param note MIDI note to map
				\param type IO device to map to
				\param output Output of the device to map to
			*/
			void addMapping(uint8_t note, IOType type, uint8_t output);
			
			void deleteMapping(uint8_t note);			
			
			void pulseNote(uint8_t note);
			void toggleNote(uint8_t note);
			void stopNote(uint8_t note);
			void stopNotes();
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Pulse_Controller MPC;
#endif
