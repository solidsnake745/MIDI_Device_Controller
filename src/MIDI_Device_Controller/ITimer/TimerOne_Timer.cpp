#include "TimerOne_Timer.h"

void TimerOne_Timer::setupOnce(uint16_t interval, void (*action)())
{
	
	if(_isSetup) return;
	
	Timer1.initialize();
	Timer1.attachInterrupt(action);
	
	//There's a possible problem here if the interval is changed for some reason
	//Since setupOnce is only executed once and exits out on subsequent calls, setPeriod will never be called with the updated value
	//Shouldn't happen realistically as resolution is kind of a constant, but it is technically possible to change it
	//TODO: Address this potential problem if it actually matters
	Timer1.setPeriod(interval);
	
	_isSetup = true;
}

void TimerOne_Timer::start()
{
	Timer1.start();
}

void TimerOne_Timer::stop()
{
	Timer1.stop();
}