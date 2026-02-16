#ifndef PitchBend_h
	#define PitchBend_h
	
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	
	#if INCLUDE_TESTS
		#include "MIDI_Periods.h"
	#endif
	
	/// @brief Shared class for pitch bending
	/// @details Used to centralize logic and settings
	class PitchBend
	{
		#if BEND_MODE == 1
			inline constexpr static float BEND_PER_OCTAVE = ((float)BEND_SEMITONES/(float)SEMITONES_PER_OCTAVE);
		#endif
		
		PitchBend(){}; //Disallow creating an instance
		
		public:			
			/// @brief Calculates pitch bend factor based on settings
			/// @param bend MIDI pitch bend value to calculate the factor for
			/// @param shiftRange When true, shifts bend values down from 0 to 16383 to -8192 to 8191
			static float calculateFactor(int16_t bend, bool shiftRange = false);
			
			#if INCLUDE_TESTS
			inline static SerialDebug _debug = SerialDebug(DEBUG_PITCHBEND);
			
			inline static void runTest()
			{
				#if BEND_MODE == 1
					_debug.println(F("BEND_PER_OCTAVE: %f"), BEND_PER_OCTAVE);
				#endif
	
				int16_t testBendValues[] = { -8192, -4096, 0, 4096, 8191 };
				
				uint32_t middleCPeriod = MIDI_Periods::getOriginalPeriod(MIDDLE_C_NOTE);
				_debug.println(F("Bend test for middle C - Period: %d"), middleCPeriod);
				
				uint8_t numTestValues = (sizeof(testBendValues)/sizeof(int16_t));
				for(uint8_t x = 0; x < numTestValues; x++)
				{
					int16_t testValue = testBendValues[x];
					_debug.println(F("Bend value: %d"), testValue);
					
					//Factors should be exactly the same
					float factorNormal = calculateFactor(testValue);
					float factorShifted = calculateFactor(testValue + 8192, true);
					_debug.println(F("\tFactors: %f/%f"), factorNormal, factorShifted);
					
					//Period after calculating with the new factor
					uint32_t expectedPeriod = MIDI_Periods::getOriginalPeriod(MIDDLE_C_NOTE + (x - 2));
					_debug.println(F("\tNew/Expected period: %d (%d)"), (uint16_t)(middleCPeriod/factorNormal), expectedPeriod);
				}
			}
			#endif
	};
	
#endif
