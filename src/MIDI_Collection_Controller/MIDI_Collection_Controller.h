#ifndef MIDI_Collection_Controller_h
	#define MIDI_Collection_Controller_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Pitch/Base_MIDI_Pitch.h"
	#include "Base_MIDI_Pitch_Collection.h"

	/// @brief Stores and provides Base_MIDI_Pitch_Collection instances
	/// @details Used to hold Base_MIDI_Pitch_Collection instances and simplify their usage.<br>
	/// TODO: Refactor and consolidate MIDI_Collection_Factory into this?
	class MIDI_Collection_Controller
	{
		// Give Device access to all private members
		friend class Base_MIDI_Pitch;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_COLLECTIONCONTROLLER);
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			MIDI_Collection_Controller() = default; //Disallow creating an instance
			inline static MIDI_Collection_Controller* _instance = nullptr;
			
		public:
			/// @private
			/// @brief Used to populate our single instance MCC for consumption
			static MIDI_Collection_Controller &getInstance();
			
		//Collection management
		//_______________________________________________________________________________________________________
		private:
			inline static Base_MIDI_Pitch_Collection* _collections[MAX_COLLECTIONS];

		public:
			/// @brief Prints status information about this controller
			void printStatus();
			
			/// @brief Adds a collection to this controller
			/// @param index Index to assign the collection to
			/// @param c Collection to be added
			void addCollection(uint8_t index, Base_MIDI_Pitch_Collection* c);
			
			/// @brief Gets a collection from this controller
			/// @param index Index to get the collection from
			/// @retval nullptr if no collection at the index or index is out of range
			Base_MIDI_Pitch_Collection* getCollection(uint8_t index);
			
			//Really not supporting delete at this time as it's really unnecessary
			//Commenting them out so it doesn't seem like you are able to
			
			// /// @brief Deletes a collection from this controller
			// /// @param index Index to try deleting the collection from
			// void deleteCollection(uint8_t index);

		//Collection operation
		//_______________________________________________________________________________________________________
		public:		
			///
			/*!
				\param index Index of the collection to play the note on
				\param note MIDI note to play
			*/
			
			/// @brief Attempts to play the specified note on the specified collection
			/// @param index Index of the collection to play the note on
			/// @param note MIDI note to play
			void playNote(uint8_t index, uint8_t note);
			
			/// @brief Pitch bends all active notes in the specified collection
			/// @param index Index of the collection to bend the notes of
			/// @param bend MIDI pitch bend value to bend by
			/// @param shiftRange When true, shifts incoming bend values down from 0 to 16383 to -8192 to 8191
			void bendNote(uint8_t index, int16_t bend, bool shiftRange = false);
			
			/// @brief Attempts to stops the given note on the specified collection
			/// @param index Index of the collection to stop the note on
			/// @param note MIDI note to stop
			void stopNote(uint8_t index, uint8_t note);
			
			/// @brief Attempts to stops the given note on the specified collection
			/// @param index Index of the collection to stop the note on
			/// @param note MIDI note to stop
			void reset(uint8_t index);
			
			///Resets all collections if they can be
			void resetAll();
	};

	//Defines a global instance of our class for users to consume
	inline MIDI_Collection_Controller MCC = MIDI_Collection_Controller::getInstance();
	
#endif