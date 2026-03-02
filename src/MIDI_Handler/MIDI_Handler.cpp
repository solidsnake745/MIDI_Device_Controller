#include "MIDI_Handler.h"

void MIDI_Handler::handleMIDI(MIDI_Message msg)
{
	switch(msg.getType())
	{
		case MIDI_MsgType::NoteOff:
			noteOff(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MIDI_MsgType::NoteOn:
			noteOn(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MIDI_MsgType::PolyPressure:
			polyPressure(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MIDI_MsgType::ControlChange:
			controlChange(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MIDI_MsgType::ProgramChange:
			programChange(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MIDI_MsgType::ChannelPressure:
			channelPressure(msg.getChannel(), msg.getData1(), msg.getData2());
			break;
		case MIDI_MsgType::PitchBend:
			pitchBend(msg.getChannel(), msg.getBendValue());
			break;
		default: break;
	}
}