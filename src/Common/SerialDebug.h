#ifndef SerialDebug_h
	#define SerialDebug_h

	#define DEBUG_ENABLED 0
	#define PRINT_ENABLED 0
	#define ANY_OUTPUT_ENABLED (DEBUG_ENABLED || PRINT_ENABLED)
	#define FLASH_STRING_BUFFERSIZE 64

	#include <Arduino.h>

	// Functions utilizing templates and argument packs need to be defined inline here
	// Technical reason: original definition needs to be available for compiler to interpert types

	/// @private
	class SerialDebug
	{
		uint8_t _debugLevel = 0;

	#if (!defined(CORE_TEENSY) && ANY_OUTPUT_ENABLED)
		inline static FILE serial_out;
		inline static int writeChar(char c, FILE *f);	
	#endif
		
		inline static void setup()
		{
		#if (!defined(CORE_TEENSY) && ANY_OUTPUT_ENABLED)
			fdev_setup_stream(&serial_out, writeChar, NULL, _FDEV_SETUP_WRITE);
			stdout = &serial_out;
		#endif
		};
		
		inline void readToBuffer(char *buffer, const __FlashStringHelper *string)
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
			
		inline bool shouldDebug(uint8_t level)
		{
			if(_debugLevel == 0) return false;
			return _debugLevel >= level; 
		};	
		
		//Internal methods for printing
		inline void internalPrint(const char *string)
		{
		#if ANY_OUTPUT_ENABLED
			Serial.print(string);
		#endif
		};
		
		inline void internalPrintln(const char *string = "")
		{
		#if ANY_OUTPUT_ENABLED
			Serial.println(string);
		#endif
		};
			
		template<typename... Args>
		inline void internalPrint(const char *format, Args... args)
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
		inline void internalPrintln(const char *format, Args... args)
		{
		#if ANY_OUTPUT_ENABLED
			internalPrint(format, args...);
			Serial.println();
		#endif
		};
			
		public:
			SerialDebug() { setup(); };

			SerialDebug(uint8_t level)
			{
				setup();
				_debugLevel = level;
			};
		
			inline void setDebugLevel(uint8_t level) { _debugLevel = level; };
			inline uint8_t getDebugLevel() { return _debugLevel; };
		
			//Print
			//Regular strings
			inline void print(const char *string)
			{
			#if PRINT_ENABLED
				internalPrint(string);
			#endif
			};
			
			inline void println(const char *string = "")
			{
			#if PRINT_ENABLED
				internalPrintln(string);
			#endif
			};
			
			template<typename... Args>
			inline void print(const char *format, Args... args)
			{			
			#if PRINT_ENABLED
				internalPrint(format, args...);
			#endif
			};
			
			template<typename... Args>
			inline void println(const char *format, Args... args)
			{
			#if PRINT_ENABLED
				internalPrintln(format, args...);
			#endif
			};
			
			//Print
			//Strings from flash
			inline void print(const __FlashStringHelper *string)
			{
			#if PRINT_ENABLED
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrint(buffer);
			#endif
			};
			
			void println(const __FlashStringHelper *string)
			{		
			#if PRINT_ENABLED
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrintln(buffer);
			#endif
			};
			
			template<typename... Args>
			inline void print(const __FlashStringHelper *format, Args... args)
			{
			#if PRINT_ENABLED
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrint(buffer, args...);
			#endif
			};
			
			template<typename... Args>
			inline void println(const __FlashStringHelper *format, Args... args)
			{
			#if PRINT_ENABLED
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrintln(buffer, args...);
			#endif
			};

			//Debug
			//Regular strings
			inline void debug(uint8_t level, const char *string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrint(string);
			#endif
			};
			
			void debugln(uint8_t level, const char *string = "")
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrintln(string);
			#endif
			};
			
			template<typename... Args>
			inline void debug(uint8_t level, const char *format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrint(format, args...);
			#endif
			};
			
			template<typename... Args>
			inline void debugln(uint8_t level, const char *format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				internalPrintln(format, args...);
			#endif
			};
			
			//Debug
			//Strings from flash
			inline void debug(uint8_t level, const __FlashStringHelper *string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrint(buffer);
			#endif
			};
			
			void debugln(uint8_t level, const __FlashStringHelper *string)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, string);
				internalPrintln(buffer);
			#endif
			};
			
			template<typename... Args>
			void debug(uint8_t level, const __FlashStringHelper *format, Args... args)
			{
			#if DEBUG_ENABLED
				if(!shouldDebug(level)) return;
				char buffer[FLASH_STRING_BUFFERSIZE];
				readToBuffer(buffer, format);
				internalPrint(buffer, args...);
			#endif
			};
			
			template<typename... Args>
			void debugln(uint8_t level, const __FlashStringHelper *format, Args... args)
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