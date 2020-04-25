#ifndef MIDI_Collection_Controller_h
	#define MIDI_Collection_Controller_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Device_Controller/MIDI_Pitch.h"
	#include "Base_MIDI_Pitch_Collection.h"

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
			static MIDI_Collection_Controller &getInstance();
			
		//Device management
		//_______________________________________________________________________________________________________
		private:
			static Base_MIDI_Pitch_Collection *_collections[MAX_COLLECTIONS];

		public:
			void printStatus(); //Print each device slot's status
			
			void addCollection(uint8_t index, Base_MIDI_Pitch_Collection *c); //Adds a Collection by index
			Base_MIDI_Pitch_Collection *getCollection(uint8_t index); //Retrieves a Collection by specified index
			void deleteCollection(uint8_t index); //Deletes a Collection by specified index if populated

			void playNote(int8_t index, uint8_t note);
			void bendNote(int8_t index, uint16_t bend);
			void stopNote(int8_t index, uint8_t note);			

		//Settings
		//_______________________________________________________________________________________________________
		private:
			
		public:
		
		//Tests/Debug 
		//_______________________________________________________________________________________________________
		public:

	};

	//Defines a global instance of our class for users to consume
	extern MIDI_Collection_Controller MCC;
#endif
