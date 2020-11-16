#ifndef MIDI_Pulse_Controller_h
	#define MIDI_Pulse_Controller_h

	#include "../SerialDebug/SerialDebug.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Devices/PulseDeviceEnum.h"
	#include "../MIDI_Devices/IPulseNotes.h"
	#include "../MIDI_Devices/MIDI_SN74HC595N.h"
	#include "../MIDI_Devices/MIDI_Digital_IO.h"
	
	//Resolve STL dependency
	#if defined(ARDUINO_ARCH_AVR)
		#include "ArduinoSTL.h"
		#include "map"
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
			mapEntry(IPulseNotes* d, uint8_t o) 
			{
				device = d;
				out = o;
			};
			
			IPulseNotes* device = NULL;
			uint8_t out;
		};
	
		std::map<uint8_t, mapEntry*> _noteMap;

		public:
			//Used to populate our single instance MDF for consumption
			/// @private
			static MIDI_Pulse_Controller &getInstance();			
			
			///Adds a mapping between note and pulse device output
			/*!
				\param note MIDI note to map
				\param device Pulse device to map to
				\param output Output of the device to map to
			*/
			void addMapping(uint8_t note, PulseDevice device, uint8_t output);
			
			void deleteMapping(uint8_t note);
			
			void pulseNote(uint8_t note);
			void stopNote(uint8_t note);
			void stopNotes();
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Pulse_Controller MPC;
#endif
