#include "SerialDebug.h"

SerialDebug::SerialDebug()
{
	setup();
}

SerialDebug::SerialDebug(uint8_t level)
{
	setup();
	_debugLevel = level;
}

#if (!defined(CORE_TEENSY) && ANY_OUTPUT_ENABLED)
	FILE SerialDebug::serial_out;
	int SerialDebug::writeChar(char c, FILE *f) { return !Serial.write(c); }
#endif

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