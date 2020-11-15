#include "MIDI_SN74HC595N.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

SerialDebug MIDI_SN74HC595N::_debug(DEBUG_SN74HC595N);

MIDI_SN74HC595N::MIDI_SN74HC595N(uint8_t numRegisters, uint8_t latchPin) 
{
	if(numRegisters == 0)
		numRegisters = 1;
	
	//Save settings
	_numRegisters = numRegisters; //Number of 8 bit shift registers
	_maxOutput = (numRegisters * 8) - 1;
	
	//Initialize collections
	_registers = new byteNoteRegister[_numRegisters];
	for(int x = 0; x < _numRegisters; x++)
		_registers[x] = byteNoteRegister();
	
	//Setup SPI
	setLatchPin(latchPin);
	SPI.begin();
	
	//0 out all configured shift registers
	for(int x = 0; x < _numRegisters; x++)
		SPI.transfer(0);
	latchRegisters();
};

void MIDI_SN74HC595N::testRegistersDirect()
{	
	//Enable each output on each register gradually
	_debug.println(F("Testing each register's individual outputs"));
	
	for(int x = 0; x < 8; x++)
	{
		int z = 1 << x;
		_debug.println(F("Testing output %d"), x);
		
		for(int y = 0; y < _numRegisters; y++)
		{
			SPI.transfer(z);
			latchRegisters();			
		}
		
		delay(50);
	}
	
	//Clear registers
	for(int y = 0; y < _numRegisters; y++)
	{
		SPI.transfer(0);
		latchRegisters();
	}
}

void MIDI_SN74HC595N::testRegistersInterrupt()
{	
	_belongsTo->startPlaying();
	for(int x = 0; x <= _maxOutput; x++)
	{
		pulseOutput(x);
		delay(50);
	}
	
	delay(50);
	_belongsTo->stopPlaying();
}

void MIDI_SN74HC595N::pulseOutput(uint8_t out)
{
	_debug.debugln(20, F("Attempting to pulse output: %d"), out);
	
	if(out > _maxOutput)
	{
		_debug.debugln(15, F("Output %d is out of range; Max is %d"), out, _maxOutput);
		return;
	}
	
	//Calculate which register and bit this output correlates to
	uint8_t registerIndex = out/8;
	uint8_t bitIndex = out%8;
	_debug.debugln(15, F("Calculated register %d and output %d"), registerIndex, bitIndex);
	
	//Set the output if not already set
	if(_registers[registerIndex].getOutput(bitIndex) == true)
	{
		_debug.debugln(15, F("Output %d is already active"), out);
		return;
	}
	
	_registers[registerIndex].setOutput(bitIndex);
	_registersChanged = true;
	
	if(_belongsTo) 
		_belongsTo->noteAssigned();
}

void MIDI_SN74HC595N::stopOutput(uint8_t out)
{
	_debug.debugln(20, F("Attempting to stop output: %d"), out);
	
	if(out > _maxOutput)
	{
		_debug.debugln(15, F("Output %d is out of range; Max is %d"), out, _maxOutput);
		return;
	}
	
	//Calculate which register and bit this output correlates to
	uint8_t registerIndex = out/8;
	uint8_t bitIndex = out%8;
	_debug.debugln(15, F("Calculated register %d and output %d"), registerIndex, bitIndex);
	
	//Clear the output if not already cleared
	if(_registers[registerIndex].getOutput(bitIndex) == false)
	{
		_debug.debugln(15, F("Output %d is already stopped"), out);
		return;
	}
	
	_registers[registerIndex].clearOutput(bitIndex);
	_registersChanged = true;
}

void MIDI_SN74HC595N::stopOutputs()
{
	_debug.debugln(20, F("Attempting to stop all actives notes"));
	
	for(int bitIndex = 0; bitIndex < 8; bitIndex++)
		for(int registerIndex = 0; registerIndex < _numRegisters; registerIndex++)
			_registers[registerIndex].clearOutput(bitIndex);
	
	_registersChanged = true;
	updateSN74HC595N();
}

void MIDI_SN74HC595N::playNotes()
{
	_debug.debugln(50, F("playNotes begin"));
	
	updateSN74HC595N();
	updateDurations();
};

void MIDI_SN74HC595N::updateDurations()
{	
	_debug.debugln(50, F("updateDurations begin"));
	
	for(int x = 0; x < _numRegisters; x++)
	{
		bool registerUpdated = _registers[x].updateDurations(MIDI_Periods::getResolution(), _maxDuration);
		_registersChanged = _registersChanged || registerUpdated;
	}
};

void MIDI_SN74HC595N::updateSN74HC595N()
{	
	_debug.debugln(50, F("updateSN74HC595N begin"));
	
	if(!_registersChanged)
	{
		_debug.debugln(50, F("Registers have not changed"));		
		return;
	}
	
	_debug.debugln(20, F("Registers have changed"));		
	uint8_t newValues[_numRegisters];
	uint8_t newIndex = _writeDirection ? _numRegisters - 1 : 0;
	
	_debug.debug(3, F("New register values: "));
	for(int x = 0; x < _numRegisters; x++)
	{
		_debug.debug(3, F("%d, "), _registers[x].getOutputs());
		newValues[newIndex] = _registers[x].getOutputs();
		newIndex = _writeDirection ? newIndex - 1 : newIndex + 1;
	}
	_debug.debugln(3);
	
	SPI.transfer(&newValues, _numRegisters);
	latchRegisters();

	_registersChanged = false;
};

void MIDI_SN74HC595N::setController(MIDI_Device_Controller *controller)
{
	_belongsTo = controller;
}