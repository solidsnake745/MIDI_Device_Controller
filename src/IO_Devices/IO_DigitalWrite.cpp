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
	for(uint8_t x = 0; x < _numRegisters; x++)
		_registers[x] = ByteNoteRegister();
	
	_outputs = new pinOut*[_maxOutputs];
	for(uint8_t x = 0; x < _maxOutputs; x++)
		_outputs[x] = nullptr;
	
	//Setup IO
	//Nothing to do here without mappings
}

//Not really sure if this makes a difference
//TODO: research and confirm
#pragma GCC push_options
#pragma GCC optimize("Ofast")
void IO_DigitalWrite::updateOutputs()
{
	_debug.debugln(ISR_TRACE, F("IO_DigitalWrite::updateOutputs() begin"));
	
	if(!_outputsChanged)
	{
		_debug.debugln(ISR_TRACE, F("Outputs have not changed"));		
		return;
	}
	
	_debug.debugln(ISR, F("Outputs have changed"));
	
	auto change = _changedOutputs.begin();
	while(change != _changedOutputs.end())
	{
		_debug.debugln(ISR, F("Changing pin: %d to %d"), change->pin, change->state);
		#if defined(CORE_TEENSY)
			//TODO: Looking back at this, this probably has no advantage since we're not using it on a constant value; confirm and remove
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

void IO_DigitalWrite::addPin(uint8_t pin)
{
	//Check there are outputs available
	if(_usedOutputs == _maxOutputs)
	{
		_debug.println(F("No available outputs to add to; Max is %d"), _maxOutputs);
		return;
	}
	
	//Check pin is not already mapped
	if(isValidMapping(pin))
	{
		_debug.println(F("DigitalIO pin %d has already been added"), pin);
		return;
	}
	
	for(uint8_t x = 0; x < _maxOutputs; x++)
	{
		if(_outputs[x] != nullptr)
			continue;
		
		uint8_t registerIndex = x / 8;
		uint8_t bitIndex = x % 8;
		ByteNoteRegister* r = &_registers[registerIndex];
		pinOut* p = new pinOut(pin, r, bitIndex);

		_outputs[x] = p;
		_pinMap[pin] = p;
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
		_usedOutputs++;
		_debug.println(F("Added pin %d to DigitalIO outputs; %d additional pin(s) can be added"), pin, (_maxOutputs - _usedOutputs));
		return;
	}
}

void IO_DigitalWrite::deletePin(uint8_t pin)
{
	auto find = _pinMap.find(pin);
	if(find == _pinMap.end())
		_debug.println(F("DigitalIO pin %d has not been added"), pin);
	else
	{
		for(uint8_t x = 0; x < _maxOutputs; x++)
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

bool IO_DigitalWrite::isValidMapping(uint8_t out) { return (findOutput(out) != nullptr); }

IO_DigitalWrite::pinOut* IO_DigitalWrite::findOutput(uint8_t out)
{
	auto find = _pinMap.find(out);
	if(find == _pinMap.end())
	{
		_debug.println(F("DigitalIO pin %d has not been added"), out);
		return nullptr;
	}
	
	return find->second;
}

void IO_DigitalWrite::setInverted(uint8_t out, bool value)
{
	_debug.debugln(TRACE, F("Attempting to set output %d's inverted setting"), out);
	
	pinOut* p = findOutput(out);
	if(!p) return;

	p->reg->setInverted(p->bitIndex, value);
	_debug.println(F("DigitalIO pin %d inverted set to: %s"), out, toString(value));
	
	//Update IO as this changes the output's initial/current value
	_changedOutputs.push_back(changedOutput(p));
	_outputsChanged = true;
	updateOutputs();
}

void IO_DigitalWrite::setShouldStop(uint8_t out, bool value)
{
	_debug.debugln(TRACE, F("Attempting to set output %d's shouldBeStopped setting"), out);
	
	pinOut* p = findOutput(out);
	if(!p) return;
	
	p->shouldBeStopped = value;
	_debug.println(F("DigitalIO pin %d shouldBeStopped set to: %s"), out, toString(value));
}

bool IO_DigitalWrite::getOutput(uint8_t out)
{
	_debug.debugln(TRACE, F("Attempting to get output %d's state"), out);
	
	pinOut* p = findOutput(out);
	if(!p) return false; //Have to return something
		
	return p->reg->getBit(p->bitIndex); //Ignores invert setting
}

void IO_DigitalWrite::setOutput(uint8_t out, bool state)
{
	_debug.debugln(TRACE, F("Attempting to set output %d's state"), out);
	
	pinOut* p = findOutput(out);
	if(!p) return;
	
	//Set the output if not already set
	//getBit to get the actual value without invert setting
	if(p->reg->getBit(p->bitIndex) == state)
	{
		_debug.debugln(DEBUG, F("DigitalIO pin %d is already %d"), out, state);
		return;
	}
	
	//Update value in data and queue updating the IO
	p->reg->setBitValue(p->bitIndex, state);
	_changedOutputs.push_back(changedOutput(p));
	_outputsChanged = true;
}

void IO_DigitalWrite::toggleOutput(uint8_t out)
{
	_debug.debugln(TRACE, F("Attempting to toggle output %d"), out);
	
	pinOut* p = findOutput(out);
	if(!p) return;
	
	//Update value in data and queue updating the IO
	p->reg->toggleBit(p->bitIndex);
	_changedOutputs.push_back(changedOutput(p));
	_outputsChanged = true;
}

void IO_DigitalWrite::testOutputs()
{
	_debug.println(F("Testing each added DigitalIO pin"));
	
	auto out = _pinMap.begin();
	while(out != _pinMap.end())
	{
		setOutput(out->first, HIGH);
		updateOutputs();		
		delay(250);
		
		setOutput(out->first, LOW);
		updateOutputs();
		out++;
	}
}

void IO_DigitalWrite::stopOutputs()
{
	_debug.debugln(TRACE, F("Attempting to stop outputs"));
	
	auto out = _pinMap.begin();
	while(out != _pinMap.end())
	{
		pinOut* p = out->second;
		_debug.debugln(DEBUG, F("DigitalIO pin %d shouldBeStopped set to %s"), p->pin, toString(p->shouldBeStopped));
		if(p->shouldBeStopped)
		{			
			p->reg->clearBit(p->bitIndex);
			_changedOutputs.push_back(changedOutput(p));
		}
		out++;
	}
	
	_outputsChanged = true;
	updateOutputs();
	_debug.println(F("DigitalIO outputs stopped"));
}

void IO_DigitalWrite::resetOutputs()
{
	_debug.debugln(TRACE, F("Attempting to reset outputs"));
	
	auto out = _pinMap.begin();
	while(out != _pinMap.end())
	{
		pinOut* p = out->second;
		p->reg->clearBit(p->bitIndex);
		_changedOutputs.push_back(changedOutput(p));
		out++;
	}
	
	_outputsChanged = true;
	updateOutputs();
	_debug.println(F("DigitalIO outputs reset"));
}