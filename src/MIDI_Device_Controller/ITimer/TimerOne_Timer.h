#ifndef TimerOne_Timer_h
	#define TimerOne_Timer_h
	
	#include "ITimer.h"
	#include <TimerOne.h>
	
	/// @private
	class TimerOne_Timer : public ITimer
	{
		public:
			inline void setupOnce(uint16_t interval, void (*action)())
			{
				_debug.debugln(TRACE, F("Starting setupOnce()"));
				if(_isSetup)
				{
					_debug.debugln(DEBUG, F("Timer already setup"));
					return;
				}
				
				_debug.debugln(TRACE, F("Initialize timer"));
				Timer1.initialize();
				_debug.debugln(TRACE, F("Attach interrupt"));
				Timer1.attachInterrupt(action);
				
				//There's a possible problem here if the interval is changed for some reason
				//Since setupOnce is only executed once and exits out on subsequent calls, setPeriod will never be called with the updated value
				//Shouldn't happen realistically as resolution is kind of a constant, but it is technically possible to change it
				//TODO: Address this potential problem if it actually matters
				_debug.debugln(TRACE, F("Set interval"));
				Timer1.setPeriod(interval);
				
				_isSetup = true;
				_debug.debugln(TRACE, F("Finished setupOnce()"));
			};
			
			inline void start()
			{
				_debug.debugln(TRACE, F("Starting timer"));
				Timer1.start();
				_debug.debugln(TRACE, F("Timer started"));
			};
			
			inline void stop()
			{
				_debug.debugln(TRACE, F("Stopping timer"));
				Timer1.stop();
				_debug.debugln(TRACE, F("Timer stopped"));
			};
	};
	
#endif