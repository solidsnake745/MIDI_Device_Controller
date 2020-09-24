#ifndef FirstAvailable_Collection_h
	#define FirstAvailable_Collection_h

	#include "../../MIDI_Device_Controller/MIDI_Pitch.h"
	#include "../Base_MIDI_Pitch_Collection.h"

	//Forward declaration for compiling
	struct MIDI_Pitch_Node;

	///Assigns notes to the first available device in the collection
	class FirstAvailable_Collection : public Base_MIDI_Pitch_Collection
	{	
		private:
		
		public:		
			//Abstract implementations		
			bool playNote(uint8_t note);
			void stopNote(uint8_t note);
	};
	
#endif