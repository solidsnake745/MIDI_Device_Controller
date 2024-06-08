#ifndef noteDurationTracker_h
	#define noteDurationTracker_h
	
	#include <stdint.h>
	#include "../Settings.h"
	#include "../SerialDebug/SerialDebug.h"

	//Resolve STL dependency
	#if defined(ARDUINO_ARCH_AVR)
		#include "../../ArduinoSTLClone/cstdlib"		
	#elif defined(CORE_TEENSY)
		// Already included
	#elif defined(ESP32)
		// Already included
	#endif
	
	//@private
	struct noteDuration
	{
		private:
			inline static SerialDebug _debug = SerialDebug(5);
			uint16_t _seconds = 0, _millis = 0, _micros = 0;
			
		public:
			noteDuration(){};

			//Create new object with microsonds, milliseconds, and/or seconds
			noteDuration(uint16_t us, uint16_t ms = 0, uint16_t s = 0)
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

			bool operator <(noteDuration &nd) 
			{
				if(_seconds > nd._seconds)
					return false;

				if(_millis > nd._millis)
					return false;

				if(_micros > nd._micros)
					return false;  

				return true;
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
			inline static void runTest()
			{
				noteDuration test;
				
				_debug.debugln(5, F("Test microsecond rollover"));
				test = noteDuration(998);
				for(uint16_t x = 0; x < 3; x++)
				{
					test.addMicros(1);
					_debug.debugln(5, F("%d\t%d\t%d"), test.getSeconds(), test.getMillis(), test.getMicros());
				}
				
				_debug.debugln(5, F("Test millisecond rollover"));
				test = noteDuration(0, 998);
				for(uint16_t x = 0; x < 3; x++)
				{
					test.addMillis(1);
					_debug.debugln(5, F("%d\t%d\t%d"), test.getSeconds(), test.getMillis(), test.getMicros());
				}
			}
			#endif
	};

#endif
