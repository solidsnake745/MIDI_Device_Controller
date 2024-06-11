#include "SerialDebug.h"

SerialDebug::SerialDebug()
{
#if (!defined(CORE_TEENSY) && ANY_OUTPUT_ENABLED)
	setup();
#endif
}

SerialDebug::SerialDebug(uint8_t level)
{
#if (!defined(CORE_TEENSY) && ANY_OUTPUT_ENABLED)
	setup();
#endif
	_debugLevel = level;
}

void SerialDebug::setDebugLevel(uint8_t level)
{
	_debugLevel = level;
}

uint8_t SerialDebug::getDebugLevel()
{
	return _debugLevel;
}

#if (!defined(CORE_TEENSY) && ANY_OUTPUT_ENABLED)

	FILE SerialDebug::serial_out;

	int SerialDebug::writeChar(char c, FILE *f) 
	{
		return !Serial.write(c);
	}

	void SerialDebug::setup()
	{
		fdev_setup_stream(&serial_out, writeChar, NULL, _FDEV_SETUP_WRITE);
		stdout = &serial_out;
	}

#endif	

bool SerialDebug::shouldDebug(uint8_t level) 
{ 
	if(_debugLevel == 0) return false;
	return _debugLevel >= level; 
}

void SerialDebug::readToBuffer(char *buffer, const __FlashStringHelper *string)
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
}

void SerialDebug::print(const char *string)
{
#if PRINT_ENABLED
	Serial.print(string);
#endif
}

void SerialDebug::println(const char *string)
{
#if PRINT_ENABLED
	Serial.println(string);
#endif
}

void SerialDebug::print(const __FlashStringHelper *string)
{
#if PRINT_ENABLED
	char buffer[FLASH_STRING_BUFFERSIZE];
	readToBuffer(buffer, string);
	print(buffer);
#endif
}

void SerialDebug::println(const __FlashStringHelper *string)
{		
#if PRINT_ENABLED
	char buffer[FLASH_STRING_BUFFERSIZE];
	readToBuffer(buffer, string);
	println(buffer);
#endif
}

void SerialDebug::debug(uint8_t level, const char *string) 
{
#if DEBUG_ENABLED
	if(!shouldDebug(level)) return;
	print(string);
#endif
}

void SerialDebug::debugln(uint8_t level, const char *string) 
{
#if DEBUG_ENABLED
	if(!shouldDebug(level)) return;
	println(string);
#endif
}

void SerialDebug::debug(uint8_t level, const __FlashStringHelper *string) 
{
#if DEBUG_ENABLED
	if(!shouldDebug(level)) return;
	print(string);
#endif
}

void SerialDebug::debugln(uint8_t level, const __FlashStringHelper *string) 
{
#if DEBUG_ENABLED
	if(!shouldDebug(level)) return;
	println(string);
#endif
}