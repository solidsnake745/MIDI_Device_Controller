#ifndef IPulseNotes_h
	#define IPulseNotes_h
	
	#include <stdint.h>
	
	/// @private
	class IPulseNotes
	{	
		public:
			virtual void pulseOutput(uint8_t out);
			virtual void stopOutput(uint8_t out);
			virtual void stopOutputs();
	};
	
#endif