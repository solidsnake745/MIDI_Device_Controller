#ifndef RoundRobin_Chain_h
	#define RoundRobin_Chain_h

	#include "../../MIDI_Device_Controller/MIDI_Device.h"
	#include "../MIDI_Device_Chain.h"

	struct MIDI_Device_Node;

	class RoundRobin_Chain : public MIDI_Device_Chain
	{	
		private:
			MIDI_Device_Node *lastAssign;
			
		public:
			//Abstract implementations		
			bool assignNote(uint8_t note);
			void clearNote(uint8_t note);
	};
	
#endif