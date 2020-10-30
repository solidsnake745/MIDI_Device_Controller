#include "noteDurationTracker.h"

noteDurationTracker::noteDurationTracker()
{

}

void noteDurationTracker::addMicroseconds(uint32_t s)
{
	_duration.microsec += s;
	if(_duration.microsec < 1000)
		return;

	while(_duration.microsec > 1000)
	{
		_duration.microsec -= 1000;
		_duration.millisec += 1;
	}

	if(_duration.millisec < 1000)
		return;

	while(_duration.millisec > 1000)
	{
		_duration.millisec -= 1000;
		_duration.seconds += 1; 
	}
}