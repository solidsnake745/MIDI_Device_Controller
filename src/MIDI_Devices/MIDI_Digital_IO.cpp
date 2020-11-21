#include "MIDI_Digital_IO.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

SerialDebug MIDI_Digital_IO::_debug(DEBUG_DigitalIO);

MIDI_Digital_IO::MIDI_Digital_IO(uint8_t numOutputs) 
{
	if(numOutputs == 0)
		numOutputs = 1;
	
	//Save settings
	_maxOutputs = numOutputs;
	_numRegisters = (numOutputs/8) + 1;
	
	//Initialize collections
	_registers = new byteNoteRegister[_numRegisters];
	for(int x = 0; x < _numRegisters; x++)
		_registers[x] = byteNoteRegister();
	
	//Setup IO
	//Nothing to do here without mappings
};

void MIDI_Digital_IO::testOutputsDirect()
{
	auto out = _outputMap.begin();
	while(out != _outputMap.end())
	{
		digitalWrite(out->first, HIGH);
		delay(50);
		digitalWrite(out->first, LOW);
		out++;
	}
}

void MIDI_Digital_IO::testOutputsInterrupt()
{
	_belongsTo->startPlaying();
	
	auto out = _outputMap.begin();
	while(out != _outputMap.end())
	{
		pulseOutput(out->first);
		delay(50);		
		out++;
	}
	
	delay(50);
	_belongsTo->stopPlaying();
}

void MIDI_Digital_IO::addOutput(uint8_t pin)
{
	//Check there are outputs available
	if(_usedOutputs == _maxOutputs)
	{
		_debug.debugln(15, F("No available outputs to add to; Max is %d"), _maxOutputs);
		return;
	}
	
	//Check pin is not already mapped
	if(_outputMap.count(pin) > 0)
	{
		_debug.debugln(15, F("Pin %d is already added"), pin);
		return;
	}
	
	//Check outputs in order to find the first unused one
	for(int out = 0; out < _maxOutputs; out++)
	{
		bool used = false;
		auto search = _outputMap.begin();
		while(search != _outputMap.end())
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

void MIDI_Digital_IO::deleteOutput(uint8_t pin)
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

bool MIDI_Digital_IO::isValidMapping(uint8_t out)
{
	bool hasBeenAdded = _outputMap.count(out) > 0;
	if(!hasBeenAdded)
		_debug.debugln(20, F("Pin %d is not an added output"), out);
	
	return hasBeenAdded;
}

void MIDI_Digital_IO::pulseOutput(uint8_t out)
{
	_debug.debugln(20, F("Attempting to pulse output: %d"), out);
	
	auto find = _outputMap.find(out);
	if(find == _outputMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
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
	_outputsChanged = true;
	
	if(_belongsTo) 
		_belongsTo->noteAssigned();
}

void MIDI_Digital_IO::stopOutput(uint8_t out)
{
	_debug.debugln(20, F("Attempting to stop output: %d"), out);
	
	auto find = _outputMap.find(out);
	if(find == _outputMap.end())
	{
		_debug.debugln(20, F("Pin %d is not an added output"), out);
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
	_outputsChanged = true;
}

void MIDI_Digital_IO::stopOutputs()
{
	_debug.debugln(20, F("Attempting to stop all actives notes"));
	
	auto out = _outputMap.begin();
	while(out != _outputMap.end())
	{
		uint8_t registerIndex = out->second/8;
		uint8_t bitIndex = out->second%8;
		_registers[registerIndex].clearOutput(bitIndex);
		out++;
	}
	
	_outputsChanged = true;
	updateIO();
}

void MIDI_Digital_IO::playNotes()
{
	_debug.debugln(50, F("playNotes begin"));
	
	updateIO();
	updateDurations();
};

void MIDI_Digital_IO::updateDurations()
{	
	_debug.debugln(50, F("updateDurations begin"));
	
	for(int x = 0; x < _numRegisters; x++)
	{
		bool registerUpdated = _registers[x].updateDurations(MIDI_Periods::getResolution(), _maxDuration);
		_outputsChanged = _outputsChanged || registerUpdated;
	}
};

void MIDI_Digital_IO::updateIO()
{	
	_debug.debugln(50, F("updateIO begin"));
	
	if(!_outputsChanged)
	{
		_debug.debugln(50, F("Registers have not changed"));		
		return;
	}
	
	_debug.debugln(20, F("Registers have changed"));		
	
	auto out = _outputMap.begin();
	while(out != _outputMap.end())
	{
		uint8_t registerIndex = out->second/8;
		uint8_t bitIndex = out->second%8;
		
		digitalWrite(out->first, _registers[registerIndex].getOutput(bitIndex));
		out++;
	}
	
	_outputsChanged = false;
};

void MIDI_Digital_IO::setController(MIDI_Device_Controller *controller)
{
	_belongsTo = controller;
}