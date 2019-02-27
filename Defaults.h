#ifndef Defaults_h
	#define Defaults_h

	#include <avr/pgmspace.h>
	#define MAX_DEFAULTS 12

	#define Defaults_h_DEBUG 0	

	class Defaults
	{	
		//Constructor(s)
		Defaults(); //Disallow creating an instance
			
		public:
			//Constants:
			//Default pins to use for each device
			//-1 indicates no default set or not in use
			constexpr static int8_t STEPPIN_DEFAULTS[MAX_DEFAULTS] PROGMEM = {
				2,  	//Device 0
				4,  	//Device 1
				6,  	//Device 2
				8,  	//Device 3
				10,  	//Device 4
				14,  	//Device 5
				16,  	//Device 6
				18,  	//Device 7
				-1,   	//Device 8
				-1,  	//Device 9
				-1,  	//Device 10
				-1  	//Device 11
			};

			constexpr static int8_t DIRPIN_DEFAULTS[MAX_DEFAULTS] PROGMEM = {
				3,  	//Device 0
				5,  	//Device 1
				7,  	//Device 2
				9,  	//Device 3
				11,  	//Device 4
				15,  	//Device 5
				17,  	//Device 6
				19,  	//Device 7
				-1,   	//Device 8
				-1,  	//Device 9
				-1,  	//Device 10
				-1  	//Device 11
			};

			//Default max position to use for each device
			//-1 indicates no default set or not in use
			constexpr static int16_t MAXPOSITION_DEFAULTS[MAX_DEFAULTS] PROGMEM = {
				158,	//Device 0
				158,	//Device 1
				158,  	//Device 2
				158,  	//Device 3
				158,  	//Device 4
				158,  	//Device 5
				158,  	//Device 6
				158,  	//Device 7
				-1,   	//Device 8
				-1,   	//Device 9
				-1,   	//Device 10
				-1   	//Device 11
			};
			
			static int8_t getStepDefault(uint8_t device);
			static int8_t getDirDefault(uint8_t device);
			static int16_t getMaxPosDefault(uint8_t device);
			
			static void printStepDefault(uint8_t device);
			static void printStepDefaults();
			static void printDirDefault(uint8_t device);
			static void printDirDefaults();
			static void printMaxPositionDefault(uint8_t device);
			static void printMaxPositionDefaults();
	};
#endif