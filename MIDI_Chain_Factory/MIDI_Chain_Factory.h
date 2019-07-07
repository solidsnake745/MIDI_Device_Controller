#ifndef MIDI_Chain_Factory_h
	#define MIDI_Chain_Factory_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Chain_Controller/MIDI_Device_Chain.h"
	#include "../MIDI_Chain_Controller/MIDI_Device_Chains.h"
	
	enum ChainType_t { Direct, FirstAvailable, RoundRobin };
	
	class MIDI_Chain_Factory
	{
		//Constructor(s)
		MIDI_Chain_Factory(); //Disallow creating an instance
		static MIDI_Chain_Factory *_instance;
			
		public:
			//Used to populate our single instance MDF for consumption
			static MIDI_Chain_Factory &getInstance();
			
			void createChain(uint8_t index, ChainType_t type, uint8_t deviceIndexes[]);
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Chain_Factory MCF;
#endif