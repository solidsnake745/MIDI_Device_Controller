#ifndef MIDI_Periods_h
	#define MIDI_Periods_h

	#include <avr/pgmspace.h>
	#include "../Settings.h"
	#include "../SerialDebug/SerialDebug.h"

	class MIDI_Periods
	{
		//Give MIDI_DeviceController access to all private members
		friend class MIDI_DeviceController;
		
		//Constructor(s)
		MIDI_Periods(); //Disallow creating an instance
		
		static SerialDebug _debug;
		
		// Variables:
		static uint32_t _currentResolution;
		
		//Used to pull original microperiod values from program memory
		//Indexed based on MIDI note system (0 - 127)
		static uint16_t getOriginalPeriod(uint16_t index);
		
		//Used to update and maintain calculatedPeriods with _currentResolution
		//Sets resolution and computes adjusted resolution to be used for note processing
		static void calculatePeriods(uint16_t resolution);
			
		public:
			// Overall properties
			// Calculated microperiods based on the set resolution for each note
			static uint16_t calculatedPeriods[128];
			
			//Original microperiods of notes based on the MIDI note system (0 - 127)
			//Stored in program memory to save flash
			constexpr static uint32_t ORIGINAL_PERIODS[128] PROGMEM = {
				122312, 115447, 108968, 102852, 97079, 91631, 86488, 81634, 77052, 72727, 68645, 64793,
				61156, 57724, 54484, 51426, 48540, 45815, 43244, 40817, 38526, 36364, 34323, 32396,
				30578, 28862, 27242, 25713, 24270, 22908, 21622, 20408, 19263, 18182, 17161, 16198,
				15289, 14431, 13621, 12856, 12135, 11454, 10811, 10204, 9631, 9091, 8581, 8099,
				7645, 7215, 6810, 6428, 6067, 5727, 5405, 5102, 4816, 4545, 4290, 4050,
				3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551, 2408, 2273, 2145, 2025,
				1911, 1804, 1703, 1607, 1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,
				956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506,
				478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253,
				239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127,
				119, 113, 106, 100, 95, 89, 84, 80
			};
			
			static uint32_t getResolution();
			static void setResolution(uint32_t resolution = DEFAULT_RESOLUTION);
			
			static void printOriginalPeriod(uint8_t note);
			static void printOriginalPeriods();
			static void printCalculatedPeriod(uint8_t note);
			static void printCalculatedPeriods();
	};
#endif
