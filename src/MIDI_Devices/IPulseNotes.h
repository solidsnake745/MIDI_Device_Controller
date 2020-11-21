#ifndef IPulseNotes_h
	#define IPulseNotes_h
	
	#include <stdint.h>
	
	/// @private
	class IPulseNotes
	{	
		public:
			virtual bool isValidMapping(uint8_t out) = 0;
			virtual void pulseOutput(uint8_t out) = 0;
			virtual void stopOutput(uint8_t out) = 0;
			virtual void stopOutputs() = 0;
			virtual void playNotes() = 0;
	};
	
#endif