#include "MIDI_SquareWave.h"
#include "../MIDI_Device_Controller.h" //Need the definition of startPlaying()

void MIDI_SquareWave::testInterrupt()
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
		_outIO->toggleOutput(_outNum); //Toggle on (step)
		delayMicroseconds(MANUAL_CHANGE_DELAY);
		_outIO->toggleOutput(_outNum); //Toggle back off
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}
	
	if(_parent) _parent->stopPlaying();
}

void MIDI_SquareWave::testDirect()
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
		_outIO->toggleOutput(_outNum); //Toggle on (step)
		_outIO->updateOutputs();
		delayMicroseconds(MANUAL_CHANGE_DELAY);
		
		_outIO->toggleOutput(_outNum); //Toggle back off
		_outIO->updateOutputs();
		delayMicroseconds(MANUAL_CHANGE_DELAY);
	}
}