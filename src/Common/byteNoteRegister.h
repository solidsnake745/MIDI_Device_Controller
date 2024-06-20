#ifndef ByteNoteRegister_h
	#define ByteNoteRegister_h

	#include "NoteDuration.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"

	class ByteNoteRegister
	{
		inline static SerialDebug _debug = SerialDebug(DEBUG_BYTENOTEREGISTER);
		
		uint8_t _data = 0;
		NoteDuration _durations[8];
		NoteDuration _maxDurations[8];
			
		public:
			ByteNoteRegister() {};
			
			#pragma GCC push_options
			#pragma GCC optimize("Ofast")
			inline void updateDurations(uint8_t period)
			{
				for(int x = 0; x < 8; x++)
				{
					if(_maxDurations[x].isZero() || !getBit(x))
						continue;

					_durations[x].addMicros(period);
				}
			};
			#pragma GCC pop_options
		
			inline uint8_t getByteValue() { return _data; };
			inline bool getBit(uint8_t i) { return bitRead(_data, i); };
			inline void setBit(uint8_t i) { bitSet(_data, i); };
		  
			inline void clearBit(uint8_t i)
			{
				bitClear(_data, i);
				_durations[i].reset();
			};
		  
			inline void setBitValue(uint8_t i, bool value) { value ? setBit(i) : clearBit(i); };		
		  
			inline void setMaxDuration(uint8_t i, uint32_t us)
			{ 
				_maxDurations[i].reset(); 
				_maxDurations[i].addMicros(us); 
			};
			
			inline bool isPastMaxDuration(uint8_t i)
			{
				if(_maxDurations[i].isZero())
					return false;
				
				return _maxDurations[i] < _durations[i];			
			};
			
			//TODO: This may not be necessary as isPastMaxDuration and related functions are available and provide more control over the process
			inline void checkMaxDuration()
			{
				for(int x = 0; x < 8; x++)
				{
					if(_maxDurations[x].isZero())
					{
						_debug.debugln(30, F("ByteNoteRegister: %d - No max duration"), x);
						continue;
					}
					
					if(_durations[x] < _maxDurations[x])
					{
						_debug.debugln(30, F("ByteNoteRegister: %d - Duration less than max"), x);
						continue;
					}					

					_debug.debugln(20, F("ByteNoteRegister: %d - Reached max duration"), x);
					clearBit(x);
				}
			};
			
			#if INCLUDE_TESTS
			inline static void runTest()
			{
				ByteNoteRegister test;
				_debug.debugln(5, F("Initial value of _data: %d"), test.getByteValue());
				for(int x = 0; x < 8; x++)
				{
					test._data = 0;
					test.setBit(x);
					_debug.debugln(5, F("Value after bit %d set: %d"), x, test.getByteValue());
				}
			}
			#endif
	};
  
#endif
