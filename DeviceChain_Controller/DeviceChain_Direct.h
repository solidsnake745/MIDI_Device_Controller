#ifndef DeviceChain_Direct_h
	#define DeviceChain_Direct_h

	#include "../MIDI_DeviceController/Device.h"
	#include "DeviceChain_Base.h"

	struct DeviceNode;

	//Directly assigns the given note to all devices in the chain
	class DeviceChain_Direct : public DeviceChain_Base
	{	
		private:
		
		public:		
			//Abstract implementations		
			void assignNote(int8_t note);
			void clearNote(int8_t note);
	};

	//First available 
	class DeviceChain_FA : public DeviceChain_Base
	{	
		private:
		
		public:		
			//Abstract implementations		
			void assignNote(int8_t note);
			void clearNote(int8_t note);
	};

	class DeviceChain_RR : public DeviceChain_Base
	{	
		private:
			DeviceNode *lastAssign;
			
		public:
			//Abstract implementations		
			void assignNote(int8_t note);
			void clearNote(int8_t note);
	};
#endif