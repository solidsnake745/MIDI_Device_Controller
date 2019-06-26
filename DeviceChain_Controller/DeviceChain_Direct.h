#ifndef DeviceChain_Direct_h
	#define DeviceChain_Direct_h

	#include "../MIDI_Device_Controller/MIDI_Device.h"
	#include "DeviceChain_Base.h"

	struct MIDI_Device_Node;

	//Directly assigns the given note to all devices in the chain
	class DeviceChain_Direct : public DeviceChain_Base
	{	
		private:
		
		public:		
			//Abstract implementations		
			void assignNote(uint8_t note);
			void clearNote(uint8_t note);
	};

	//First available 
	class DeviceChain_FA : public DeviceChain_Base
	{	
		private:
		
		public:		
			//Abstract implementations		
			void assignNote(uint8_t note);
			void clearNote(uint8_t note);
	};

	class DeviceChain_RR : public DeviceChain_Base
	{	
		private:
			MIDI_Device_Node *lastAssign;
			
		public:
			//Abstract implementations		
			void assignNote(uint8_t note);
			void clearNote(uint8_t note);
	};
#endif