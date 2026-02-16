#include "NoteDuration.h"

NoteDuration::NoteDuration(uint16_t us, uint16_t ms, uint16_t s)
{
	addSeconds(s);
	addMillis(ms);
	addMicros(us);
}

bool NoteDuration::operator <=(NoteDuration& nd)
{
	if (_seconds != nd._seconds)
		return _seconds < nd._seconds;
	
	if (_millis != nd._millis)
		return _millis < nd._millis;

	return _micros <= nd._micros;
};

void NoteDuration::addSeconds(uint16_t s) { addMillis(s * 1000); }
void NoteDuration::addMillis(uint32_t ms) { addMicros(ms * 1000); }

void NoteDuration::addMicros(uint32_t us)
{
	//Add passed in value
	if(us >= 1000000)
	{
		auto dv = std::div(us, 1000000);
		_seconds += dv.quot;
		us = dv.rem;
	}

	if(us >= 1000)
	{
		auto dv = std::div(us, 1000);
		_millis += dv.quot;
		us = dv.rem;
	}

	_micros += us;
	
	//Rollover updated duration
	if(_micros >= 1000)
	{
		auto dv = std::div(_micros, 1000);
		_millis += dv.quot;
		_micros = dv.rem;
	}
	
	if(_millis >= 1000)
	{
		auto dv = std::div(_millis, 1000);
		_seconds += dv.quot;
		_millis = dv.rem;
	}
}