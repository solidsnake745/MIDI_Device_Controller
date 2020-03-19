#ifndef MIDI_Serial_h
	#define MIDI_Serial_h

	#include "../Settings.h"
	#include "../MIDI_Message/MIDI_Message.h"
	#include "../MIDI_Enums/MIDI_Enums.h"
	#include "../MIDI_Handler/MIDI_Handler.h"
	#include "../SerialDebug/SerialDebug.h"
	#include <Arduino.h>
	
	class MIDI_Serial
	{
		static SerialDebug _debug;
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			MIDI_Serial();
			static MIDI_Serial *_instance;
			
		public:
			//Used to populate our single instance MS for consumption
			static MIDI_Serial &getInstance();
			
		//Serial Handling
		//_______________________________________________________________________________________________________
		private:
			MIDI_Message parseSerial();
		
		public:	
			void read();
			
		//MIDI Handling
		//_______________________________________________________________________________________________________
		private:
			MIDI_Handler _midiHandler;

		public:
			inline void setNoteOnHandle(void (*noteOn)(uint8_t, uint8_t, uint8_t))
				{ _midiHandler.setNoteOnHandle(noteOn); };
			
			inline void setNoteOffHandle(void (*noteOff)(uint8_t, uint8_t, uint8_t))
				{ _midiHandler.setNoteOffHandle(noteOff); };
				
			inline void setPitchBendHandle(void (*pitchBend)(uint8_t, uint16_t))
				{ _midiHandler.setPitchBendHandle(pitchBend); };
	};

	//Defines a global instance of our class for users to consume
	extern MIDI_Serial MS;
#endif