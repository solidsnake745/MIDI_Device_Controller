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
	
	/// @brief Stores MIDI period information for notes
	/// @details Used by Base_MIDI_Pitch classes for playing notes
	/// Stores full sized data in program memory and caches at half size during runtime
	class MIDI_Periods
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_DeviceController;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_MIDIPERIODS);
		
		//Constructor(s)
		MIDI_Periods(); //Disallow creating an instance
		
		inline static uint32_t _currentResolution;
		
		/// @brief Calculates and populates calculatedPeriods based on the current resolution
		static void calculatePeriods();

		public:
			/// @brief Calculated microperiods based on the set resolution for each note
			/// @details The largest value will be ORIGINAL_PERIODS[0]/2 = 61156 which is just under 2^16 (65535)
			inline static uint16_t calculatedPeriods[128];
			
			/// @brief Original microperiods of notes based on the MIDI note system (0 - 127)
			/// @details Stored in program memory to save flash
			inline constexpr static uint32_t ORIGINAL_PERIODS[128] PROGMEM = 
			{
			//	C		C#		D		D#		E		F		F#		G		G#		A		A#		B
				122312,	115447,	108968,	102852,	97079,	91631,	86488,	81634,	77052,	72727,	68645,	64793, 	//Octave -1
				61156,	57724,	54484,	51426,	48540,	45815,	43244,	40817,	38526,	36364,	34323,	32396, 	//Octave 0
				30578,	28862,	27242,	25713,	24270,	22908,	21622,	20408,	19263,	18182,	17161,	16198,	//Octave 1
				15289,	14431,	13621,	12856,	12135,	11454,	10811,	10204,	9631,	9091,	8581,	8099,	//Octave 2
				7645,	7215,	6810,	6428,	6067,	5727,	5405,	5102,	4816,	4545,	4290,	4050,	//Octave 3
				3822,	3608,	3405,	3214,	3034,	2863,	2703,	2551,	2408,	2273,	2145,	2025,	//Octave 4 (Middle C = 60)
				1911,	1804,	1703,	1607,	1517,	1432,	1351,	1276,	1204,	1136,	1073,	1012,	//Octave 5
				956,	902,	851,	804,	758,	716,	676,	638,	602,	568,	536,	506,	//Octave 6
				478,	451,	426,	402,	379,	358,	338,	319,	301,	284,	268,	253,	//Octave 7
				239,	225,	213,	201,	190,	179,	169,	159,	150,	142,	134,	127,	//Octave 8
				119,	113,	106,	100,	95,		89,		84,		80										//Octave 9
			};
			
			/// @brief Gets the current resolution
			inline static uint32_t getResolution() { return _currentResolution; };
			
			/// @brief Sets the current resolution and updates calculated microperiods
			/// @param resolution Resolution to set in microseconds
			static void setResolution(uint32_t resolution = DEFAULT_RESOLUTION);
			
			/// @private
			inline static void setDebugResolution() { _currentResolution = 1000000; }; //1 second interval
			
			/// @brief Gets the original period for the given note from program memory			
			static uint32_t getOriginalPeriod(uint16_t index);
			
			/// @brief Prints the original period for the given note
			/// @param note MIDI note number to print the period for
			static void printOriginalPeriod(uint8_t note);
			
			/// @brief Prints the original period for notes 0 through 127
			static void printOriginalPeriods();
			
			/// @brief Prints the calculated period for the given note
			/// @param note MIDI note number to print the period for
			static void printCalculatedPeriod(uint8_t note);
			
			/// @brief Prints the calculated period for notes 0 through 127
			static void printCalculatedPeriods();
	};
	
#endif
