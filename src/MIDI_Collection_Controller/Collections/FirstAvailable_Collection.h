#ifndef FirstAvailable_Collection_h
	#define FirstAvailable_Collection_h

	#include "../../MIDI_Device_Controller/MIDI_Device.h"
	#include "../Base_MIDI_Device_Collection.h"

	struct MIDI_Device_Node;

	//First available 
	class FirstAvailable_Collection : public Base_MIDI_Device_Collection
	{	
		private:
		
		public:		
			//Abstract implementations		
			bool assignNote(uint8_t note);
			void clearNote(uint8_t note);
	};
	
#endif