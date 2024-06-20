#ifndef MIDI_Periods_h
	#define MIDI_Periods_h
	
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	
	//Resolve pgmspace dependency
	#if ARDUINO_ARCH_AVR
		#include <avr/pgmspace.h>
	#elif defined(CORE_TEENSY)
		//Already included
	#elif ARDUINO_ARCH_ESP32
		#include <pgmspace.h>
	#endif

	/// @private
	class MIDI_Periods
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_DeviceController;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_MIDIPERIODS);
		
		//Constructor(s)
		MIDI_Periods(); //Disallow creating an instance		
		
		// Variables:
		inline static uint32_t _currentResolution;
		
		//Used to update and maintain calculatedPeriods with _currentResolution
		//Sets resolution and computes adjusted resolution to be used for note processing
		static void calculatePeriods(uint16_t resolution);

		public:
			// Overall properties
			// Calculated microperiods based on the set resolution for each note
			inline static uint16_t calculatedPeriods[128];
			
			//Original microperiods of notes based on the MIDI note system (0 - 127)
			//Stored in program memory to save flash
			inline constexpr static uint32_t ORIGINAL_PERIODS[128] PROGMEM = 
			{
			//	C		C#		D		D#		E		F		F#		G		G#		A		A#		B
				122312,	115447,	108968,	102852,	97079,	91631,	86488,	81634,	77052,	72727,	68645,	64793, 	//C0 - B0
				61156,	57724,	54484,	51426,	48540,	45815,	43244,	40817,	38526,	36364,	34323,	32396, 	//C1 - B1
				30578,	28862,	27242,	25713,	24270,	22908,	21622,	20408,	19263,	18182,	17161,	16198,	//C2 - B2
				15289,	14431,	13621,	12856,	12135,	11454,	10811,	10204,	9631,	9091,	8581,	8099,	//C3 - B3
				7645,	7215,	6810,	6428,	6067,	5727,	5405,	5102,	4816,	4545,	4290,	4050,	//C4 - B4 (Middle C = 48)
				3822,	3608,	3405,	3214,	3034,	2863,	2703,	2551,	2408,	2273,	2145,	2025,	//C5 - B5
				1911,	1804,	1703,	1607,	1517,	1432,	1351,	1276,	1204,	1136,	1073,	1012,	//C6 - B6
				956,	902,	851,	804,	758,	716,	676,	638,	602,	568,	536,	506,	//C7 - B7
				478,	451,	426,	402,	379,	358,	338,	319,	301,	284,	268,	253,	//C8 - B8
				239,	225,	213,	201,	190,	179,	169,	159,	150,	142,	134,	127,	//C9 - B9
				119,	113,	106,	100,	95,		89,		84,		80										//C10 - G10
			};
			
			//Used to pull original microperiod values from program memory
			//Indexed based on MIDI note system (0 - 127)
			inline static uint32_t getOriginalPeriod(uint16_t index) { return pgm_read_dword(ORIGINAL_PERIODS + index); };
			
			inline static uint32_t getResolution() { return _currentResolution; };
			static void setResolution(uint32_t resolution = DEFAULT_RESOLUTION);
			
			/// @private
			inline static void setDebugResolution() { _currentResolution = 100000; };
			
			inline static void printOriginalPeriod(uint8_t note) { _debug.println(F("Original period for %d: %d"), note, getOriginalPeriod(note)); };
			inline static void printOriginalPeriods() { for(uint8_t i = 0; i < 128; i++) printOriginalPeriod(i); };
			inline static void printCalculatedPeriod(uint8_t note) { _debug.println(F("Calculated period for %d: %d"), note, calculatedPeriods[note]); };
			inline static void printCalculatedPeriods() { for(uint8_t i = 0; i < 128; i++) printCalculatedPeriod(i); };
	};
#endif
