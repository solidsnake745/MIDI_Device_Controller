#ifndef PitchBend_h
	#define PitchBend_h
	
	#include "../Settings.h"
	#include "../SerialDebug/SerialDebug.h"
	
	#if INCLUDE_TESTS
		#include "../MIDI_Device_Controller/MIDI_Periods.h"
		#define MIDDLE_C_NOTE 48
	#endif
	
	//@private
	class PitchBend
	{		
		inline static SerialDebug _debug = SerialDebug(5);
		inline constexpr static float BEND_PER_OCTAVE = ((float)BEND_SEMITONES/(float)SEMITONES_PER_OCTAVE);
		PitchBend(){}; //Disallow creating an instance
		
		public:
			inline static float calculateFactor(int bend, bool shiftRange = false)
			{
				if(shiftRange) bend -= 8192;
				float result = pow(2.0, (BEND_PER_OCTAVE * (bend / 8192.0)));
				return result;
			}
			
			#if INCLUDE_TESTS
			inline static void runTest()
			{
				_debug.debugln(5, F("BEND_PER_OCTAVE: %f"), BEND_PER_OCTAVE);
				
				//Bending by these values should result in -2, -1, 0, 1, and 2 notes over (when BEND_SEMITONES = 2)
				int16_t testBendValues[] = { -8192, -4096, 0, 4096, 8191 };
				
				uint32_t middleCPeriod = MIDI_Periods::getOriginalPeriod(MIDDLE_C_NOTE);
				_debug.debugln(5, F("Bend test for middle C - Period: %d"), middleCPeriod);
				
				uint8_t numTestValues = (sizeof(testBendValues)/sizeof(int16_t));
				for(uint8_t x = 0; x < numTestValues; x++)
				{
					int16_t testValue = testBendValues[x];
					_debug.debugln(5, F("Bend value: %d"), testValue);
					
					//Factors should be exactly the same
					float factorNormal = calculateFactor(testValue);
					float factorShifted = calculateFactor(testValue + 8192, true);
					_debug.debugln(5, F("\tFactors: %f/%f"), factorNormal, factorShifted);
					
					//Period after calculating with the new factor
					uint32_t expectedPeriod = MIDI_Periods::getOriginalPeriod(MIDDLE_C_NOTE + (x - 2));
					_debug.debugln(5, F("\tNew/Expected period: %d (%d)"), (uint16_t)(middleCPeriod/factorNormal), expectedPeriod);
				}
			}
			#endif
	};
	
#endif
