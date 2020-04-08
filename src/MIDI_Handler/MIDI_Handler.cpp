#include "MIDI_Handler.h"

void MIDI_Handler::handleMIDI(MIDI_Message msg)
{
	switch(msg.getType())
	{
		case MsgType::NoteOff: 
			if(_noteOff) 
				_noteOff(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MsgType::NoteOn: 
			if(_noteOn) 
				_noteOn(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MsgType::PolyPressure: 
			if(_polyPressure) 
				_polyPressure(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MsgType::ControlChange: 
			if(_controlChange) 
				_controlChange(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MsgType::ProgramChange: 
			if(_programChange) 
				_programChange(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MsgType::ChannelPressure: 
			if(_channelPressure) 
				_channelPressure(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MsgType::PitchBend: 
			if(_pitchBend) 
				_pitchBend(msg.getChannel(), msg.getBendValue());
			break;
		default: break;
	}
};

void MIDI_Handler::setNoteOffHandle(NoteOff handle) { _noteOff = handle; };
void MIDI_Handler::setNoteOnHandle(NoteOn handle) { _noteOn = handle; };
void MIDI_Handler::setPitchBendHandle(PitchBend handle) { _pitchBend = handle; };