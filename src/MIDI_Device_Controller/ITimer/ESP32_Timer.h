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
				
				//ESP32 version 2.x implementation
				// _timer = timerBegin(0, 80, true); //1 MHz (1 microsecond per tick)
				// timerAttachInterrupt(_timer, action, true);
				// timerAlarmWrite(_timer, microseconds, true);
				// timerAlarmEnable(_timer); //Start
				// timerAlarmDisable(_timer); //Stop
				
				_debug.debugln(8, F("Create timer"));
				_timer = timerBegin(1000000); //1 MHz (1 microsecond per tick)
				_debug.debugln(8, F("Attach interrupt"));
				timerAttachInterrupt(_timer, action);
				
				//There's a possible problem here if the interval is changed for some reason
				//Since setupOnce is only executed once and exits out on subsequent calls, setPeriod will never be called with the updated value
				//Shouldn't happen realistically as resolution is kind of a constant, but it is technically possible to change it
				//TODO: Address this potential problem if it actually matters
				_debug.debugln(8, F("Set interval"));
				timerAlarm(_timer, interval, true, 0);
				
				_isSetup = true;
				_debug.debugln(8, F("Finished setupOnce()"));
			};
			
			inline void start()
			{
				_debug.debugln(8, F("Starting timer"));
				timerStart(_timer);
				_debug.debugln(8, F("Timer started"));
			};
			
			inline void stop()
			{
				_debug.debugln(8, F("Stopping timer"));
				timerStop(_timer);
				_debug.debugln(8, F("Timer stopped"));
			};
	};
	
#endif