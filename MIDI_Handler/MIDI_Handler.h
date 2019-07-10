#ifndef MIDI_Handler_h
	#define MIDI_Handler_h

	#include "../MIDI_Message/MIDI_Message.h"

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

		public:
			inline void handleMIDI(MIDI_Message msg)
			{
				switch(msg.getStatus())
				{
					case MIDI_MessageType_t::NoteOff: 
						if(_noteOff) 
							_noteOff(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MIDI_MessageType_t::NoteOn: 
						if(_noteOn) 
							_noteOn(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MIDI_MessageType_t::PolyPressure: 
						if(_polyPressure) 
							_polyPressure(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MIDI_MessageType_t::ControlChange: 
						if(_controlChange) 
							_controlChange(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MIDI_MessageType_t::ProgramChange: 
						if(_programChange) 
							_programChange(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MIDI_MessageType_t::ChannelPressure: 
						if(_channelPressure) 
							_channelPressure(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MIDI_MessageType_t::PitchBend: 
						if(_pitchBend) 
							_pitchBend(msg.getChannel(), msg.getBendValue());
						break;
				}
			};
			
			inline void setNoteOffHandle(NoteOff handle) { _noteOff = handle; };
			inline void setNoteOnHandle(NoteOn handle) { _noteOn = handle; };
			inline void setPitchBendHandle(PitchBend handle) { _pitchBend = handle; };
	};

#endif