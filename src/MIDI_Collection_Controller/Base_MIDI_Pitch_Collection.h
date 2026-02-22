#ifndef Base_MIDI_Device_Collection_h
	#define Base_MIDI_Device_Collection_h
	
	#include "MIDI_Pitch_Node.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"	
	#include "../Common/PitchBend.h"
	#include "../MIDI_Pitch/Base_MIDI_Pitch.h"
	
	/// @brief Base definition/implementation of a collection of MIDI Pitch devices for distributing notes
	/// @details Current implementations can be found in CollectionType.
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
			virtual ~Base_MIDI_Pitch_Collection() = default;
			
			/// @brief Adds the specified Base_MIDI_Pitch device to this collection
			/// @param d Device to add
			void addDevice(Base_MIDI_Pitch* d);
			
			/// @brief Removes a given Base_MIDI_Pitch device by ID from this collection
			/// @param id The ID of the Base_MIDI_Pitch to attempt removing
			void removeDevice(uint8_t id);
			
			//Overridable methods
			
			/// @brief Resets the collection if applicable
			virtual void reset();
			
			/// @brief Prints status information about this collection to Serial
			virtual void printStatus();
			
			/// @brief Attempts to play the given note based on the collection type
			/// @param note MIDI note to play
			/// @retval true if assignment was successful
			virtual bool playNote(uint8_t note);
			
			/// @brief Pitch bend all active notes in the collection
			/// @param bend MIDI pitch bend value to bend by
			/// @param shiftRange When true, shifts incoming bend values down from 0 to 16383 to -8192 to 8191
			virtual void bendNote(int16_t bend, bool shiftRange = false);
			
			/// @brief Stops the given note
			/// @param note MIDI note to stop
			virtual void stopNote(uint8_t note);
			
			//Test methods
			
			/// @brief Tests pitch bending all devices in the collection
			void testPitchBend();
	};
	
#endif