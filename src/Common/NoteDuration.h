#ifndef NoteDuration_h
	#define NoteDuration_h
	
	#include <stdint.h>
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"

	//Resolve STL dependency
	#if ARDUINO_ARCH_AVR
		#include "../../ArduinoSTLClone/cstdlib"		
	#elif defined(CORE_TEENSY)
		// Already included
	#elif ARDUINO_ARCH_ESP32
		// Already included
	#endif
		
	/// @brief Class that represents a note's duration
	/// @details Used to track how long notes have been playing.<br>
	/// Used for max duration functionality in various places.
	class NoteDuration
	{
		uint16_t _seconds = 0, _millis = 0, _micros = 0;
			
		public:
			/// @brief Default constructor
			inline NoteDuration() {};

			/// @brief Create a new NoteDuration with microseconds, milliseconds, and/or seconds
			NoteDuration(uint16_t us, uint16_t ms = 0, uint16_t s = 0);

			/// @brief Gets the second component of duration
			inline uint16_t getSeconds() { return _seconds; };
			
			/// @brief Gets the millisecond component of duration
			inline uint16_t getMillis() { return _millis; };
			
			/// @brief Gets the microsecond component of duration
			inline uint16_t getMicros() { return _micros; };
			
			/// @brief Resets duration to 0
			inline void reset() { _seconds = 0; _millis = 0; _micros = 0; };

			/// @brief Indicates if duration is 0
			/// @retval true if zero
			/// @retval false if not zero
			inline bool isZero() { return _seconds == 0 && _millis == 0 && _micros == 0; };

			// inline double convertToSeconds()
			// {
				// double sec = _seconds;
				// double milli = _millis / 1000.0;
				// double micro = _micros / 1000000.0;
				// return sec + milli + micro;
			// }

			//This works but is horrifically inefficient
			// bool operator <(NoteDuration& nd) { return convertToSeconds() < nd.convertToSeconds(); };
			
			bool operator <=(NoteDuration& nd);

			/// @brief Adds seconds to duration
			/// @param s Seconds to add
			void addSeconds(uint16_t s);
			
			/// @brief Adds milliseconds to duration
			/// @param s Milliseconds to add
			void addMillis(uint32_t ms);

			/// @brief Adds microseconds to duration
			/// @param s Microseconds to add
			void addMicros(uint32_t us);
			
			#if INCLUDE_TESTS
			inline static SerialDebug _debug = SerialDebug(DEBUG);
			
			inline static void printTime(NoteDuration nd) { _debug.println(F("%d\t%d\t%d"), nd.getSeconds(), nd.getMillis(), nd.getMicros()); };
			
			inline static void runTest()
			{
				NoteDuration test1;
				
				_debug.println(F("Test microsecond rollover"));
				test1 = NoteDuration(998);
				for(uint16_t x = 0; x < 3; x++)
				{
					test1.addMicros(1);
					printTime(test1);
				}
				_debug.println();
				
				_debug.println(F("Test millisecond rollover"));
				test1 = NoteDuration(0, 998);
				for(uint16_t x = 0; x < 3; x++)
				{
					test1.addMillis(1);
					printTime(test1);
				}
				_debug.println();
				
				_debug.println(F("Test convert to seconds"));
				test1 = NoteDuration(999, 999, 999);
				_debug.println("Converted: %6f", test1.convertToSeconds());
				_debug.println();
				
				NoteDuration test2;
				_debug.println(F("Test <= operator"));
				test1.reset();
				test1.addMicros(2 * 1000 * 1000);
				
				while(test2 <= test1)
					test2.addMicros(1);
				
				_debug.print(F("Test1 time: "));
				printTime(test1);
				
				_debug.print(F("Test2 time: "));
				printTime(test2);
			}
			#endif
	};

#endif
