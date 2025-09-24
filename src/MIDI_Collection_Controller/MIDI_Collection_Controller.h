#ifndef MIDI_Collection_Controller_h
	#define MIDI_Collection_Controller_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Pitch/MIDI_Pitch.h"
	#include "Base_MIDI_Pitch_Collection.h"

	///[MCC] Controls and manages collections of MIDI Pitch objects
	class MIDI_Collection_Controller
	{
		// Give Device access to all private members
		friend class MIDI_Pitch;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_COLLECTIONCONTROLLER);
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			MIDI_Collection_Controller();
			inline static MIDI_Collection_Controller* _instance = nullptr;
			
		public:
			//Used to populate our single instance MDC for consumption
			/// @private
			static MIDI_Collection_Controller &getInstance();
			
		//Collection management
		//_______________________________________________________________________________________________________
		private:
			inline static Base_MIDI_Pitch_Collection* _collections[MAX_COLLECTIONS];

		public:
			///Prints status information about this controller to Serial
			void printStatus();
			
			///Adds a collection to the controller
			/*!
				\param index Index to assign the collection to
				\param c Collection to be added
			*/
			void addCollection(uint8_t index, Base_MIDI_Pitch_Collection* c);
			
			///Retrieves a collection from the controller
			/*!
				\param index Index to retrieve the collection from
			*/
			Base_MIDI_Pitch_Collection* getCollection(uint8_t index);
			
			///Deletes a collection from the controller
			/*!
				\param index Index to try deleting the collection from
			*/
			void deleteCollection(uint8_t index);

		//Collection operation
		//_______________________________________________________________________________________________________
		public:		
			///Plays the given note on a collection
			/*!
				\param index Index of the collection to play the note on
				\param note MIDI note to play
			*/
			void playNote(uint8_t index, uint8_t note);
			
			///Pitch bends all active notes in a collection
			/*!
				\param index Index of the collection to bend the notes of
				\param bend Value to pitch bend by (-8192 to 8191)
				\param shiftRange True indicates incoming values need to be shifted from 0 to 16383 to -8192 to 8191
			*/
			void bendNote(uint8_t index, int16_t bend, bool shiftRange = false);
			
			///Stops the given note on a collection
			/*!
				\param index Index of the collection to stop the note on
				\param note MIDI note to stop
			*/
			void stopNote(uint8_t index, uint8_t note);
			
			///Resets a given collection if it can be
			/*!
				\param index Index of the collection to reset
			*/
			void reset(uint8_t index);
			
			///Resets all collections if they can be
			void resetAll();
	};

	//Defines a global instance of our class for users to consume
	inline MIDI_Collection_Controller MCC = MIDI_Collection_Controller::getInstance();
#endif
