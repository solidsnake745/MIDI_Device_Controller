#ifndef MIDI_Collection_Factory_h
	#define MIDI_Collection_Factory_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Collection_Controller/Base_MIDI_Pitch_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/Direct_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/FirstAvailable_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/RoundRobin_Collection.h"
	#include "../SerialDebug/SerialDebug.h"
	
	enum CollectionType { Direct, FirstAvailable, RoundRobin };
	
	///[MCF] Simplifies creating MIDI Pitch collections and associating them to the collection controller
	class MIDI_Collection_Factory
	{
		static SerialDebug _debug;
		
		//Constructor(s)
		MIDI_Collection_Factory(); //Disallow creating an instance
		static MIDI_Collection_Factory *_instance;
		
		//Creates a new collection and adds it to the controller (MCC)
		Base_MIDI_Pitch_Collection *createInitialCollection(uint8_t index, CollectionType type);
		
		MIDI_Pitch *getDeviceFromMDC(uint8_t index);
		
		//Recursive template method for populating a device Collection from an argument pack of IDs
		template<typename... IDs>
		inline void populateCollection(Base_MIDI_Pitch_Collection *c, uint8_t id, IDs... ids)
		{
			MIDI_Pitch *d = getDeviceFromMDC(id);
			if(d) c->addDevice(d);
			populateCollection(c, ids...);
		};
		
		//Recursive decay case
		inline void populateCollection(Base_MIDI_Pitch_Collection *c) { _debug.debugln(5, F("Finished populating")); };		
		
		public:
			//Used to populate our single instance MDF for consumption
			/// @private
			static MIDI_Collection_Factory &getInstance();
			
			//Used to generate device Collections and add devices by specified IDs
			//Requires at least one device ID
			
			///Creates a new collection with devices from MDC and adds it to the controller (MCC)
			/*!
				\param index Index to assign the collection to
				\param type Type of collection to create
				\param firstId ID of the first device to add from MDC
				\param ids Rest of the device IDs to be added (I.E. firstId, 1, 2, 3)
			*/
			template<typename... IDs>
			inline void createCollection(uint8_t index, CollectionType type, uint8_t firstId, IDs... ids)
			{
				Base_MIDI_Pitch_Collection *newCollection = createInitialCollection(index, type);
				
				if(newCollection)
				{
					_debug.println(F("Populating Collection"));
					populateCollection(newCollection, firstId, ids...);
				}
				else
					_debug.println(F("Failed to create new Collection"));
			};
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Collection_Factory MCF;
#endif