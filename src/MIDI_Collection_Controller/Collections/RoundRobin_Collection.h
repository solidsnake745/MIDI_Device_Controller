#ifndef RoundRobin_Collection_h
	#define RoundRobin_Collection_h

	#include "../../MIDI_Device_Controller/MIDI_Device.h"
	#include "../Base_MIDI_Device_Collection.h"

	struct MIDI_Device_Node;

	class RoundRobin_Collection : public Base_MIDI_Device_Collection
	{	
		private:
			MIDI_Device_Node *lastAssign;
			
		public:
			//Abstract implementations		
			bool assignNote(uint8_t note);
			void clearNote(uint8_t note);
	};
	
#endif