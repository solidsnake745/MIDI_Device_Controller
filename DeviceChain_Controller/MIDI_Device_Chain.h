#ifndef MIDI_Device_Chain_h
	#define MIDI_Device_Chain_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller/MIDI_Device.h"
	#include "../SerialDebug.h"

	struct MIDI_Device_Node;

	//A doubly linked list of devices used for pooling note assignments
	class MIDI_Device_Chain
	{
		//Give MIDI_Device_Node access to all private members
		friend class MIDI_Device_Node;
		
		void deleteNode(MIDI_Device_Node *node);
		
		protected:
			MIDI_Device_Node *start = NULL;
			MIDI_Device_Node *end = NULL;
			uint8_t count = 0;
			static SerialDebug _debug;
			
		public:
			virtual ~MIDI_Device_Chain();
			
			void addDevice(MIDI_Device *d);
			void addDevices(MIDI_Device devices[], uint16_t numDevices); //Adds the given set of devices
			void removeDevice(uint8_t id);
			
			//Overrideable methods
			virtual void printStatus();
			virtual void assignNote(uint8_t note);
			virtual void clearNote(uint8_t note);
			virtual void pitchBend(uint16_t bend);
	};
#endif