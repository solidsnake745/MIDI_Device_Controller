#include "IO_Factory.h"

//Global singleton instance
IO_Factory IOF = IO_Factory::getInstance();

IO_Factory *IO_Factory::_instance = NULL;

IO_Device *IO_Factory::_ioDevices[MAX_IO_DEVICES];

IO_Factory::IO_Factory()
{
	for(int i = 0; i < MAX_IO_DEVICES; i++)
		_ioDevices[i] = NULL;
}

IO_Factory &IO_Factory::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new IO_Factory();
	return *_instance;
};

IO_Device *IO_Factory::getDevice(IO_DeviceEnum device)
{
	uint8_t i = static_cast<uint8_t>(device);
	if(_ioDevices[i])
		return _ioDevices[i];
	
	return NULL;
}

IO_DigitalWrite *IO_Factory::createDigitalIO(uint8_t numOutputs)
{
	uint8_t i = static_cast<uint8_t>(DigitalWrite);
	if(_ioDevices[i])
		return (IO_DigitalWrite*) _ioDevices[i];
	
	_ioDevices[i] = new IO_DigitalWrite(numOutputs);
	return (IO_DigitalWrite*) _ioDevices[i];
}

IO_SN74HC595N *IO_Factory::createSN74HC595N(uint8_t numRegisters, uint8_t latchPin)
{
	uint8_t i = static_cast<uint8_t>(SN74HC595N);
	if(_ioDevices[i])
		return (IO_SN74HC595N*) _ioDevices[i];
	
	_ioDevices[i] = new IO_SN74HC595N(numRegisters, latchPin);
	return (IO_SN74HC595N*) _ioDevices[i];
}

// MIDI_SN74HC595N *IO_Factory::createSN74HC595N(uint8_t numOutputs, uint8_t latchPin)
// {
	// if(MDC.getSN74HC595N())
		// return MDC.getSN74HC595N();
	
	// MDC.setSN74HC595N(new MIDI_SN74HC595N(numOutputs, latchPin));
	// return MDC.getSN74HC595N();
// }