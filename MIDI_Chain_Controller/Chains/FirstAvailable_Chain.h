#ifndef FirstAvailable_Chain_h
	#define FirstAvailable_Chain_h

	#include "../../MIDI_Device_Controller/MIDI_Device.h"
	#include "../MIDI_Device_Chain.h"

	struct MIDI_Device_Node;

	//First available 
	class FirstAvailable_Chain : public MIDI_Device_Chain
	{	
		private:
		
		public:		
			//Abstract implementations		
			bool assignNote(uint8_t note);
			void clearNote(uint8_t note);
	};
	
#endif