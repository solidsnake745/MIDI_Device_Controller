#ifndef MIDI_Device_Chains_h
	#define MIDI_Device_Chains_h

	#include "../../MIDI_Device_Controller/MIDI_Device.h"
	#include "../MIDI_Device_Chain.h"

	struct MIDI_Device_Node;

	//Directly assigns the given note to all devices in the chain
	class Direct_Chain : public MIDI_Device_Chain
	{	
		private:
		
		public:		
			//Abstract implementations		
			bool assignNote(uint8_t note);
			void clearNote(uint8_t note);
	};
	
#endif