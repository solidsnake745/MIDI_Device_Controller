#ifndef MIDI_Collection_Factory_h
	#define MIDI_Collection_Factory_h

	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Collection_Controller/MIDI_Collection_Controller.h"
	#include "../MIDI_Collection_Controller/Base_MIDI_Pitch_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/Direct_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/FirstAvailable_Collection.h"
	#include "../MIDI_Collection_Controller/Collections/RoundRobin_Collection.h"	
	
	/// @brief Types of collections available
	enum CollectionType 
	{ 
		/// @brief Direct_Collection
		Direct, 
		/// @brief FirstAvailable_Collection
		FirstAvailable, 
		/// @brief RoundRobin_Collection
		RoundRobin 
	};
	
	/// @brief Creates, stores, and provides Base_MIDI_Pitch_Collection instances
	/// @details Used to setup device collections for distributing notes across them.<br>
	/// TODO: Refactor and consolidate this into MIDI_Collection_Controller?
	class MIDI_Collection_Factory
	{
		inline static SerialDebug _debug = SerialDebug(DEBUG_COLLECTION_FACTORY);
		
		//Constructor(s)
		MIDI_Collection_Factory() = default; //Disallow creating an instance
		inline static MIDI_Collection_Factory* _instance = nullptr;
		
		Base_MIDI_Pitch* const getDeviceFromMDC(uint8_t index);
		
		//Recursive template method for populating a device Collection from an argument pack of IDs
		template<typename... IDs>
		inline void populateCollection(Base_MIDI_Pitch_Collection* c, uint8_t id, IDs... ids)
		{
			Base_MIDI_Pitch* d = getDeviceFromMDC(id);
			if(d) c->addDevice(d);
			populateCollection(c, ids...);
		};
		
		//Recursive decay case
		inline void populateCollection(Base_MIDI_Pitch_Collection* c) { _debug.println(F("Finished populating")); };
		
		public:
			/// @private
			/// @brief Used to populate our single instance MCF for consumption
			static MIDI_Collection_Factory& getInstance();
			
			/// @brief Creates a new empty collection and adds it to MCC
			/// @param index Index to assign the collection to
			/// @param type Type of collection to create
			Base_MIDI_Pitch_Collection* createCollection(uint8_t index, CollectionType type);
			
			/// @brief Creates a new collection with devices from MDC and adds it to MCC
			/// @param index Index to assign the collection to
			/// @param type Type of collection to create
			/// @param firstId ID of the first device to add from MDC
			/// @param ids Rest of the device IDs to be added (I.E. firstId, 1, 2, 3)
			template<typename... IDs>
			inline void createCollection(uint8_t index, CollectionType type, uint8_t firstId, IDs... ids)
			{
				Base_MIDI_Pitch_Collection* newCollection = createCollection(index, type);
				
				if(newCollection)
				{
					_debug.println(F("Populating collection from device IDs"));
					populateCollection(newCollection, firstId, ids...);
				}
				else
					_debug.println(F("Failed to create new collection"));
			};
	};
	
	/// @brief Global singleton instance of MIDI_Collection_Factory
	inline MIDI_Collection_Factory MCF = MIDI_Collection_Factory::getInstance();
	
#endif