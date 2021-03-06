#ifndef RoundRobin_Collection_h
	#define RoundRobin_Collection_h

	#include "../../MIDI_Devices/MIDI_Pitch.h"
	#include "../Base_MIDI_Pitch_Collection.h"

	//Forward declaration for compiling
	struct MIDI_Pitch_Node;

	///Assigns notes to the next available device in the collection
	class RoundRobin_Collection : public Base_MIDI_Pitch_Collection
	{	
		private:
			MIDI_Pitch_Node *lastAssign;
			
		public:
			//Abstract implementations		
			bool playNote(uint8_t note);
			void stopNote(uint8_t note);
	};
	
#endif