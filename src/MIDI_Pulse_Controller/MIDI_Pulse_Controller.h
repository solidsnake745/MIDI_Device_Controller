#ifndef MIDI_Pulse_Controller_h
	#define MIDI_Pulse_Controller_h
	
	#include "../MIDI_Device_Controller.h"
	#include "../SerialDebug/SerialDebug.h"
	#include "../MIDI_Device_Controller.h"
	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	
	//Resolve STL dependency
	#if defined(ARDUINO_ARCH_AVR)
		//#include "../Common/ArduinoSTLClone/ArduinoSTL.h"
		#include "../Common/ArduinoSTLClone/map"
		//#include <ArduinoSTL.h>
		//#include "map"
	#elif defined(CORE_TEENSY)
		#include <map>
	#elif defined(ESP32)
		#include <map>
	#endif
			
	///[MPC] Manages mapping MIDI notes to the outputs of pulse devices
	class MIDI_Pulse_Controller
	{
		static SerialDebug _debug;
		
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
	
		std::map<uint8_t, mapEntry*> _noteMap;
		
		uint32_t _defaultDuration = 5000;
		
		public:
			//Used to populate our single instance MDF for consumption
			/// @private
			static MIDI_Pulse_Controller &getInstance();			
			
			inline void setDefaultDuration(uint32_t limit) { _defaultDuration = limit; };
			
			///Adds a mapping between note and pulse device output
			/*!
				\param note MIDI note to map
				\param device IO device to map to
				\param output Output of the device to map to
			*/
			void addMapping(uint8_t note, IO_DeviceEnum device, uint8_t output);
			
			void deleteMapping(uint8_t note);			
			
			void pulseNote(uint8_t note);
			void stopNote(uint8_t note);
			void stopNotes();
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Pulse_Controller MPC;
#endif
