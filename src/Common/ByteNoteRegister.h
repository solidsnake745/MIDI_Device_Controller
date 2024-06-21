#ifndef ByteNoteRegister_h
	#define ByteNoteRegister_h

	#include "NoteDuration.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"

	class ByteNoteRegister
	{
		inline static SerialDebug _debug = SerialDebug(DEBUG_BYTENOTEREGISTER);
		
		uint8_t _data = 0;
		uint8_t _invert = 0;
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
		
			inline uint8_t getByteValue() 
			{ 
				if(_invert > 0)
					return ((~_data & _invert) + (_data & ~_invert));
				
				return _data;
			};
			
			inline void setBit(uint8_t i) { bitSet(_data, i); };
			inline bool getBit(uint8_t i) { return bitRead(_data, i); };			
			inline bool getBitValue(uint8_t i)
			{ 
				bool value = bitRead(_data, i);
				return isInverted(i) ? !value : value;
			};
			
			inline bool isInverted(uint8_t i) { return bitRead(_invert, i); };
			inline void setInverted(uint8_t i, bool value)
			{
				if(value)
					bitSet(_invert, i);
				else
					bitClear(_invert, i);
			};
			
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
		private:
			inline static void printBits(uint8_t value)
			{
				for(int x = 7; x >= 0; x--) 
					_debug.debug(5, "%d", bitRead(value, x));
			}
		
		public: 
			inline static void runTest()
			{
				//getBit/setBit tests
				ByteNoteRegister test;			
				//_data should default to 0 at this point
				_debug.debugln(5, F("Initial value of _data before setting: %d"), test.getByteValue());
				for(int x = 0; x < 8; x++)
				{
					test._data = 0;
					test.setBit(x);
					_debug.debug(5, F("Bit %d set: %d\t"), x, test.getByteValue());
					printBits(test.getByteValue());
					_debug.debugln(5);
				}
				
				test._data = 255;
				_debug.debugln(5, F("Initial value of _data before clearing: %d"), test.getByteValue());
				for(int x = 0; x < 8; x++)
				{
					test._data = 255;
					test.clearBit(x);
					_debug.debug(5, F("Bit %d cleared: %d (255 - %d)\t"), x, test.getByteValue(), 1 << x);
					printBits(test.getByteValue());
					_debug.debugln(5);
				}
				
				//Invert tests
				_debug.debugln(5, F("Invert logic confirmation tests"));
				
				test._data = B10101010;
				uint8_t invertMask = B01010101;
				
				_debug.debug(5, F("Initial value:\t\t")); printBits(test._data); _debug.debugln(5);
				_debug.debug(5, F("Invert Mask:\t\t")); printBits(invertMask); _debug.debugln(5);
				
				//Inverted portion (Invert and then mask with invert mask) + non-inverted portion (Mask with inverted invert mask)				
				uint8_t invertedPortion = (~test._data & invertMask);
				uint8_t nonInvertedPortion = (test._data & ~invertMask);
				_debug.debug(5, F("Inverted portion:\t")); printBits(invertedPortion); _debug.debugln(5);
				_debug.debug(5, F("Non-inverted portion:\t")); printBits(nonInvertedPortion); _debug.debugln(5);
				
				//Combination should result in all 1s
				_debug.debug(5, F("Combined:\t\t")); printBits(invertedPortion + nonInvertedPortion); _debug.debugln(5);
				
				test._invert = B11110000;
				_debug.debug(5, F("Initial value of _invert before setting: %d ("), test._invert);
				printBits(test._invert);
				_debug.debugln(5, F(")"));
				
				for(int x = 0; x < 8; x++)
				{
					//setInverted basically uses the same logic as setBit/clearBit
					//So no need to test every index - testing each path on half the values
					test._invert = B11110000;
					if(x >= 4)
					{
						test.setInverted(x, false);
						_debug.debug(5, F("Bit %d cleared: %d\t"), x, test._invert);
					}
					else
					{
						test.setInverted(x, true);
						_debug.debug(5, F("Bit %d set: %d\t\t"), x, test._invert);
					}
					
					printBits(test._invert);
					_debug.debugln(5);
				}
				
				_debug.debugln(5, F("Invert top 4 bits using _invert setting"));
				test._data = B00001111;
				test._invert = B11110000;
				_debug.debug(5, F("Initial value:\t")); printBits(test._data); _debug.debugln(5);
				_debug.debug(5, F("Invert Mask:\t")); printBits(test._invert); _debug.debugln(5);
				_debug.debug(5, F("Result:\t\t")); printBits(test.getByteValue()); _debug.debugln(5);
				
				_debug.debugln(5, F("Invert bottom 4 bits using _invert setting"));
				test._data = B00001111;
				test._invert = B00001111;
				_debug.debug(5, F("Initial value:\t")); printBits(test._data); _debug.debugln(5);
				_debug.debug(5, F("Invert Mask:\t")); printBits(test._invert); _debug.debugln(5);
				_debug.debug(5, F("Result:\t\t")); printBits(test.getByteValue()); _debug.debugln(5);
				
				_debug.debugln(5, F("Invert middle 4 bits using _invert setting"));
				test._data = B00001111;
				test._invert = B00111100;
				_debug.debug(5, F("Initial value:\t")); printBits(test._data); _debug.debugln(5);
				_debug.debug(5, F("Invert Mask:\t")); printBits(test._invert); _debug.debugln(5);
				_debug.debug(5, F("Result:\t\t")); printBits(test.getByteValue()); _debug.debugln(5);
				
				_debug.debugln(5, F("Use getBitValue to print the same result from the last test"));
				_debug.debug(5, F("getBitValue:\t"));
				for(int x = 7; x >= 0; x--) _debug.debug(5, "%d", test.getBitValue(x));
				_debug.debugln(5);
			}
		#endif
	};
#endif