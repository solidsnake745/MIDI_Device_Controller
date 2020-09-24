#ifndef ITimer_h
	#define ITimer_h
	
	#include <stdint.h>
	
	/// @private
	class ITimer
	{
		protected:
			bool _isSetup = false;
		
		public:
			virtual void setupOnce(uint16_t interval, void (*action)());
			virtual void start();
			virtual void stop();
	};
	
#endif