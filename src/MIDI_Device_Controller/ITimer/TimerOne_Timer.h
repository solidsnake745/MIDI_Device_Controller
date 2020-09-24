#ifndef TimerOne_Timer_h
	#define TimerOne_Timer_h
	
	#include "ITimer.h"
	#include <TimerOne.h>
	
	/// @private
	class TimerOne_Timer : public ITimer
	{
		private:
			bool _isSetup = false;
			
		public:
			void setupOnce(uint16_t interval, void (*action)());
			void start();
			void stop();
	};
	
#endif