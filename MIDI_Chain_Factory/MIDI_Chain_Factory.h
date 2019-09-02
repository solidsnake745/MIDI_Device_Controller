#ifndef MIDI_Chain_Factory_h
	#define MIDI_Chain_Factory_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller.h"
	#include "../MIDI_Chain_Controller/MIDI_Device_Chain.h"
	#include "../MIDI_Chain_Controller/MIDI_Device_Chains.h"
	#include "../SerialDebug/SerialDebug.h"
	
	//Implement conjunction for C++ 11
	template<bool...> struct bool_pack{};
	template<class... Ts>
	using conjunction = std::is_same<bool_pack<true,Ts::value...>, bool_pack<Ts::value..., true>>;

	//Define compiler check for parameter packs to use uint8_t
	template<typename... Ts>
	using Alluint8_t = typename std::enable_if<conjunction<std::is_convertible<Ts, uint8_t>...>::value>::type;

	enum ChainType_t { Direct, FirstAvailable, RoundRobin };
	
	class MIDI_Chain_Factory
	{
		static SerialDebug _debug;
		
		//Constructor(s)
		MIDI_Chain_Factory(); //Disallow creating an instance
		static MIDI_Chain_Factory *_instance;
		
		//Creates a new chain and adds it to the controller (MCC)		
		MIDI_Device_Chain *createInitialChain(uint8_t index, ChainType_t type);
		
		MIDI_Device *getDeviceFromMDC(uint8_t index);
		
		//Recursive template method for populating a device chain from an argument pack of IDs
		template<typename... IDs, typename = Alluint8_t<IDs...>>
		inline void populateChain(MIDI_Device_Chain *c, uint8_t id, IDs... ids)
		{
			c->addDevice(getDeviceFromMDC(id));
			populateChain(c, ids...);
		};
		
		//Recursive decay case
		inline void populateChain(MIDI_Device_Chain *c) { _debug.debugln(5, "Finished populating"); };		
		
		public:
			//Used to populate our single instance MDF for consumption
			static MIDI_Chain_Factory &getInstance();
			
			//Used to generate device chains and add devices by specified IDs
			//Requires at least one device ID
			template<typename... IDs, typename = Alluint8_t<IDs...>>
			inline void createChain(uint8_t index, ChainType_t type, uint8_t firstId, IDs... ids)
			{
				MIDI_Device_Chain *newChain = createInitialChain(index, type);
				
				if(newChain)
				{
					_debug.println("Populating chain");
					populateChain(newChain, firstId, ids...);
				}
				else
					_debug.println("Failed to create new chain");
			};
	};
	
	//Defines a global instance of our class for users to consume
	extern MIDI_Chain_Factory MCF;
#endif