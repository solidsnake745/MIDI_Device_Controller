#include "IO_DigitalWrite.h"

std::map<uint8_t, uint8_t> IO_DigitalWrite::_outputMap;
std::vector<IO_DigitalWrite::changedOutput> IO_DigitalWrite::_changedOutputs;
 
IO_DigitalWrite::IO_DigitalWrite(uint8_t numOutputs) 
{
	if(numOutputs == 0)
		numOutputs = 1;
	
	//Save settings
	_maxOutputs = numOutputs;
	_numRegisters = (numOutputs/8) + 1;
	
	//Initialize collections
	_registers = new ByteNoteRegister[_numRegisters];
	for(int x = 0; x < _numRegisters; x++)
		_registers[x] = ByteNoteRegister();
	
	//Setup IO
	//Nothing to do here without mappings
};

void IO_DigitalWrite::checkMaxDuration()
{
	auto out = _outputMap.begin();
	while(out != _outputMap.end())
	{
		uint8_t registerIndex = out->second/8;
		uint8_t bitIndex = out->second%8;
		if(_registers[registerIndex].isPastMaxDuration(bitIndex))
		{
			_registers[registerIndex].clearBit(bitIndex);
			_changedOutputs.push_back(changedOutput(out->first, false));
		}
		out++;
	}
}

void IO_DigitalWrite::updateOutputs()
{
	_debug.debugln(30, F("updateOuts begin"));
	updateDurations();
	updateIO();	
}

#pragma GCC push_options
#pragma GCC optimize("Ofast")
void IO_DigitalWrite::updateDurations()
{	
	_debug.debugln(30, F("updateDurations begin"));
	
	for(int x = 0; x < _numRegisters; x++)
		_registers[x].updateDurations(MIDI_Periods::getResolution());
}

void IO_DigitalWrite::updateIO()
{	
	_debug.debugln(30, F("updateIO begin"));
	
	if(!_outputsChanged)
	{
		_debug.debugln(20, F("Registers have not changed"));		
		return;
	}
	
	_debug.debugln(20, F("Registers have changed"));
	
	auto change = _changedOutputs.begin();
	while(change != _changedOutputs.end())
	{
		_debug.debugln(5, F("Changing pin: %d to %d"), change->pin, change->state);
		digitalWrite(change->pin, change->state);
		change++;
	}
	
	_changedOutputs.clear();
	_outputsChanged = false;
}
#pragma GCC pop_options

bool IO_DigitalWrite::isValidMapping(uint8_t out)
{
	bool hasBeenAdded = _outputMap.count(out) > 0;
	if(!hasBeenAdded)
		_debug.debugln(20, F("Pin %d is not an added output"), out);
	
	return hasBeenAdded;
}

void IO_DigitalWrite::setMaxDuration(uint8_t out, uint32_t us)
{
	auto find = _outputMap.find(out);
	if(find == _outputMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
		return;
	}
	
	uint8_t registerIndex = find->second/8;
	uint8_t bitIndex = find->second%8;
	_debug.debugln(15, F("Calculated register %d and output %d"), registerIndex, bitIndex);
	
	_registers[registerIndex].setMaxDuration(bitIndex, us);
}

void IO_DigitalWrite::setOutputInverted(uint8_t out, bool value)
{
	_debug.debugln(20, F("Attempting to set output invert: %d"), out);
	
	auto find = _outputMap.find(out);
	if(find == _outputMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
		return;
	}
	
	//Calculate which register and bit this output correlates to
	uint8_t registerIndex = find->second/8;
	uint8_t bitIndex = find->second%8;
	_debug.debugln(15, F("Calculated register %d and output %d"), registerIndex, bitIndex);
	
	//Set setting on the calculated output
	_registers[registerIndex].setInverted(bitIndex, value);
}

void IO_DigitalWrite::setOutput(uint8_t out, bool value)
{
	_debug.debugln(20, F("Attempting to set output: %d"), out);
	
	auto find = _outputMap.find(out);
	if(find == _outputMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
		return;
	}
	
	//Calculate which register and bit this output correlates to
	uint8_t registerIndex = find->second/8;
	uint8_t bitIndex = find->second%8;
	_debug.debugln(15, F("Calculated register %d and output %d"), registerIndex, bitIndex);
	
	//Set the output if not already set	
	if(_registers[registerIndex].getBit(bitIndex) == value)
	{
		_debug.debugln(15, F("Output %d is already %d"), value);
		return;
	}
	
	_registers[registerIndex].setBitValue(bitIndex, value);
	_changedOutputs.push_back(changedOutput(find->first, value));
	_outputsChanged = true;
}

void IO_DigitalWrite::stopOutputs()
{
	_debug.debugln(20, F("Attempting to stop all actives notes"));
	
	auto out = _outputMap.begin();
	while(out != _outputMap.end())
	{
		uint8_t registerIndex = out->second/8;
		uint8_t bitIndex = out->second%8;
		_registers[registerIndex].clearBit(bitIndex);
		out++;
	}
	
	_outputsChanged = true;
	updateIO();
}

void IO_DigitalWrite::addOutput(uint8_t pin)
{
	//Check there are outputs available
	if(_usedOutputs == _maxOutputs)
	{
		_debug.debugln(15, F("No available outputs to add to; Max is %d"), _maxOutputs);
		return;
	}
	
	//Check pin is not already mapped
	if(isValidMapping(pin))
	{
		_debug.debugln(15, F("Pin %d is already added"), pin);
		return;
	}
	
	//Check outputs in order to find the first unused one
	for(int out = 0; out < _maxOutputs; out++)
	{
		bool used = false;
		auto search = _outputMap.begin();
		while(search != _outputMap.end() && !used)
		{
			used = search->second == out || used;
			search++;
		}
		
		if(!used)
		{
			_debug.debugln(15, F("Adding pin %d to output %d"), pin, out);
			_outputMap[pin] = out;
			pinMode(pin, OUTPUT);
			digitalWrite(pin, LOW);
			_usedOutputs++;
			break;
		}
	}
}

void IO_DigitalWrite::deleteOutput(uint8_t pin)
{
	auto find = _outputMap.find(pin);
	if(find == _outputMap.end())
		_debug.debugln(20, F("Pin %d is not an added output"), pin);
	else
	{
		_outputMap.erase(find);
		_usedOutputs--;
	}
}

void IO_DigitalWrite::testOutputs()
{
	auto out = _outputMap.begin();
	while(out != _outputMap.end())
	{
		digitalWrite(out->first, HIGH);
		delay(250);
		digitalWrite(out->first, LOW);
		delay(250);
		out++;
	}
}