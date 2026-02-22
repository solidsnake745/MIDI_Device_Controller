#ifndef ByteNoteRegister_h
	#define ByteNoteRegister_h

	#if INCLUDE_TESTS
		#include "../Settings.h"
		#include "SerialDebug.h"
	#endif
		
	/// @brief Class to simplify storing a byte and manipulating its bits
	/// @details Used in various IO devices as storage for output states.<br>
	/// Was originally meant to include more note related information, but that was later refactored out.<br>
	/// Should probably be renamed to ByteRegister.
	class ByteNoteRegister
	{
		uint8_t _data = 0;
		uint8_t _invert = 0;
			
		public:
			/// @brief Default constructor
			ByteNoteRegister() {};			
			
			/// @brief Retrieve the entire register's value as a byte
			inline uint8_t getByteValue() 
			{ 
				//If there any bits set to be inverted, return the inverted value
				return _invert > 0 ? ((~_data & _invert) + (_data & ~_invert)) : _data;
			};
			
			/// @brief Gets the specified bit
			/// @param i (Zero) Index of the bit to get
			inline bool getBit(uint8_t i) { return bitRead(_data, i); };			
			
			/// @brief Gets the specified bit and applies the invert setting
			/// @param i (Zero) Index of the bit to get
			inline bool getBitValue(uint8_t i)
			{ 
				bool value = bitRead(_data, i);
				return isInverted(i) ? !value : value;
			};
			
			/// @brief Sets the specified bit to 1
			/// @param i (Zero) Index of the bit to set
			inline void setBit(uint8_t i) { bitSet(_data, i); };
			
			/// @brief Sets the specified bit to 0
			/// @param i (Zero) Index of the bit to set
			inline void clearBit(uint8_t i)	{ bitClear(_data, i); };
			
			/// @brief Sets the specified bit's value
			/// @param i (Zero) Index of the bit to set
			/// @param value Value to set it to
			inline void setBitValue(uint8_t i, bool value) { value ? setBit(i) : clearBit(i); };		
			
			/// @brief Toggles the specified bit's value
			/// @param i (Zero) Index of the bit to toggle
			inline void toggleBit(uint8_t i) { setBitValue(i, !bitRead(_data, i)); }
			
			/// @brief Gets the specified bit's inverted setting
			/// @param i (Zero) Index of the bit to get
			/// @retval true if inverted
			inline bool isInverted(uint8_t i) { return bitRead(_invert, i); };
			
			/// @brief Sets the specified bit's inverted setting
			/// @param i (Zero) Index of the bit to set
			/// @param value Value to set it to
			inline void setInverted(uint8_t i, bool value) { value ? bitSet(_invert, i) : bitClear(_invert, i); };
			
		#if INCLUDE_TESTS
		private:			
			inline static SerialDebug _debug = SerialDebug(DEBUG_BYTENOTEREGISTER);
			
			inline static void printBits(uint8_t value)
			{
				for(int x = 7; x >= 0; x--) 
					_debug.println("%d", bitRead(value, x));
			}
		
		public: 
			inline static void runTest()
			{
				//getBit/setBit tests
				ByteNoteRegister test;			
				//_data should default to 0 at this point
				_debug.println(F("Initial value of _data before setting: %d"), test.getByteValue());
				for(int x = 0; x < 8; x++)
				{
					test._data = 0;
					test.setBit(x);
					_debug.println(F("Bit %d set: %d\t"), x, test.getByteValue());
					printBits(test.getByteValue());
					_debug.println();
				}
				
				test._data = 255;
				_debug.println(F("Initial value of _data before clearing: %d"), test.getByteValue());
				for(int x = 0; x < 8; x++)
				{
					test._data = 255;
					test.clearBit(x);
					_debug.println(F("Bit %d cleared: %d (255 - %d)\t"), x, test.getByteValue(), 1 << x);
					printBits(test.getByteValue());
					_debug.println();
				}
				
				//Invert tests
				_debug.println(F("Invert logic confirmation tests"));
				
				test._data = B10101010;
				uint8_t invertMask = B01010101;
				
				_debug.println(F("Initial value:\t\t")); printBits(test._data); _debug.println();
				_debug.println(F("Invert Mask:\t\t")); printBits(invertMask); _debug.println();
				
				//Inverted portion (Invert and then mask with invert mask) + non-inverted portion (Mask with inverted invert mask)				
				uint8_t invertedPortion = (~test._data & invertMask);
				uint8_t nonInvertedPortion = (test._data & ~invertMask);
				_debug.println(F("Inverted portion:\t")); printBits(invertedPortion); _debug.println();
				_debug.println(F("Non-inverted portion:\t")); printBits(nonInvertedPortion); _debug.println();
				
				//Combination should result in all 1s
				_debug.println(F("Combined:\t\t")); printBits(invertedPortion + nonInvertedPortion); _debug.println();
				
				test._invert = B11110000;
				_debug.println(F("Initial value of _invert before setting: %d ("), test._invert);
				printBits(test._invert);
				_debug.println(F(")"));
				
				for(int x = 0; x < 8; x++)
				{
					//setInverted basically uses the same logic as setBit/clearBit
					//So no need to test every index - testing each path on half the values
					test._invert = B11110000;
					if(x >= 4)
					{
						test.setInverted(x, false);
						_debug.println(F("Bit %d cleared: %d\t"), x, test._invert);
					}
					else
					{
						test.setInverted(x, true);
						_debug.println(F("Bit %d set: %d\t\t"), x, test._invert);
					}
					
					printBits(test._invert);
					_debug.println();
				}
				
				_debug.println(F("Invert top 4 bits using _invert setting"));
				test._data = B00001111;
				test._invert = B11110000;
				_debug.println(F("Initial value:\t")); printBits(test._data); _debug.println();
				_debug.println(F("Invert Mask:\t")); printBits(test._invert); _debug.println();
				_debug.println(F("Result:\t\t")); printBits(test.getByteValue()); _debug.println();
				
				_debug.println(F("Invert bottom 4 bits using _invert setting"));
				test._data = B00001111;
				test._invert = B00001111;
				_debug.println(F("Initial value:\t")); printBits(test._data); _debug.println();
				_debug.println(F("Invert Mask:\t")); printBits(test._invert); _debug.println();
				_debug.println(F("Result:\t\t")); printBits(test.getByteValue()); _debug.println();
				
				_debug.println(F("Invert middle 4 bits using _invert setting"));
				test._data = B00001111;
				test._invert = B00111100;
				_debug.println(F("Initial value:\t")); printBits(test._data); _debug.println();
				_debug.println(F("Invert Mask:\t")); printBits(test._invert); _debug.println();
				_debug.println(F("Result:\t\t")); printBits(test.getByteValue()); _debug.println();
				
				_debug.println(F("Use getBitValue to print the same result from the last test"));
				_debug.println(F("getBitValue:\t"));
				for(int x = 7; x >= 0; x--) _debug.println("%d", test.getBitValue(x));
				_debug.println();
			}
		#endif
	};
	
#endif