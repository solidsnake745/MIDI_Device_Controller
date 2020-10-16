#include "TimerOne_Timer.h"

void TimerOne_Timer::setupOnce(uint16_t interval, void (*action)())
{
	if(_isSetup) return;
	
	Timer1.initialize();
	Timer1.attachInterrupt(action);
	Timer1.setPeriod(interval);
}

void TimerOne_Timer::start()
{
	Timer1.start();
}

void TimerOne_Timer::stop()
{
	Timer1.stop();
}