#include "MIDI_SquareWave_Direction.h"
#include "../MIDI_Device_Controller.h" //Need the definition of startPlaying()

void MIDI_SquareWave_Direction::testInterrupt()
{
	//Nothing to test if no valid step output set
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Output not setup"), _id);
		return;
	}
	
	if(_parent) _parent->startPlaying();

	//Initial state should be off, set it if not
	if(_outIO->getOutput(_outNum))
	{
		_outIO->setOutput(_outNum, LOW);
		delayMicroseconds(50);
	}
	
	uint32_t t = millis();
	while(millis() - t < 3000)
	{		
		cycleOutput(); //Toggle on (step)
		delayMicroseconds(MANUAL_CHANGE_DELAY);
		cycleOutput(); //Toggle back off
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}
	
	if(_parent) _parent->stopPlaying();
}

void MIDI_SquareWave_Direction::testDirect()
{
	//Nothing to test if no valid step output set
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Output not setup"), _id);
		return;
	}
	
	//Initial state should be off, set it if not
	if(_outIO->getOutput(_outNum))
	{
		_outIO->setOutput(_outNum, LOW);
		delayMicroseconds(50);
	}
	
	uint32_t t = millis();
	while(millis() - t < 3000)
	{		
		cycleOutput(); //Toggle on (step)
		_outIO->updateOutputs();
		if(_dirOutIO && _dirOutIO != _outIO) _dirOutIO->updateOutputs();
		delayMicroseconds(MANUAL_CHANGE_DELAY);
		
		cycleOutput(); //Toggle back off
		_outIO->updateOutputs();
		if(_dirOutIO && _dirOutIO != _outIO) _dirOutIO->updateOutputs();
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}
}

void MIDI_SquareWave_Direction::testMaxDirection()
{
	//Nothing to test if no valid step output set
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Output not setup"), _id);
		return;
	}
	
	//Nothing to test if no valid max position set
	if(!isTrackingPosition()) 
	{
		_debug.debugln(7, F("%d - Not tracking position"), _id);
		return;
	}
	
	_debug.debugln(7, F("%d - Testing max direction part 1"), _id);
	if(_parent) _parent->startPlaying();
	while(!isAtMaxPosition()) 
	{
		_debug.debugln(7, F("%d - Testing max direction part 1a"), _id);
		stepOnce(true);
		_currentPosition++;
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}

	_debug.debugln(7, F("%d - Testing max direction part 2"), _id);
	toggleDirection();
	zeroPosition();

	delay(1000);
	
	_debug.debugln(7, F("%d - Testing max direction part 3"), _id);
	while(!isAtMaxPosition()) 
	{
		stepOnce(true);
		_currentPosition++;
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}

	toggleDirection();
	zeroPosition();
	if(_parent) _parent->stopPlaying();
}

void MIDI_SquareWave_Direction::testStepInterrupt(uint32_t steps, bool direction)
{
	//Nothing to test if no valid step output set
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Output not setup"), _id);
		return;
	}
	
	setDirection(direction);
	if(_parent) _parent->startPlaying();

	//Initial state should be off, set it if not
	if(_outIO->getOutput(_outNum))
	{
		_outIO->setOutput(_outNum, LOW);
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}
				
	while(steps-- > 0)
	{		
		_outIO->toggleOutput(_outNum); //Toggle on (step)
		delayMicroseconds(MANUAL_CHANGE_DELAY);
		_outIO->toggleOutput(_outNum); //Toggle back off
		
		if(isTrackingPosition())
		{
			_currentPosition++;
			if(isAtMaxPosition())
			{
				toggleDirection();
				zeroPosition();
			}
		}
		
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}
	
	if(_parent) _parent->stopPlaying();
}

void MIDI_SquareWave_Direction::testStepDirect(uint32_t steps, bool direction)
{
	// Nothing to test if no valid step output set
	if(!_outIO)
	{
		_debug.debugln(7, F("%d - Output not setup"), _id);
		return;
	}
	
	setDirection(direction);
	while(steps-- > 0)
	{
		stepOnce(true);
		
		if(isTrackingPosition())
		{
			_currentPosition++;
			if(isAtMaxPosition())
			{
				toggleDirection();
				zeroPosition();
			}
		}
		
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}
}