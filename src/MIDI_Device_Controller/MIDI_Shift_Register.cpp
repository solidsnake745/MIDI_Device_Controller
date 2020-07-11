#include "MIDI_Shift_Register.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

SerialDebug MIDI_Shift_Register::_debug(15);

MIDI_Shift_Register::MIDI_Shift_Register(uint8_t size, uint8_t startingNote, uint8_t latchPin) 
{
	//Save settings
	_numRegisters = size; //Number of 8 bit shift registers 
	_startingNote = startingNote;
	
	//ex. _startingNote = 50 and _size = 1 then 50 + ((1 * 8) - 1) = 57
	_endingNote = _startingNote + (size * 8) - 1;
	
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
	SPI.begin();
	
	//0 out all configured shift registers
	for(int i = 0; i < _numRegisters; i++)
		SPI.transfer(0);
	latchRegisters();
};

void MIDI_Shift_Register::testRegistersDirect()
{
	int testDelay = 50;
	
	//Enable each output on each register gradually
	_debug.println("Testing each register's individual outputs");
	
	for(int x = 0; x < 8; x++)
	{
		int z = 1 << x;
		_debug.println("Testing output %d", x);
		
		for(int y = 0; y < _numRegisters; y++)
		{
			SPI.transfer(z);
			latchRegisters();
			delay(testDelay);
		}
		delay(testDelay * 2);
	}
	
	//Clear registers
	for(int y = 0; y < _numRegisters; y++)
	{
		SPI.transfer(0);
		latchRegisters();
	}
}

void MIDI_Shift_Register::playNote(uint8_t note)
{
	_debug.debugln(20, "Attempting to play note: %d", note);
	
	//Don't play notes outside of the expected range
	if(!validateNote(note))
		return;
	
	//Calculate which register and bit this note correlates to
	uint8_t shiftedNote = note - _startingNote;
	uint8_t registerIndex = shiftedNote/8;
	uint8_t bitIndex = shiftedNote%8;
	_debug.debugln(5, "Calculated register %d and output %d", registerIndex, bitIndex);	
	
	//Set the output if not already set
	if(_registers[registerIndex].bits.getBit(bitIndex) == true)
	{
		_debug.debugln(15, "Note %d is already playing", note);
		return;
	}
	
	_registers[registerIndex].bits.setBit(bitIndex);
	_numActiveOutputs++;
	_registersChanged = true;
	
	if(_belongsTo) 
		_belongsTo->noteAssigned();
};

void MIDI_Shift_Register::stopNote(uint8_t note)
{
	_debug.debugln(20, "Attempting to stop note: %d", note);
	
	//Don't stop notes outside of the expected range
	if(!validateNote(note))
		return;
	
	//Calculate which register and bit this note correlates to
	uint8_t shiftedNote = note - _startingNote;
	uint8_t registerIndex = shiftedNote/8;
	uint8_t bitIndex = shiftedNote%8;
	_debug.debugln(15, "Calculated register %d and output %d", registerIndex, bitIndex);
	
	//Clear the output if not already cleared
	if(_registers[registerIndex].bits.getBit(bitIndex) == false)
	{
		_debug.debugln(15, "Note %d is already stopped", note);
		return;
	}
	
	_registers[registerIndex].bits.clearBit(bitIndex);
	_durations[registerIndex].resetDuration(bitIndex);
	_numActiveOutputs--;
	_registersChanged = true;
};

void MIDI_Shift_Register::stopNotes()
{
	_debug.debugln(20, "Attempting to stop all actives notes");
	
	if(_numActiveOutputs == 0)
	{
		_debug.debugln(15, "No active notes to stop");
		return;
	}
	
	for(int bitIndex = 0; bitIndex < 8; bitIndex++)
		for(int registerIndex = 0; registerIndex < _numRegisters; registerIndex++)
		{
			_registers[registerIndex].bits.clearBit(bitIndex);
			_durations[registerIndex].resetDuration(bitIndex);
		}
	
	_numActiveOutputs = 0;
	_registersChanged = true;
	updateRegisters();
}

bool MIDI_Shift_Register::validateNote(uint8_t note)
{
	if(note < _startingNote)
	{
		_debug.debugln(15, "Note %d is out of range; starting note is %d", note, _startingNote);
		return false;
	}
	if(note > _endingNote) 
	{
		_debug.debugln(15, "Note %d is out of range; ending note is %d", note, _endingNote);
		return false;
	}
	
	return true;
};

void MIDI_Shift_Register::playNotes()
{
	_debug.debugln(50, "playNotes begin");
	
	updateRegisters();
	
	if(_numActiveOutputs > 0)
		updateDurations();
};

void MIDI_Shift_Register::updateDurations()
{	
	_debug.debugln(50, "updateDurations begin");
	
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
				_debug.debugln(20, "Resetting register %d output %d duration (%d)", x, y, temp);
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
	_debug.debugln(50, "updateRegisters begin");
	
	if(!_registersChanged)
	{
		_debug.debugln(50, "Registers have not changed");		
		return;
	}
	
	_debug.debugln(20, "Registers have changed; %d active outputs", _numActiveOutputs);		
	for(int i = _numRegisters - 1; i >= 0; i--)
	{
		_debug.debugln(3, "Register %d value: %d", i, _registers[i].value);
		SPI.transfer(_registers[i].value);
	}
	latchRegisters();

	_registersChanged = false;
};

void MIDI_Shift_Register::setController(MIDI_Device_Controller *controller)
{
	_belongsTo = controller;
}