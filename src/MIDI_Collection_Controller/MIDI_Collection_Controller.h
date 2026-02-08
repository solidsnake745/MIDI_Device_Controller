#ifndef MIDI_Collection_Controller_h
	#define MIDI_Collection_Controller_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Pitch/Base_MIDI_Pitch.h"
	#include "Base_MIDI_Pitch_Collection.h"

	///[MCC] Controls and manages collections of MIDI Pitch objects
	class MIDI_Collection_Controller
	{
		// Give Device access to all private members
		friend class Base_MIDI_Pitch;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_COLLECTIONCONTROLLER);
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			inline MIDI_Collection_Controller() {};
			inline static MIDI_Collection_Controller* _instance = nullptr;
			
		public:
			//Used to populate our single instance MDC for consumption
			/// @private
			inline static MIDI_Collection_Controller &getInstance()
			{
				//Single instance check, instantiation, and return
				if (_instance == nullptr) _instance = new MIDI_Collection_Controller();
				return *_instance;
			};
			
		//Collection management
		//_______________________________________________________________________________________________________
		private:
			inline static Base_MIDI_Pitch_Collection* _collections[MAX_COLLECTIONS];

		public:
			///Prints status information about this controller to Serial
			inline void printStatus()
			{
				int i = 0;
				while(i != MAX_COLLECTIONS)
				{
					_debug.println(F("Collection %d"), i);
					if(_collections[i])
					{
						_debug.debugln(DEBUG, F("Populated"));
						_collections[i]->printStatus();
					}
					else
					{
						_debug.println(F("Empty"));
					}
					
					_debug.println();
					delay(10);
					i++;
				}
			};
			
			///Adds a collection to the controller
			/*!
				\param index Index to assign the collection to
				\param c Collection to be added
			*/
			inline void addCollection(uint8_t index, Base_MIDI_Pitch_Collection* c)
			{
				if(index > MAX_COLLECTIONS - 1)
				{
					_debug.println(F("Can't add collection at index %d; max is %d"), index, MAX_COLLECTIONS - 1);
					return;
				}

				if(_collections[index] != nullptr)
				{		
					_debug.println(F("Collection already exists at index %d"), index);
					return;
				}
				
				_collections[index] = c;
				_debug.println(F("Collection added at index %d"), index);
			};
			
			///Retrieves a collection from the controller
			/*!
				\param index Index to retrieve the collection from
			*/
			inline Base_MIDI_Pitch_Collection* getCollection(uint8_t index)
			{
				if(index > MAX_COLLECTIONS - 1)
				{
					_debug.println(F("No collection at index %d; max is %d"), index, MAX_COLLECTIONS - 1);
					return nullptr;
				}
				
				return _collections[index];
			};
			
			///Deletes a collection from the controller
			/*!
				\param index Index to try deleting the collection from
			*/
			inline void deleteCollection(uint8_t index)
			{
				if(index > MAX_COLLECTIONS - 1)
				{
					_debug.debugln(3, F("Max index is %d"), MAX_COLLECTIONS - 1);
					return;
				}
				
				if(_collections[index])
				{
					_debug.debugln(2, F("Deleting collection at %d"), index);
					delete _collections[index];
					_collections[index] = nullptr;
					return;
				}
				else
				{
					_debug.debugln(2, F("No collection at %d"), index);
				}
			};

		//Collection operation
		//_______________________________________________________________________________________________________
		public:		
			///Plays the given note on a collection
			/*!
				\param index Index of the collection to play the note on
				\param note MIDI note to play
			*/
			inline void playNote(uint8_t index, uint8_t note)
			{
				Base_MIDI_Pitch_Collection* c = getCollection(index);
				if(!c) return;
				c->playNote(note);
			};
			
			///Pitch bends all active notes in a collection
			/*!
				\param index Index of the collection to bend the notes of
				\param bend Value to pitch bend by (-8192 to 8191)
				\param shiftRange True indicates incoming values need to be shifted from 0 to 16383 to -8192 to 8191
			*/
			inline void bendNote(uint8_t index, int16_t bend, bool shiftRange = false)
			{
				Base_MIDI_Pitch_Collection* c = getCollection(index);
				if(!c) return;
				c->bendNote(bend, shiftRange);
			};
			
			///Stops the given note on a collection
			/*!
				\param index Index of the collection to stop the note on
				\param note MIDI note to stop
			*/
			inline void stopNote(uint8_t index, uint8_t note)
			{
				Base_MIDI_Pitch_Collection* c = getCollection(index);
				if(!c) return;
				c->stopNote(note);
			};
			
			///Resets a given collection if it can be
			/*!
				\param index Index of the collection to reset
			*/
			inline void reset(uint8_t index)
			{
				Base_MIDI_Pitch_Collection* c = getCollection(index);
				if(!c) return;
				c->reset();
			};
			
			///Resets all collections if they can be
			inline void resetAll()
			{
				int i = 0;
				while(i != MAX_COLLECTIONS)
					reset(i++);
			};
	};

	//Defines a global instance of our class for users to consume
	inline MIDI_Collection_Controller MCC = MIDI_Collection_Controller::getInstance();
#endif