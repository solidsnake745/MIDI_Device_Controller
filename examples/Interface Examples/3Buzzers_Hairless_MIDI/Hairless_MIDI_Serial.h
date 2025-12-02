#ifndef Hairless_MIDI_Serial_h
	#define Hairless_MIDI_Serial_h

	#include <Arduino.h>
	#include <MIDI_Handler\MIDI_Handler.h>
	
	class Hairless_MIDI_Serial
	{
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			Hairless_MIDI_Serial();
			inline static Hairless_MIDI_Serial* _instance = nullptr;
			
		public:
			//Used to populate our single instance MS for consumption
			/// @private
			static Hairless_MIDI_Serial& getInstance();
			
		//Serial Handling
		//_______________________________________________________________________________________________________
		private:
			MIDI_Message parseSerial();
		
		public:	
			void process();
		
		//MIDI Handling
		//_______________________________________________________________________________________________________
		private:
			MIDI_Handler _midiHandler;

		public:
			inline void setNoteOnHandle(void (*noteOn)(uint8_t, uint8_t, uint8_t)) { _midiHandler.setNoteOnHandle(noteOn); };
			inline void setNoteOffHandle(void (*noteOff)(uint8_t, uint8_t, uint8_t)) { _midiHandler.setNoteOffHandle(noteOff); };
			inline void setControlChangeHandle(void (*controlChange)(uint8_t, uint8_t, uint8_t)) { _midiHandler.setControlChangeHandle(controlChange); };
			inline void setPitchBendHandle(void (*pitchBend)(uint8_t, int)) { _midiHandler.setPitchBendHandle(pitchBend); };
	};

	//Defines a global instance of our class for users to consume
	inline Hairless_MIDI_Serial HMS = Hairless_MIDI_Serial::getInstance();;
#endif