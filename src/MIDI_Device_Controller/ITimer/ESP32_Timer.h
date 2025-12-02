#ifndef TimerOne_Timer_h
	#define TimerOne_Timer_h
	
	#include "ITimer.h"
	#include <Arduino.h>
	
	/// @private
	class ESP32_Timer : public ITimer
	{
		private:
			hw_timer_t* _timer;
			
		public:
			inline void setupOnce(uint16_t interval, void (*action)())
			{
				_debug.debugln(8, F("Starting setupOnce()"));
				if(_isSetup)
				{
					_debug.debugln(8, F("Timer already setup"));
					return;
				}
				
				#if ESP_ARDUINO_VERSION_MAJOR == 2
					//ESP32 version 2.x implementation
					_debug.debugln(8, F("Create timer"));
					_timer = timerBegin(0, 80, true); //1 MHz (1 microsecond per tick)
					
					_debug.debugln(8, F("Attach interrupt"));
					timerAttachInterrupt(_timer, action, true);
				
					_debug.debugln(8, F("Set interval"));
					timerAlarmWrite(_timer, interval, true);
				#elif ESP_ARDUINO_VERSION_MAJOR == 3
					_debug.debugln(8, F("Create timer"));
					_timer = timerBegin(1000000); //1 MHz (1 microsecond per tick)
					
					_debug.debugln(8, F("Attach interrupt"));
					timerAttachInterrupt(_timer, action);
					
					_debug.debugln(8, F("Set interval"));
					timerAlarm(_timer, interval, true, 0);
				#endif
				
				//There's a possible problem here if the interval is changed for some reason
				//Since setupOnce is only executed once and exits out on subsequent calls, setPeriod will never be called with the updated value
				//Shouldn't happen realistically as resolution is kind of a constant, but it is technically possible to change it
				//TODO: Address this potential problem if it actually matters
				_isSetup = true;
				_debug.debugln(8, F("Finished setupOnce()"));
			};
			
			inline void start()
			{
				_debug.debugln(8, F("Starting timer"));
				#if ESP_ARDUINO_VERSION_MAJOR == 2
					timerAlarmEnable(_timer);
				#elif ESP_ARDUINO_VERSION_MAJOR == 3
					timerStart(_timer);
				#endif				
				_debug.debugln(8, F("Timer started"));
			};
			
			inline void stop()
			{
				_debug.debugln(8, F("Stopping timer"));
				#if ESP_ARDUINO_VERSION_MAJOR == 2
					timerAlarmDisable(_timer);
				#elif ESP_ARDUINO_VERSION_MAJOR == 3
					timerStop(_timer);
				#endif				
				_debug.debugln(8, F("Timer stopped"));
			};
	};
	
#endif