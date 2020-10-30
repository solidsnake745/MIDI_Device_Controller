#ifndef Base_MIDI_Device_Collection_h
	#define Base_MIDI_Device_Collection_h

	#include "../Settings.h"
	#include "../MIDI_Devices/MIDI_Pitch.h"
	#include "../SerialDebug/SerialDebug.h"

	struct MIDI_Pitch_Node;

	///Base implementation of a collection of MIDI Pitch devices for distributing notes
	class Base_MIDI_Pitch_Collection
	{
		//Give MIDI_Pitch_Node access to all private members
		friend class MIDI_Pitch_Node;
		
		void deleteNode(MIDI_Pitch_Node *node);
		
		protected:
			MIDI_Pitch_Node *start = NULL;
			MIDI_Pitch_Node *end = NULL;
			uint8_t _count = 0;
			static SerialDebug _debug;
			
		public:
			virtual ~Base_MIDI_Pitch_Collection();
			
			///Adds a given MIDI_Pitch device to this collection
			/*!
				\param d Pitch device to be added
			*/
			void addDevice(MIDI_Pitch *d);
			
			///Removes a given MIDI_Pitch device by ID from this collection
			/*!
				\param id The ID of the MIDI_Pitch to attempt removing
			*/
			void removeDevice(uint8_t id);
			
			//Overrideable methods
			
			///Prints status information about this collection to Serial
			virtual void printStatus();
			
			///Plays the given note
			virtual bool playNote(uint8_t note);			
			
			///Pitch bend all active notes in the collection
			virtual void bendNote(uint16_t bend);
			
			///Stops the given note
			virtual void stopNote(uint8_t note);
			
			//Test methods
			
			///Tests pitch bending all devices in the collection
			void testPitchBend();
	};
#endif