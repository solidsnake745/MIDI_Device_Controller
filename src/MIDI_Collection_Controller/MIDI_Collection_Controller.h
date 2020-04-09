#ifndef MIDI_Collection_Controller_h
	#define MIDI_Collection_Controller_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Device_Controller/MIDI_Device.h"
	#include "Base_MIDI_Device_Collection.h"

	class MIDI_Collection_Controller
	{
		// Give Device access to all private members
		friend class MIDI_Device;
		
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
			static Base_MIDI_Device_Collection *_collections[MAX_COLLECTIONS];

		public:
			void printStatus(); //Print each device slot's status
			
			void addCollection(uint8_t index, Base_MIDI_Device_Collection *c); //Adds a Collection by index
			Base_MIDI_Device_Collection *getCollection(uint8_t index); //Retrieves a Collection by specified index
			void deleteCollection(uint8_t index); //Deletes a Collection by specified index if populated

			void assignNote(int8_t index, uint8_t note);
			void clearNote(int8_t index, uint8_t note);
			void pitchBend(int8_t index, uint16_t bend);

		//Settings
		//_______________________________________________________________________________________________________
		private:
			
		public:	
			
		
		public: 
		
		//Tests/Debug 
		//_______________________________________________________________________________________________________
		public:

	};

	//Defines a global instance of our class for users to consume
	extern MIDI_Collection_Controller MCC;
#endif
