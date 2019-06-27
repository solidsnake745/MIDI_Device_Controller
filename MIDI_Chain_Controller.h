#ifndef MIDI_Chain_Controller_h
	#define MIDI_Chain_Controller_h

	#include "Settings.h"
	#include "MIDI_Device_Controller.h"
	#include "MIDI_Device_Controller/MIDI_Device.h"
	#include "MIDI_Chain_Controller/MIDI_Device_Chain.h"
	#include "MIDI_Chain_Controller/MIDI_Device_Chains.h"

	enum ChainType_t { Direct, FirstAvailable, RoundRobin };

	class MIDI_Chain_Controller
	{
		// Give Device access to all private members
		friend class MIDI_Device;
		
		static SerialDebug _debug;
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			MIDI_Chain_Controller();
			static MIDI_Chain_Controller *_instance;
			
		public:
			//Used to populate our single instance MDC for consumption
			static MIDI_Chain_Controller &getInstance();
			
		//Device management
		//_______________________________________________________________________________________________________
		private:
			static MIDI_Device_Chain *_chains[MAX_CHAINS];

		public:
			void printStatus(); //Print each device slot's status
			
			void addChain(uint8_t index, MIDI_Device_Chain *c); //Adds a chain by index
			MIDI_Device_Chain *getChain(uint8_t index); //Retrieves a chain by specified index
			
			void createChain(uint8_t index, ChainType_t type, uint8_t deviceIndexes[]);
			void deleteChain(uint8_t index); //Deletes a chain by specified index if populated

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
	extern MIDI_Chain_Controller MCC;
#endif
