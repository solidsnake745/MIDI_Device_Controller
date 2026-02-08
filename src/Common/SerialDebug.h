#ifndef SerialDebug_h
	#define SerialDebug_h
	
	//This is a precompiled header therefore these defines must be resolved here
	#define DEBUG_ENABLED 1
	#define PRINT_ENABLED 1
	#define ANY_OUTPUT_ENABLED (DEBUG_ENABLED || PRINT_ENABLED)
	#define FLASH_STRING_BUFFERSIZE 128

	#include <Arduino.h>	

	enum LogLevel : uint16_t
	{
		OFF = 0,
		PRINT = 1,
		DEBUG = 13106,
		TRACE = 26213,
		ISR = 39320,
		ISR_TRACE = 52427,
		ALL = 65535
	};
	
	//Functions utilizing templates and argument packs need to be defined inline here
	//Technical reason: original definition needs to be available for compiler to interpret types
	
	inline const __FlashStringHelper* EMPTY_STRING = F("");	
	inline const __FlashStringHelper* TRUE_STRING = F("true");
	inline const __FlashStringHelper* FALSE_STRING = F("false");
	inline const __FlashStringHelper* toString(bool value) { return value ? TRUE_STRING : FALSE_STRING; };
	
	/// @private
	class SerialDebug
	{
		//Log level related
		uint16_t _level = 0; //Default off
		
		public:
		#if DEBUG_ENABLED
			inline bool shouldDebug(uint16_t level)
			{
				//Serial.print(level); Serial.print(" <= "); Serial.println(_level);
				//Serial.println(level <= _level ? "should debug true" : "should debug false");
				return level <= _level;
			};
			inline bool shouldDebug(LogLevel level) { return shouldDebug((uint16_t) level); };
		#else
			constexpr bool shouldDebug(uint16_t level) { return false; };
			constexpr bool shouldDebug(LogLevel level) { return false; };
		#endif
		#if PRINT_ENABLED
			inline bool shouldPrint() { return _level >= PRINT; };
		#else
			constexpr bool shouldPrint() { return false; };
		#endif
		
		public:
			SerialDebug() { setup(); };
			SerialDebug(LogLevel level) : SerialDebug() { _level = (uint16_t) level; };
			SerialDebug(uint16_t level) : SerialDebug() { _level = level; };
		
			inline void setDebugLevel(uint16_t level) { _level = level; };
			inline uint16_t getDebugLevel() { return _level; };
		
		//Serial out related
		private:
		#if (!defined(CORE_TEENSY) && ANY_OUTPUT_ENABLED)
			inline static FILE serial_out;
			inline static int writeChar(char c, FILE* f) { return !Serial.write(c); };
		#endif
			
			inline static void setup()
			{
			#if (!defined(CORE_TEENSY) && ANY_OUTPUT_ENABLED)
				fdev_setup_stream(&serial_out, writeChar, NULL, _FDEV_SETUP_WRITE);
				stdout = &serial_out;
			#endif
			};
			
			inline void readToBuffer(char* buffer, const __FlashStringHelper* string)
			{
				PGM_P p = reinterpret_cast<PGM_P>(string);
			
				//Read out the string from flash into our passed in buffer
				uint16_t i = 0;
				while (i < FLASH_STRING_BUFFERSIZE) 
				{
					char c = pgm_read_byte(p++);	
					buffer[i++] = c;
					if (c == 0) break;
				}
			};
			//Internal methods for printing
			inline void internalPrint(const char* string)
			{
			#if ANY_OUTPUT_ENABLED
				Serial.print(string);
			#endif
			};
			
			inline void internalPrintln(const char* string = "")
			{
			#if ANY_OUTPUT_ENABLED
				Serial.println(string);
			#endif
			};
				
			template<typename... Args>
			inline void internalPrint(const char* format, Args... args)
			{
			#if ANY_OUTPUT_ENABLED		
				#ifdef CORE_TEENSY
					Serial.printf(format, args...);
				#else
					printf(format, args...);
				#endif
			#endif
			};
			
			template<typename... Args>
			inline void internalPrintln(const char* format, Args... args)
			{
			#if ANY_OUTPUT_ENABLED
				internalPrint(format, args...);
				Serial.println();
			#endif
			};
			
		public:
			//Print
			//Regular strings
			inline void print(const char* string)
			{
			#if PRINT_ENABLED
				if(!shouldPrint()) return;
				internalPrint(string);
			#endif
			};
			
			inline void println(const char* string = "")
			{
			#if PRINT_ENABLED
				if(!shouldPrint()) return;
				internalPrintln(string);
			#endif
			};
			
			template<typename... Args>
			inline void print(const char* format, Args... args)
			{			
			#if PRINT_ENABLED
				if(!shouldPrint()) return;
				internalPrint(format, args...);
			#endif
			};
			
			template<typename... Args>
			inline void println(const char* format, Args... args)
			{
			#if PRINT_ENABLED
				if(!shouldPrint()) return;
				internalPrintln(format, args...);
			#endif
			};
			
			//Print
			//Strings from flash
			inline void print(const __FlashStringHelper* string)
			{
			#if PRINT_ENABLED
				if(!shouldPrint()) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrint(buffer);
			#endif
			};
			
			void println(const __FlashStringHelper* string)
			{		
			#if PRINT_ENABLED
				if(!shouldPrint()) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrintln(buffer);
			#endif
			};
			
			template<typename... Args>
			inline void print(const __FlashStringHelper* format, Args... args)
			{
			#if PRINT_ENABLED
				if(!shouldPrint()) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrint(buffer, args...);
			#endif
			};
			
			template<typename... Args>
			inline void println(const __FlashStringHelper* format, Args... args)
			{
			#if PRINT_ENABLED
				if(!shouldPrint()) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrintln(buffer, args...);
			#endif
			};

			//Debug - level = uint16_t
			//Regular strings
			inline void debug(uint16_t level, const char* string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrint(string);
			#endif
			};
			
			void debugln(uint16_t level, const char* string = "")
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrintln(string);
			#endif
			};
			
			template<typename... Args>
			inline void debug(uint16_t level, const char* format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrint(format, args...);
			#endif
			};
			
			template<typename... Args>
			inline void debugln(uint16_t level, const char* format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrintln(format, args...);
			#endif
			};
			
			//Debug - level = LogLevel
			//Regular strings
			inline void debug(LogLevel level, const char* string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrint(string);
			#endif
			};
			
			void debugln(LogLevel level, const char* string = "")
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrintln(string);
			#endif
			};
			
			template<typename... Args>
			inline void debug(LogLevel level, const char* format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrint(format, args...);
			#endif
			};
			
			template<typename... Args>
			inline void debugln(LogLevel level, const char* format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrintln(format, args...);
			#endif
			};
			
			//Debug - level = uint16_t
			//Strings from flash
			inline void debug(uint16_t level, const __FlashStringHelper* string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrint(buffer);
			#endif
			};
			
			void debugln(uint16_t level, const __FlashStringHelper* string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrintln(buffer);
			#endif
			};
			
			template<typename... Args>
			void debug(uint16_t level, const __FlashStringHelper* format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrint(buffer, args...);
			#endif
			};
			
			template<typename... Args>
			void debugln(uint16_t level, const __FlashStringHelper* format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrintln(buffer, args...);
			#endif
			};
			
			//Debug - level = LogLevel
			//Strings from flash
			inline void debug(LogLevel level, const __FlashStringHelper* string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrint(buffer);
			#endif
			};
			
			void debugln(LogLevel level, const __FlashStringHelper* string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrintln(buffer);
			#endif
			};
			
			template<typename... Args>
			void debug(LogLevel level, const __FlashStringHelper* format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrint(buffer, args...);
			#endif
			};
			
			template<typename... Args>
			void debugln(LogLevel level, const __FlashStringHelper* format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrintln(buffer, args...);
			#endif
			};
			
			#if INCLUDE_TESTS
			static void runTest()
			{
				Serial.print("DEBUG_ENABLED: "); Serial.println(DEBUG_ENABLED);
				Serial.print("PRINT_ENABLED: "); Serial.println(PRINT_ENABLED);
				Serial.print("ANY_OUTPUT_ENABLED: "); Serial.println(ANY_OUTPUT_ENABLED);
				
				SerialDebug test(5);
				Serial.println(F("Testing each output method - One per line"));
							
				//Print regular strings
				test.print("print\tregular");
				#if PRINT_ENABLED 
					Serial.println(); 
				#endif
				test.println("println\tregular");
				test.print("print\tregular format %d, %d, %d", 1, 2, 3);
				#if PRINT_ENABLED 
					Serial.println(); 
				#endif
				test.println("println\tregular format %d, %d, %d", 1, 2, 3);

				//Print flash strings
				test.print(F("print\tflash"));
				#if PRINT_ENABLED 
					Serial.println(); 
				#endif
				test.println(F("println\tflash"));
				test.print(F("print\tflash format %d, %d, %d"), 1, 2, 3);
				#if PRINT_ENABLED 
					Serial.println(); 
				#endif
				test.println(F("println\tflash format %d, %d, %d"), 1, 2, 3);
				
				//Debug regular strings
				test.debug(1, "debug\tregular");
				#if DEBUG_ENABLED
					Serial.println();
				#endif
				test.debugln(1, "debugln\tregular");
				test.debug(1, "debug\tregular format %d, %d, %d", 1, 2, 3);
				#if DEBUG_ENABLED
					Serial.println();
				#endif
				test.debugln(1, "debugln\tregular format %d, %d, %d", 1, 2, 3);

				//Debug flash strings
				test.debug(1, F("debug\tflash"));
				#if DEBUG_ENABLED
					Serial.println();
				#endif
				test.debugln(1, F("debugln\tflash"));
				test.debug(1, F("debug\tflash format %d, %d, %d"), 1, 2, 3);
				#if DEBUG_ENABLED
					Serial.println();
				#endif
				test.debugln(1, F("debugln\tflash format %d, %d, %d"), 1, 2, 3);
			};
			#endif
	};
#endif