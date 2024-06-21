#include "IO_SN74HC595N.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

constexpr uint8_t IO_SN74HC595N::reverseLookup[];

IO_SN74HC595N::IO_SN74HC595N(uint8_t numRegisters, uint8_t latchPin) 
{
	if(numRegisters == 0)
		numRegisters = 1;
	
	//Save settings
	_numRegisters = numRegisters; //Number of 8 bit shift registers
	_maxOutput = (numRegisters * 8) - 1;
	
	//Initialize collections
	_registers = new ByteNoteRegister[_numRegisters];
	for(int x = 0; x < _numRegisters; x++)
		_registers[x] = ByteNoteRegister();
	
	// for(int x = 0; x < 8; x++) _registers[0].setInverted(x, true);
		
	//Setup SPI
	setLatchPin(latchPin);
	SPI.begin();
	
	//0 out all configured shift registers
	for(int x = 0; x < _numRegisters; x++)
		SPI.transfer(0);
	latchRegisters();
}

void IO_SN74HC595N::checkMaxDuration()
{
	for(int registerIndex = 0; registerIndex < _numRegisters; registerIndex++)
		for(int bitIndex = 0; bitIndex < 8; bitIndex++)
		{
			if(_registers[registerIndex].isPastMaxDuration(bitIndex))
			{
				_registers[registerIndex].clearBit(bitIndex);
				_registersChanged = true;
			}			
		}	
}

void IO_SN74HC595N::updateOutputs()
{
	_debug.debugln(50, F("updateOuts begin"));
	
	updateSN74HC595N();
	updateDurations();
}

void IO_SN74HC595N::updateDurations()
{	
	_debug.debugln(50, F("updateDurations begin"));
	
	for(int x = 0; x < _numRegisters; x++)
		_registers[x].updateDurations(MIDI_Periods::getResolution());
}

void IO_SN74HC595N::updateSN74HC595N()
{	
	_debug.debugln(50, F("updateSN74HC595N begin"));
	
	if(!_registersChanged)
	{
		_debug.debugln(50, F("Registers have not changed"));		
		return;
	}
	
	_debug.debugln(20, F("Registers have changed"));		
	uint8_t newValues[_numRegisters];
	uint8_t newIndex = !_reverseOutput ? _numRegisters - 1 : 0;
	
	_debug.debug(3, F("New register values: "));
	for(int x = 0; x < _numRegisters; x++)
	{
		newValues[newIndex] = !_reverseOutput ? _registers[x].getByteValue() : reverseByte(_registers[x].getByteValue());
		_debug.debug(3, F("%d (%d), "), _registers[x].getByteValue(), newValues[newIndex]);
		newIndex = !_reverseOutput ? newIndex - 1 : newIndex + 1;
	}
	_debug.debugln(3);
	
	SPI.transfer(&newValues, _numRegisters);
	latchRegisters();

	_registersChanged = false;
}

uint8_t IO_SN74HC595N::reverseByte(uint8_t n)
{
	// Taken from https://stackoverflow.com/a/2603254
	// Reverse the top and bottom nibble then swap them
	return (reverseLookup[n&0b1111] << 4) | reverseLookup[n>>4];
}

bool IO_SN74HC595N::isValidMapping(uint8_t out)
{
	bool isWithinRange = out <= _maxOutput;
	if(!isWithinRange)
		_debug.debugln(20, F("Output %d is not valid; Max is %d"), out, _maxOutput);
	
	return isWithinRange;
}

void IO_SN74HC595N::setMaxDuration(uint8_t out, uint32_t us)
{
	if(out > _maxOutput)
	{
		_debug.debugln(15, F("Output %d is out of range; Max is %d"), out, _maxOutput);
		return;
	}
	
	uint8_t registerIndex = out/8;
	uint8_t bitIndex = out%8;
	_debug.debugln(15, F("Calculated register %d and output %d"), registerIndex, bitIndex);
	
	_registers[registerIndex].setMaxDuration(bitIndex, us);
}

void IO_SN74HC595N::setOutputInverted(uint8_t out, bool value)
{
	
}

void IO_SN74HC595N::setOutput(uint8_t out, bool value)
{
	_debug.debugln(20, F("Attempting to set output: %d"), out);
	
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
	if(_registers[registerIndex].getBit(bitIndex) == value)
	{
		_debug.debugln(15, F("Output %d is already %d"), value);
		return;
	}
	
	_registers[registerIndex].setBitValue(bitIndex, value);
	_registersChanged = true;
}

void IO_SN74HC595N::stopOutputs()
{
	_debug.debugln(20, F("Attempting to stop all actives notes"));
	
	for(int registerIndex = 0; registerIndex < _numRegisters; registerIndex++)
		for(int bitIndex = 0; bitIndex < 8; bitIndex++)		
			_registers[registerIndex].clearBit(bitIndex);
	
	_registersChanged = true;
	updateSN74HC595N();
}

void IO_SN74HC595N::testOutputs()
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
		
		delay(250);
	}
	
	//Clear registers
	for(int y = 0; y < _numRegisters; y++)
	{
		SPI.transfer(0);
		latchRegisters();
	}
}