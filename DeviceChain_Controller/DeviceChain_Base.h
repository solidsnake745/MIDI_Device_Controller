#ifndef DeviceChain_Base_h
	#define DeviceChain_Base_h

	#include "../Settings.h"
	#include "../MIDI_DeviceController/MIDI_Device.h"
	#include "../SerialDebug.h"

	struct DeviceNode;

	//A doubly linked list of devices used for pooling note assignments
	class DeviceChain_Base
	{
		//Give DeviceNode access to all private members
		friend class DeviceNode;
		
		void deleteNode(DeviceNode *node);
		
		protected:
			DeviceNode *start = NULL;
			DeviceNode *end = NULL;
			uint8_t count = 0;
			static SerialDebug _debug;
			
		public:
			virtual ~DeviceChain_Base();
			
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