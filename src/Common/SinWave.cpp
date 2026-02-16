#include "SinWave.h"

float SinWave::getSinValue(uint16_t d)
{
	//Handle invalid values
	if(d > 360)
		d = 360;

	//First quadrant
	if(d <= 90)
		return getFirstQuadValue(d);

	//Second quadrant (mirror first quadrant)
	if(d > 90 && d <= 180)
	{
		d = 90 - (d - 90);
		return getFirstQuadValue(d);
	}

	//Third quadrant (invert first)
	if(d > 180 && d <= 270) 
	{
		d -= 180;
		return -getFirstQuadValue(d);
	}

	//Fourth quadrant (mirror and invert first)
	//if(d > 270 && d <= 360) { }
	d = 90 - (d - 270);
	return -getFirstQuadValue(d);
}

void SinWave::printSinValues() 
{ 
	for(uint16_t i = 0; i <= 360; i++) 
		_debug.println(F("Sin for %d: %6f"), i, getSinValue(i));
}