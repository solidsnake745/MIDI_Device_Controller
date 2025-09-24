#include "IO_DigitalWrite.h"

std::map<uint8_t, IO_DigitalWrite::pinOut*> IO_DigitalWrite::_pinMap;
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
	
	_outputs = new pinOut*[_maxOutputs];
	for(int x = 0; x < _maxOutputs; x++)
		_outputs[x] = nullptr;
	
	//Setup IO
	//Nothing to do here without mappings
}

void IO_DigitalWrite::updateOutputs()
{
	_debug.debugln(30, F("updateOuts begin"));
	updateIO();	
}

#pragma GCC push_options
#pragma GCC optimize("Ofast")
void IO_DigitalWrite::updateIO()
{	
	_debug.debugln(30, F("updateIO begin"));
	
	if(!_outputsChanged)
	{
		_debug.debugln(30, F("Outputs have not changed"));		
		return;
	}
	
	_debug.debugln(30, F("Outputs have changed"));
	
	auto change = _changedOutputs.begin();
	while(change != _changedOutputs.end())
	{
		_debug.debugln(30, F("Changing pin: %d to %d"), change->pin, change->state);
		#if defined(CORE_TEENSY)
			digitalWriteFast(change->pin, change->state);
		#else
			digitalWrite(change->pin, change->state);
		#endif		
		change++;
	}
	
	_changedOutputs.clear();
	_outputsChanged = false;
}
#pragma GCC pop_options

bool IO_DigitalWrite::isValidMapping(uint8_t out)
{
	bool hasBeenAdded = _pinMap.count(out) > 0;
	if(!hasBeenAdded)
		_debug.debugln(20, F("Pin %d is not an added output"), out);
	
	return hasBeenAdded;
}

void IO_DigitalWrite::setInverted(uint8_t out, bool value)
{
	_debug.debugln(20, F("Attempting to set invert on output : %d"), out);
	
	auto find = _pinMap.find(out);
	if(find == _pinMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
		return;
	}
		
	//Set setting on the output
	pinOut* p = find->second;
	p->reg->setInverted(p->bitIndex, value);
	
	//Update IO as this changes the output's initial/current value
	_changedOutputs.push_back(changedOutput(p));
	_outputsChanged = true;
	updateIO();
}

void IO_DigitalWrite::setShouldStop(uint8_t out, bool value)
{
	_debug.debugln(20, F("Attempting to set shouldStop on output: %d"), out);
	
	auto find = _pinMap.find(out);
	if(find == _pinMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
		return;
	}
	
	pinOut* p = find->second;
	p->shouldBeStopped = value;
}

bool IO_DigitalWrite::getOutput(uint8_t out)
{
	_debug.debugln(30, F("Attempting to get output: %d"), out);
	
	auto find = _pinMap.find(out);
	if(find == _pinMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
		return false; //Have to return something
	}
	
	//Return that outputs value
	pinOut* p = find->second;
	return p->reg->getBit(p->bitIndex); //Ignores invert setting
}

void IO_DigitalWrite::setOutput(uint8_t out, bool value)
{
	_debug.debugln(30, F("Attempting to set output: %d"), out);
	
	auto find = _pinMap.find(out);
	if(find == _pinMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
		return;
	}
	
	//Set the output if not already set
	//getBit to get the actual value without invert setting
	pinOut* p = find->second;
	if(p->reg->getBit(p->bitIndex) == value)
	{
		_debug.debugln(15, F("Output %d is already %d"), out, value);
		return;
	}
	
	//Update value in data and queue updating the IO
	p->reg->setBitValue(p->bitIndex, value);
	_changedOutputs.push_back(changedOutput(p));
	_outputsChanged = true;
}

void IO_DigitalWrite::toggleOutput(uint8_t out)
{
	_debug.debugln(30, F("Attempting to toggle output: %d"), out);
	
	auto find = _pinMap.find(out);
	if(find == _pinMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
		return;
	}
	
	//Update value in data and queue updating the IO
	pinOut* p = find->second;
	p->reg->toggleBit(p->bitIndex);
	_changedOutputs.push_back(changedOutput(p));
	_outputsChanged = true;
}

void IO_DigitalWrite::testOutputs()
{
	auto out = _pinMap.begin();
	while(out != _pinMap.end())
	{
		setOutput(out->first, HIGH);
		updateIO();		
		delay(250);
		
		setOutput(out->first, LOW);
		updateIO();
		out++;
	}
}

void IO_DigitalWrite::stopOutputs()
{
	_debug.debugln(20, F("Attempting to stop outputs"));
	
	auto out = _pinMap.begin();
	while(out != _pinMap.end())
	{
		pinOut* p = out->second;
		_debug.debugln(10, F("Output %d shouldBeStopped set to %d"), p->pin, p->shouldBeStopped);
		if(p->shouldBeStopped)
		{			
			p->reg->clearBit(p->bitIndex);
			_changedOutputs.push_back(changedOutput(p));
		}
		out++;
	}
	
	_outputsChanged = true;
	updateIO();
}

void IO_DigitalWrite::resetOutputs()
{
	_debug.debugln(20, F("Attempting to reset outputs"));
	
	auto out = _pinMap.begin();
	while(out != _pinMap.end())
	{
		pinOut* p = out->second;
		p->reg->clearBit(p->bitIndex);
		_changedOutputs.push_back(changedOutput(p));
		out++;
	}
	
	_outputsChanged = true;
	updateIO();
}

void IO_DigitalWrite::addPin(uint8_t pin)
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
	
	for(int x = 0; x < _maxOutputs; x++)
	{
		if(_outputs[x] != nullptr)
			continue;
		
		_debug.debugln(15, F("Adding pin %d to output %d"), pin, x);
		uint8_t registerIndex = x / 8;
		uint8_t bitIndex = x % 8;
		ByteNoteRegister* r = &_registers[registerIndex];
		pinOut* p = new pinOut(pin, r, bitIndex);

		_outputs[x] = p;
		_pinMap[pin] = p;
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
		_usedOutputs++;
		return;
	}
}

void IO_DigitalWrite::deletePin(uint8_t pin)
{
	auto find = _pinMap.find(pin);
	if(find == _pinMap.end())
		_debug.debugln(20, F("Pin %d is not an added output"), pin);
	else
	{
		for(int x = 0; x < _maxOutputs; x++)
		{
			if(_outputs[x] == nullptr)
				continue;
			
			if(_outputs[x] == find->second)
			{
				_outputs[x] = nullptr;
				break;
			}
		}
		
		delete find->second;
		_pinMap[find->first] = nullptr;
		_pinMap.erase(find);
		_usedOutputs--;
	}
}