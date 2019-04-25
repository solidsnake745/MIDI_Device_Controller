#ifndef DeviceNode_h
	#define DeviceNode_h

	//#include "DebugPrint.h"
	#include <Arduino.h>
	
	class Device;
	class DeviceChain_Base;

	struct DeviceNode
	{
		friend class DeviceChain_Base;
		
		private:
			DeviceNode(Device *d, DeviceChain_Base *dc);
			DeviceChain_Base *parent = NULL;		
			
		public:
			//Devices exist and are stored in MIDI_DeviceController
			Device *device = NULL;
			DeviceNode *next = NULL;
			DeviceNode *prev = NULL;
			
			//Millisecond timestamp when this was last assigned a note
			// uint32_t lastAssignStamp = 0;
			
			void assignNote(uint8_t note);
			void clearNote();
			bool tryAssign(uint8_t note);
			bool tryClear(uint8_t note);
	};
#endif