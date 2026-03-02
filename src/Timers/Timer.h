/// @file Timer.h
/// @brief Timer interrupt using the <TimerOne.h> library
/// @details Inline usage in timer implementations is required - no benefit to splitting it to a cpp.<br>
///	Each method is only called once in MDC - no code size benefit.<br>
///	Splitting to a cpp causes the header to always be compiled even when not in use.<br>
///	Leaving as header only allows for easy dependency resolution.

#ifndef Timer_h
	#define Timer_h
	
	#include <stdint.h>
	#include "../Common/SerialDebug.h"
	#include "../Settings.h"
	
	/// @brief Interface that represents an interrupt timer MDC knows how to use
	/// @details Since we're using typedef aliases to just create the specific type we need, technically this is not necessary.<br>
	/// However it's useful for enforcing required methods are defined as expected (naming/parameters).
	class Timer
	{
		protected:
			inline static SerialDebug _debug = SerialDebug(DEBUG_TIMER);
			bool _isSetup = false;
		
		public:
			/// @brief Sets up the timer once
			/// @param interval Rate of the interrupt in microseconds
			/// @param action Method to be called for the interrupt (will always be MIDI_Device_Controller::lawl())
			virtual void setupOnce(uint16_t interval, void (*action)()) = 0;
			
			/// @brief Starts the timer interrupt
			virtual void start() = 0;
			
			/// @brief Stops the timer interrupt
			virtual void stop() = 0;
	};
	
#endif