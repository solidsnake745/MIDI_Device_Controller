#ifndef MIDI_Handler_h
	#define MIDI_Handler_h

	#include "../MIDI_Enums/MIDI_Enums.h"
	#include "../MIDI_Message/MIDI_Message.h"
	#include "../SerialDebug/SerialDebug.h"
	
	class MIDI_Handler
	{
		typedef void (*NoteOff) (uint8_t, uint8_t, uint8_t);
		typedef void (*NoteOn) (uint8_t, uint8_t, uint8_t);
		typedef void (*PolyPressure) (uint8_t, uint8_t, uint8_t);
		typedef void (*ControlChange) (uint8_t, uint8_t, uint8_t);
		typedef void (*ProgramChange) (uint8_t, uint8_t, uint8_t);
		typedef void (*ChannelPressure) (uint8_t, uint8_t, uint8_t);
		typedef void (*PitchBend) (uint8_t, uint16_t);

		NoteOff _noteOff = NULL;
		NoteOn _noteOn = NULL;  
		PolyPressure _polyPressure = NULL;  
		ControlChange _controlChange = NULL;
		ProgramChange _programChange = NULL;
		ChannelPressure _channelPressure = NULL;
		PitchBend _pitchBend = NULL;

		//static SerialDebug _debug;
		
		public:
			void handleMIDI(MIDI_Message msg);
			
			void setNoteOffHandle(NoteOff handle);
			void setNoteOnHandle(NoteOn handle);
			void setPitchBendHandle(PitchBend handle);
	};

#endif