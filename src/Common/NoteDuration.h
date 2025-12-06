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
	
	//@private
	struct NoteDuration
	{
		private:
			inline static SerialDebug _debug = SerialDebug(5);
			uint16_t _seconds = 0, _millis = 0, _micros = 0;
			
		public:
			NoteDuration(){};

			//Create new object with microsonds, milliseconds, and/or seconds
			NoteDuration(uint16_t us, uint16_t ms = 0, uint16_t s = 0)
			{
				addSeconds(s);
				addMillis(ms);
				addMicros(us);
			};

			inline void reset() 
			{
				_seconds = 0;
				_millis = 0;
				_micros = 0;
			};

			inline bool isZero() 
			{
				return _seconds == 0 && _millis == 0 && _micros == 0;
			};

			inline double convertToSeconds()
			{
				double sec = _seconds;
				double milli = _millis / 1000.0;
				double micro = _micros / 1000000.0;
				return sec + milli + micro;
			}

			//This works but is horrifically inefficient
			//bool operator <(NoteDuration &nd) { return convertToSeconds() < nd.convertToSeconds(); };
			
			bool operator <=(NoteDuration &nd)
			{
				if (_seconds != nd._seconds)
					return _seconds < nd._seconds;
				
				if (_millis != nd._millis)
					return _millis < nd._millis;

				return _micros <= nd._micros;
			};
			
			inline uint16_t getSeconds() { return _seconds; };
			inline uint16_t getMillis() { return _millis; };
			inline uint16_t getMicros() { return _micros; };

			inline void addSeconds(uint32_t s) { addMillis(s * 1000); };
			inline void addMillis(uint32_t ms) { addMicros(ms * 1000); };

			inline void addMicros(uint32_t us)
			{
				//Add passed in value
				if(us >= 1000000)
				{
					auto dv = std::div(us, 1000000);
					_seconds += dv.quot;
					us = dv.rem;
				}

				if(us >= 1000)
				{
					auto dv = std::div(us, 1000);
					_millis += dv.quot;
					us = dv.rem;
				}

				_micros += us;
				
				//Rollover updated duration
				if(_micros >= 1000)
				{
					auto dv = std::div(_micros, 1000);
					_millis += dv.quot;
					_micros = dv.rem;
				}
				
				if(_millis >= 1000)
				{
					auto dv = std::div(_millis, 1000);
					_seconds += dv.quot;
					_millis = dv.rem;
				}
			};
			
			#if INCLUDE_TESTS
			inline static void printTime(NoteDuration nd) { _debug.debugln(5, F("%d\t%d\t%d"), nd.getSeconds(), nd.getMillis(), nd.getMicros()); };
			
			inline static void runTest()
			{
				NoteDuration test1;
				
				_debug.debugln(5, F("Test microsecond rollover"));
				test1 = NoteDuration(998);
				for(uint16_t x = 0; x < 3; x++)
				{
					test1.addMicros(1);
					printTime(test1);
				}
				_debug.debugln(5);
				
				_debug.debugln(5, F("Test millisecond rollover"));
				test1 = NoteDuration(0, 998);
				for(uint16_t x = 0; x < 3; x++)
				{
					test1.addMillis(1);
					printTime(test1);
				}
				_debug.debugln(5);
				
				_debug.debugln(5, F("Test convert to seconds"));
				test1 = NoteDuration(999, 999, 999);
				_debug.debugln(5, "Converted: %6f", test1.convertToSeconds());
				_debug.debugln(5);
				
				NoteDuration test2;
				_debug.debugln(5, F("Test <= operator"));
				test1.reset();
				test1.addMicros(2 * 1000 * 1000);
				
				while(test2 <= test1)
					test2.addMicros(1);
				
				_debug.debug(5, F("Test1 time: "));
				printTime(test1);
				
				_debug.debug(5, F("Test2 time: "));
				printTime(test2);
			}
			#endif
	};

#endif
