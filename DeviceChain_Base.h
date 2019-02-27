#ifndef DeviceChain_Base_h
#define DeviceChain_Base_h

#include "Device.h"

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
		void add(Device *d);
		void remove(uint8_t id);
		
		//Overrideable methods
		virtual void printStatus();
		virtual void assignNote(uint8_t note);
		virtual void clearNote(uint8_t note);
		virtual void pitchBend(uint16_t bend);
};

#endif