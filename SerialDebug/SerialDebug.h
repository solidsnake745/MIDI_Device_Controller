#ifndef SerialDebug_h
#define SerialDebug_h

#define DEBUG_ENABLED 1
#define FLASH_STRING_BUFFERSIZE 64

#include <Arduino.h>	

// Functions utilizing templates and argument packs need to be defined inline here
// Technical reason: original definition needs to be available for compiler to interpert types

class SerialDebug
{
#ifndef CORE_TEENSY
	static FILE serial_out;
	static int writeChar(char c, FILE *f);
	static void setup();
#endif
	
	uint8_t _debugLevel = 0;
	bool shouldDebug(uint8_t level);
	void readToBuffer(char *buffer, const __FlashStringHelper *string);
	
	public:
		SerialDebug();
		SerialDebug(uint8_t level);
	
		void setDebugLevel(uint8_t level);
		uint8_t getDebugLevel();
	
		//Print
		//Regular strings
		void print(const char *string);		
		void println(const char *string = "");
		
		template<typename... Args>
		inline void print(const char *format, Args... args)
		{			
		#ifdef CORE_TEENSY
			Serial.printf(format, args...);
		#else
			printf(format, args...);
		#endif
		};
		
		template<typename... Args>
		inline void println(const char *format, Args... args)
		{
			print(format, args...);
			Serial.println();
		};
		
		//Strings from flash
		void print(const __FlashStringHelper *string);
		void println(const __FlashStringHelper *string);
		
		template<typename... Args>
		inline void print(const __FlashStringHelper *format, Args... args)
		{
			char buffer[FLASH_STRING_BUFFERSIZE];
			readToBuffer(buffer, format);
			print(buffer, args...);
		};
		
		template<typename... Args>
		inline void println(const __FlashStringHelper *format, Args... args)
		{
			print(format, args...);
			Serial.println();
		};

		//Debug
		//Regular strings
		void debug(uint8_t level, const char *string);
		void debugln(uint8_t level, const char *string = "");
		
		template<typename... Args>
		inline void debug(uint8_t level, const char *format, Args... args)
		{
		#if DEBUG_ENABLED
			if(!shouldDebug(level)) return;
			print(format, args...);
		#endif
		};
		
		template<typename... Args>
		inline void debugln(uint8_t level, const char *format, Args... args)
		{
		#if DEBUG_ENABLED
			if(!shouldDebug(level)) return;
			println(format, args...);
		#endif
		};
		
		//Debug
		//Strings from flash
		void debug(uint8_t level, const __FlashStringHelper *string);
		void debugln(uint8_t level, const __FlashStringHelper *string);
		
		template<typename... Args>
		void debug(uint8_t level, const __FlashStringHelper *format, Args... args)
		{
		#if DEBUG_ENABLED
			if(!shouldDebug(level)) return;
			print(format, args...);
		#endif
		};
		
		template<typename... Args>
		void debugln(uint8_t level, const __FlashStringHelper *format, Args... args)
		{
		#if DEBUG_ENABLED
			if(!shouldDebug(level)) return;
			println(format, args...);
		#endif
		};
};

#endif