#include "MIDI_Shift_Register.h"

SerialDebug MIDI_Shift_Register::_debug(0);

MIDI_Shift_Register::MIDI_Shift_Register(uint8_t size, uint8_t startingNote, uint8_t latchPin) 
{
	//Save settings
	_numRegisters = size;
	_startingNote = startingNote;
	_endingNote = startingNote + (size * 8) - 1;
	_debug.debugln(5, "Calculated end note: %d", _endingNote);
	
	//Initialize collections
	_registers = new shiftRegister[_numRegisters];
	for(int i = 0; i < _numRegisters; i++)
		_registers[i] = shiftRegister();
	_durations = new registerDurations[_numRegisters];
	for(int i = 0; i < _numRegisters; i++)
		_durations[i].resetAllDurations();
	
	//Setup SPI
	setLatchPin(latchPin);
	pinMode(_latchPin, OUTPUT);
	digitalWrite(_latchPin, LOW);
	SPI.begin();
	
	//0 out all configured shift registers
	for(int i = 0; i < _numRegisters; i++)
		SPI.transfer(0);
	latchRegisters();
};

void MIDI_Shift_Register::playNote(uint8_t note)
{
	_debug.debugln(1, "Attempting to play note: %d", note);
	
	//Don't play notes outside of the expected range
	if(note < _startingNote)
	{
		_debug.debugln(3, "Not playing note %d; starting note is %d", note, _startingNote);
		return;
	}
	if(note > _endingNote) 
	{
		_debug.debugln(3, "Not playing note %d; ending note is %d", note, _endingNote);
		return;
	}	
	
	//Calculate which register and bit this note correlates to
	uint8_t shiftedNote = note - _startingNote;
	uint8_t registerIndex = shiftedNote/8;
	uint8_t bitIndex = shiftedNote%8;
	_debug.debugln(5, "Calculated register %d and output %d", registerIndex, bitIndex);	
	
	//Set the output if not already set
	if(_registers[registerIndex].bits.getBit(bitIndex) == true)
	{
		_debug.debugln(3, "Note %d is already playing", note);
		return;
	}
	
	_registers[registerIndex].bits.setBit(bitIndex);
	_numActiveOutputs++;
	_registersChanged = true;
};

void MIDI_Shift_Register::stopNote(uint8_t note)
{
	_debug.debugln(1, "Attempting to stop note: %d", note);
	
	//Don't stop notes outside of the expected range
	if(note < _startingNote)
	{
		_debug.debugln(3, "Not stopping note %d; starting note is %d", note, _startingNote);
		return;
	}
	if(note > _endingNote) 
	{
		_debug.debugln(3, "Not stopping note %d; ending note is %d", note, _endingNote);
		return;
	}	
	
	//Calculate which register and bit this note correlates to
	uint8_t shiftedNote = note - _startingNote;
	uint8_t registerIndex = shiftedNote/8;
	uint8_t bitIndex = shiftedNote%8;
	_debug.debugln(5, "Calculated register %d and output %d", registerIndex, bitIndex);
	
	//Clear the output if not already cleared
	if(_registers[registerIndex].bits.getBit(bitIndex) == false)
	{
		_debug.debugln(3, "Note %d is already stopped", note);
		return;
	}
	
	_registers[registerIndex].bits.clearBit(bitIndex);
	_durations[registerIndex].resetDuration(bitIndex);
	_numActiveOutputs--;
	_registersChanged = true;
};

uint8_t MIDI_Shift_Register::getRegisterValue(uint8_t index)
{
	if(index >= _numRegisters)
	{
		_debug.debugln(3, "Register %d doesn't exist; %d register(s) configured", index, _numRegisters);
		return 0;
	}
	return _registers[index].value;
};

void MIDI_Shift_Register::playNotes()
{
	_debug.debugln(20, "playNotes begin");
	
	if(_numActiveOutputs == 0)
	{
		_debug.debugln(10, "No active outputs");
		return;
	}
	
	updateDurations();	
	updateRegisters();
};

void MIDI_Shift_Register::updateDurations()
{	
	_debug.debugln(20, "updateDurations begin");
	
	for(int x = 0; x < _numRegisters; x++)
	{		
		for(int y = 0; y < 8; y++)
		{
			//Increment output duration if active
			if(_registers[x].bits.getBit(y))
				_durations[x].incrementDuration(y, MIDI_Periods::getResolution());			

			//Reset output if past the limit
			uint32_t temp = _durations[x].getDuration(y);
			if(temp > _durationLimit)
			{
				_debug.debugln(10, "Resetting register %d output %d duration (%d)", x, y, temp);
				_durations[x].resetDuration(y);
				_registers[x].bits.clearBit(y);
				_numActiveOutputs--;
				_registersChanged = true;
			}
		}
	}
};

void MIDI_Shift_Register::updateRegisters()
{	
	_debug.debugln(20, "updateRegisters begin");
	
	if(!_registersChanged)
	{
		_debug.debugln(10, "Registers have not changed");		
		return;
	}
		
	_debug.debugln(10, "Registers have changed; %d active outputs", _numActiveOutputs);		
	for(int i = 0; i < _numRegisters; i++)
	{
		_debug.debugln(10, "Register %d value: %d", i, _registers[i].value);
		SPI.transfer(_registers[i].value);
	}
	latchRegisters();
	
	_registersChanged = false;
};