#ifndef MIDI_Collection_Factory_h
	#define MIDI_Collection_Factory_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Collection_Controller/Base_MIDI_Device_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/Direct_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/FirstAvailable_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/RoundRobin_Collection.h"
	#include "../SerialDebug/SerialDebug.h"
	
	enum CollectionType { Direct, FirstAvailable, RoundRobin };
	
	class MIDI_Collection_Factory
	{
		static SerialDebug _debug;
		
		//Constructor(s)
		MIDI_Collection_Factory(); //Disallow creating an instance
		static MIDI_Collection_Factory *_instance;
		
		//Creates a new Collection and adds it to the controller (MCC)		
		Base_MIDI_Device_Collection *createInitialCollection(uint8_t index, CollectionType type);
		
		MIDI_Device *getDeviceFromMDC(uint8_t index);
		
		//Recursive template method for populating a device Collection from an argument pack of IDs
		template<typename... IDs>
		inline void populateCollection(Base_MIDI_Device_Collection *c, uint8_t id, IDs... ids)
		{
			c->addDevice(getDeviceFromMDC(id));
			populateCollection(c, ids...);
		};
		
		//Recursive decay case
		inline void populateCollection(Base_MIDI_Device_Collection *c) { _debug.debugln(5, "Finished populating"); };		
		
		public:
			//Used to populate our single instance MDF for consumption
			static MIDI_Collection_Factory &getInstance();
			
			//Used to generate device Collections and add devices by specified IDs
			//Requires at least one device ID
			template<typename... IDs>
			inline void createCollection(uint8_t index, CollectionType type, uint8_t firstId, IDs... ids)
			{
				Base_MIDI_Device_Collection *newCollection = createInitialCollection(index, type);
				
				if(newCollection)
				{
					_debug.println("Populating Collection");
					populateCollection(newCollection, firstId, ids...);
				}
				else
					_debug.println("Failed to create new Collection");
			};
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Collection_Factory MCF;
#endif