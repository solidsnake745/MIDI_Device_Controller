#ifndef MIDI_Handler_h
	#define MIDI_Handler_h

	#include "MIDI_Enums.h"
	#include "MIDI_Message.h"
	
	class MIDI_Handler
	{
		//MIDI handler method definitions
		typedef void (*NoteOff) (uint8_t, uint8_t, uint8_t);
		typedef void (*NoteOn) (uint8_t, uint8_t, uint8_t);
		typedef void (*PolyPressure) (uint8_t, uint8_t, uint8_t);
		typedef void (*ControlChange) (uint8_t, uint8_t, uint8_t);
		typedef void (*ProgramChange) (uint8_t, uint8_t, uint8_t);
		typedef void (*ChannelPressure) (uint8_t, uint8_t, uint8_t);
		typedef void (*PitchBend) (uint8_t, int);

		//Pointers to handlers, defaults to null to indicate no handler set
		NoteOff _noteOff = NULL;
		NoteOn _noteOn = NULL;  
		PolyPressure _polyPressure = NULL;  
		ControlChange _controlChange = NULL;
		ProgramChange _programChange = NULL;
		ChannelPressure _channelPressure = NULL;
		PitchBend _pitchBend = NULL;
		
		public:
			//Handles MIDI_Messages based on type
			inline void handleMIDI(MIDI_Message msg)
			{
				switch(msg.getType())
				{
					case MsgType::NoteOff:
						noteOff(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MsgType::NoteOn:
						noteOn(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MsgType::PolyPressure:
						polyPressure(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MsgType::ControlChange:
						controlChange(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MsgType::ProgramChange:
						programChange(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MsgType::ChannelPressure:
						channelPressure(msg.getChannel(), msg.getData1(), msg.getData2());
						break;
					case MsgType::PitchBend:
						pitchBend(msg.getChannel(), msg.getBendValue());
						break;
					default: break;
				}
			};
			
			//Methods for setting handlers
			inline void setNoteOffHandle(NoteOff handle) { _noteOff = handle; };
			inline void setNoteOnHandle(NoteOn handle) { _noteOn = handle; };
			inline void setPolyPressureHandle(PolyPressure handle) { _polyPressure = handle; };
			inline void setControlChangeHandle(ControlChange handle) { _controlChange = handle; };
			inline void setProgramChangeHandle(ProgramChange handle) { _programChange = handle; };
			inline void setChannelPressureHandle(ChannelPressure handle) { _channelPressure = handle; };
			inline void setPitchBendHandle(PitchBend handle) { _pitchBend = handle; };
			
			//Direct access to calling handler methods if needed
			//Abbreviations:
			//ch = channel, n = note, v = velocity, p = pressure, cn = controller number,
			//d = data, pn = program number, u = unused, b = bend
			inline void noteOff(uint8_t ch, uint8_t n, uint8_t v) { if(_noteOff)  _noteOff(ch, n, v); };
			inline void noteOn(uint8_t ch, uint8_t n, uint8_t v) { if(_noteOn)  _noteOn(ch, n, v); };
			inline void polyPressure(uint8_t ch, uint8_t n, uint8_t p) { if(_polyPressure)  _polyPressure(ch, n, p); };
			inline void controlChange(uint8_t ch, uint8_t cn, uint8_t d) { if(_controlChange)  _controlChange(ch, cn, d); };
			inline void programChange(uint8_t ch, uint8_t pn, uint8_t u) { if(_programChange)  _programChange(ch, pn, u); };
			inline void channelPressure(uint8_t ch, uint8_t p, uint8_t u) { if(_channelPressure)  _channelPressure(ch, p, u); };
			inline void pitchBend(uint8_t ch, int b) { if(_pitchBend)  _pitchBend(ch, b); };
	};

#endif