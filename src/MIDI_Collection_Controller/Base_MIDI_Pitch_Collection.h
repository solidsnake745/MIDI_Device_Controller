#ifndef Base_MIDI_Device_Collection_h
	#define Base_MIDI_Device_Collection_h
	
	#include "../Common/PitchBend.h"
	#include "../MIDI_Pitch/Base_MIDI_Pitch.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"	

	struct MIDI_Pitch_Node;

	///Base implementation of a collection of MIDI Pitch devices for distributing notes
	class Base_MIDI_Pitch_Collection
	{
		//Give MIDI_Pitch_Node access to all private members
		friend class MIDI_Pitch_Node;
		
		void deleteNode(MIDI_Pitch_Node* node);
		
		protected:
			inline static SerialDebug _debug = SerialDebug(DEBUG_BASEPITCHCOLLECTION);
			MIDI_Pitch_Node* start = NULL;
			MIDI_Pitch_Node* end = NULL;
			uint8_t _count = 0;			
			
		public:
			virtual ~Base_MIDI_Pitch_Collection();
			
			///Adds a given MIDI_Pitch device to this collection
			/*!
				\param d Pitch device to be added
			*/
			void addDevice(Base_MIDI_Pitch* d);
			
			///Removes a given MIDI_Pitch device by ID from this collection
			/*!
				\param id The ID of the MIDI_Pitch to attempt removing
			*/
			void removeDevice(uint8_t id);
			
			//Overrideable methods
			inline virtual void reset() {};
			
			///Prints status information about this collection to Serial
			virtual void printStatus();
			
			///Plays the given note
			virtual bool playNote(uint8_t note);			
			
			///Pitch bend all active notes in the collection
			virtual void bendNote(int16_t bend, bool shiftRange = false);
			
			///Stops the given note
			virtual void stopNote(uint8_t note);
			
			//Test methods
			
			///Tests pitch bending all devices in the collection
			void testPitchBend();
	};
#endif