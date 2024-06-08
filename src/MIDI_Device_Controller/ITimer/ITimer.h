#ifndef ITimer_h
	#define ITimer_h
	
	#include <stdint.h>
	
	/// @private
	class ITimer
	{
		protected:
			bool _isSetup = false;
		
		public:
			virtual void setupOnce(uint16_t interval, void (*action)()) = 0;
			virtual void start() = 0;
			virtual void stop() = 0;
	};
	
#endif