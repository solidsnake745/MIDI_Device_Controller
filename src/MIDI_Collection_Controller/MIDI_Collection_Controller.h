#ifndef MIDI_Collection_Controller_h
	#define MIDI_Collection_Controller_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Devices/MIDI_Pitch.h"
	#include "Base_MIDI_Pitch_Collection.h"

	///[MCC] Controls and manages collections of MIDI Pitch objects
	class MIDI_Collection_Controller
	{
		// Give Device access to all private members
		friend class MIDI_Pitch;
		
		static SerialDebug _debug;
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			MIDI_Collection_Controller();
			static MIDI_Collection_Controller *_instance;
			
		public:
			//Used to populate our single instance MDC for consumption
			/// @private
			static MIDI_Collection_Controller &getInstance();
			
		//Device management
		//_______________________________________________________________________________________________________
		private:
			static Base_MIDI_Pitch_Collection *_collections[MAX_COLLECTIONS];

		public:
			///Prints status information about this controller to Serial
			void printStatus();
			
			///Adds a collection to the controller
			/*!
				\param index Index to assign the collection to
				\param c Collection to be added
			*/
			void addCollection(uint8_t index, Base_MIDI_Pitch_Collection *c);
			
			///Retrieves a collection from the controller
			/*!
				\param index Index to retrieve the collection from
			*/
			Base_MIDI_Pitch_Collection *getCollection(uint8_t index);
			
			///Deletes a collection from the controller
			/*!
				\param index Index to try deleting the collection from
			*/
			void deleteCollection(uint8_t index);

			///Plays the given note on a collection
			/*!
				\param index Index to play the note on
				\param note MIDI note to play
			*/
			void playNote(int8_t index, uint8_t note);
			
			///Pitch bends all active notes in a collection
			/*!
				\param index Index to bend the notes of
				\param bend Value to pitch bend by
			*/
			void bendNote(int8_t index, uint16_t bend);
			
			///Stops the given note on a collection
			/*!
				\param index Index to stop the note on
				\param note MIDI note to stop
			*/
			void stopNote(int8_t index, uint8_t note);
	};

	//Defines a global instance of our class for users to consume
	extern MIDI_Collection_Controller MCC;
#endif
