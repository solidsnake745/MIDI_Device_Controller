#ifndef DeviceChain_Base_h
#define DeviceChain_Base_h

#include "../MIDI_DeviceController/Device.h"
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
	
	public:
		virtual ~DeviceChain_Base();
		
		void addDevice(Device *d);
		void addDevices(Device devices[], int16_t numDevices = -1); //Adds the given set of devices
		void removeDevice(uint8_t id);
		
		//Overrideable methods
		virtual void printStatus();
		virtual void assignNote(uint8_t note);
		virtual void clearNote(uint8_t note);
		virtual void pitchBend(uint16_t bend);
};

#endif