#ifndef DeviceChain_Controller_h
	#define DeviceChain_Controller_h

	#include "Settings.h"
	#include "MIDI_Device_Controller.h"
	#include "MIDI_Device_Controller/MIDI_Device.h"
	#include "DeviceChain_Controller/DeviceChain_Base.h"
	#include "DeviceChain_Controller/DeviceChain_Direct.h"

	enum ChainType_t { Direct, FirstAvailable, RoundRobin };

	class DeviceChain_Controller
	{
		// Give Device access to all private members
		friend class MIDI_Device;
		
		static SerialDebug _debug;
		
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			DeviceChain_Controller();
			static DeviceChain_Controller *_instance;
			
		public:
			//Used to populate our single instance MDC for consumption
			static DeviceChain_Controller &getInstance();
			
		//Device management
		//_______________________________________________________________________________________________________
		private:
			static DeviceChain_Base *_chains[MAX_CHAINS];

		public:
			void printStatus(); //Print each device slot's status
			
			void addChain(uint8_t index, DeviceChain_Base *c); //Adds a chain by index
			DeviceChain_Base *getChain(uint8_t index); //Retrieves a chain by specified index
			
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
	extern DeviceChain_Controller DCC;
#endif
