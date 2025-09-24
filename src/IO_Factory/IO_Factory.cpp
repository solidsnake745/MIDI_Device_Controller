#include "IO_Factory.h"

IO_Factory::IO_Factory()
{
	for(int i = 0; i < MAX_IO_DEVICES; i++)
		_ioDevices[i] = nullptr;
}

IO_Factory& IO_Factory::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == nullptr) _instance = new IO_Factory();
	return *_instance;
}

IO_Device* IO_Factory::getIO(IOType type)
{
	uint8_t i = static_cast<uint8_t>(type);
	if(_ioDevices[i])
		return _ioDevices[i];
	
	return nullptr;
}

IO_DigitalWrite* IO_Factory::createDigitalIO(uint8_t numOutputs)
{
	uint8_t i = static_cast<uint8_t>(IODigital);
	if(_ioDevices[i])
		return (IO_DigitalWrite*) _ioDevices[i];
	
	_ioDevices[i] = new IO_DigitalWrite(numOutputs);
	return (IO_DigitalWrite*) _ioDevices[i];
}

IO_74HC595* IO_Factory::create74HC595(uint8_t numRegisters, uint8_t latchPin)
{
	uint8_t i = static_cast<uint8_t>(IO74HC595);
	if(_ioDevices[i])
		return (IO_74HC595*) _ioDevices[i];
	
	_ioDevices[i] = new IO_74HC595(numRegisters, latchPin);
	return (IO_74HC595*) _ioDevices[i];
}