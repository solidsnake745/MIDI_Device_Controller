#ifndef DeviceNode_h
	#define DeviceNode_h

	#include <Arduino.h>
	#include "../SerialDebug/SerialDebug.h"
	
	class MIDI_Device;
	class Base_MIDI_Device_Collection;

	struct MIDI_Device_Node
	{
		friend class Base_MIDI_Device_Collection;
		
		static SerialDebug _debug;
		
		private:
			MIDI_Device_Node(MIDI_Device *d, Base_MIDI_Device_Collection *dc);
			Base_MIDI_Device_Collection *parent = NULL;		
			
		public:
			//Devices exist and are stored in MIDI_DeviceController
			MIDI_Device *device = NULL;
			MIDI_Device_Node *next = NULL;
			MIDI_Device_Node *prev = NULL;
			
			//Millisecond timestamp when this was last assigned a note
			// uint32_t lastAssignStamp = 0;
			
			void assignNote(uint8_t note);
			void clearNote();
			bool tryAssign(uint8_t note);
			bool tryClear(uint8_t note);
	};
#endif