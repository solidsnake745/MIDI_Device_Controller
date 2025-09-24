#include "IO_74HC595.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

IO_74HC595::IO_74HC595(uint8_t numRegisters, uint8_t latchPin) 
{
	if(numRegisters == 0)
		numRegisters = 1;
	
	//Save settings
	_numRegisters = numRegisters; //Number of 8 bit shift registers
	_maxOutputs = (numRegisters * 8);
	
	//Initialize collections
	_registers = new ByteNoteRegister[_numRegisters];
	for(int x = 0; x < _numRegisters; x++)
		_registers[x] = ByteNoteRegister();
	
	_outputs = new regOut*[_maxOutputs];
	for(int x = 0; x < _maxOutputs; x++)
	{
		uint8_t registerIndex = x / 8;
		uint8_t bitIndex = x % 8;
		ByteNoteRegister* r = &_registers[registerIndex];
		regOut* ro = new regOut(r, bitIndex);
		_outputs[x] = ro;
	}
	
	//Setup SPI
	setLatchPin(latchPin);
	SPI.begin();
	
	//0 out all configured shift registers
	for(int x = 0; x < _numRegisters; x++)
		SPI.transfer(0);
	latchRegisters();
}

void IO_74HC595::updateOutputs()
{
	_debug.debugln(50, F("updateOuts begin"));
	update74HC595();
}

void IO_74HC595::update74HC595()
{	
	_debug.debugln(50, F("update74HC595 begin"));
	
	if(!_registersChanged)
	{
		_debug.debugln(50, F("Registers have not changed"));		
		return;
	}
	
	_debug.debugln(20, F("Registers have changed"));		
	uint8_t newValues[_numRegisters];
	uint8_t newIndex = !_reverseOutput ? _numRegisters - 1 : 0;
	
	_debug.debug(20, F("New register values: "));
	for(int x = 0; x < _numRegisters; x++)
	{
		newValues[newIndex] = !_reverseOutput ? _registers[x].getByteValue() : reverseByte(_registers[x].getByteValue());
		_debug.debug(20, F("%d (%d), "), _registers[x].getByteValue(), newValues[newIndex]);
		newIndex = !_reverseOutput ? newIndex - 1 : newIndex + 1;
	}
	_debug.debugln(20);
	
	SPI.transfer(&newValues, _numRegisters);
	latchRegisters();

	_registersChanged = false;
}

uint8_t IO_74HC595::reverseByte(uint8_t n)
{
	// Taken from https://stackoverflow.com/a/2603254
	// Reverse the top and bottom nibble then swap them
	return (reverseLookup[n&0b1111] << 4) | reverseLookup[n>>4];
}

bool IO_74HC595::isValidMapping(uint8_t out)
{
	bool isWithinRange = out < _maxOutputs;
	if(!isWithinRange)
		_debug.debugln(20, F("Output %d is not valid; Max is %d"), out, (_maxOutputs - 1));
	
	return isWithinRange;
}

void IO_74HC595::setInverted(uint8_t out, bool value)
{
	_debug.debugln(20, F("Attempting to set output: %d"), out);
	
	if(!isValidMapping(out))
		return;
	
	//Set setting on the output
	regOut* r = _outputs[out];
	r->reg->setInverted(r->bitIndex, value);
	
	//Update registers as this changes the output's initial/current value
	_registersChanged = true;
	update74HC595();
}

void IO_74HC595::setShouldStop(uint8_t out, bool value)
{
	_debug.debugln(20, F("Attempting to set shouldStop on output: %d"), out);
	
	if(!isValidMapping(out))
		return;
	
	//Set setting on the output
	regOut* r = _outputs[out];
	r->shouldBeStopped = value;
}

bool IO_74HC595::getOutput(uint8_t out)
{
	_debug.debugln(20, F("Attempting to get output: %d"), out);
	
	if(!isValidMapping(out))
		return false; //Have to return something
		
	//Return that outputs value
	regOut* r = _outputs[out];
	return r->reg->getBit(r->bitIndex);
}

void IO_74HC595::setOutput(uint8_t out, bool value)
{
	_debug.debugln(20, F("Attempting to set output: %d"), out);
	
	if(!isValidMapping(out))
		return;
		
	//Set the output if not already set
	regOut* r = _outputs[out];
	if(r->reg->getBit(r->bitIndex) == value)
	{
		_debug.debugln(15, F("Output %d is already %d"), value);
		return;
	}
	
	r->reg->setBitValue(r->bitIndex, value);
	_registersChanged = true;
}

void IO_74HC595::toggleOutput(uint8_t out)
{
	_debug.debugln(20, F("Attempting to toggle output: %d"), out);
	
	if(!isValidMapping(out))
		return;
		
	//Set the output if not already set
	regOut* r = _outputs[out];
	r->reg->toggleBit(r->bitIndex);
	_registersChanged = true;
}

void IO_74HC595::testOutputs()
{		
	_debug.println(F("Testing each register's individual outputs"));
	
	//Enable each output on each register
	for(int x = 0; x < _maxOutputs; x++)
	{
		regOut* r = _outputs[x];
		
		r->reg->setBitValue(r->bitIndex, HIGH);
		_registersChanged = true;
		update74HC595();
		delay(250);
		
		r->reg->setBitValue(r->bitIndex, LOW);
		_registersChanged = true;
		update74HC595();
	}
}

void IO_74HC595::stopOutputs()
{
	_debug.debugln(20, F("Attempting to stop outputs"));
	
	for(int x = 0; x < _maxOutputs; x++)
	{
		regOut* r = _outputs[x];
		if(r->shouldBeStopped)
			r->reg->clearBit(r->bitIndex);
	}
	
	_registersChanged = true;
	update74HC595();
}

void IO_74HC595::resetOutputs()
{
	_debug.debugln(20, F("Attempting to reset outputs"));
	
	for(int x = 0; x < _maxOutputs; x++)
	{
		regOut* r = _outputs[x];
		r->reg->clearBit(r->bitIndex);
	}
	
	_registersChanged = true;
	update74HC595();
}