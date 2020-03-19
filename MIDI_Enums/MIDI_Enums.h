#ifndef MIDI_Enums_h
	#define MIDI_Enums_h
	
	//Standard MIDI message types
	//Message types range between 0-15 (not all values currently in use)
	//Limited to 4 bit values though stored in 8 bit field
	enum class MsgType
	{
		Undefined = 0,
		NoteOff = 8,
		NoteOn = 9,
		PolyPressure = 10,
		ControlChange = 11,
		ProgramChange = 12,
		ChannelPressure = 13,
		PitchBend = 14
	};
	
#endif
