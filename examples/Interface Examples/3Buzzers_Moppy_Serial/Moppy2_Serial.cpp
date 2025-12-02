#include "Moppy2_Serial.h"

//Constructors and instance management
//_______________________________________________________________________________________________________
Moppy2_Serial* Moppy2_Serial::_instance = NULL;

Moppy2_Serial::Moppy2_Serial()
{
	Serial.setTimeout(10);
}

Moppy2_Serial& Moppy2_Serial::getInstance()
{
	//Single instance check, instantiation, and return
	if (_instance == NULL) _instance = new Moppy2_Serial();
	return *_instance;
}

//Serial Handling
//_______________________________________________________________________________________________________
void Moppy2_Serial::parseSerial()
{
	if(Serial.read() != START_BYTE)
		return; //Skip over data until we find the start byte

	//Get first 3 parts of the message that are guaranteed to be there
	char msgBuffer[3];
	Serial.readBytes(msgBuffer, 3);
	uint8_t
		deviceAddress = (uint8_t) msgBuffer[0],
		subAddress = (uint8_t) msgBuffer[1],
		payloadSize = (uint8_t) msgBuffer[2];

	//Get the remaining, variably sized payload data
	char payloadBuffer[payloadSize]; //On the stack and will get released once out of parseSerial()
	Serial.readBytes(payloadBuffer, payloadSize);

	Moppy_Message msg(deviceAddress, subAddress, payloadSize, payloadBuffer);

	if(msg.getDeviceAddress() != SYSTEM_ADDRESS && msg.getDeviceAddress() != _deviceAddress)
		return; //Message not meant for this device, carry on

	if(msg.getSubAddress() < _minSubAddress || msg.getSubAddress() > _maxSubAddress)
		return; //Device does not support this sub address, carry on 

	//Handle the message accordingly
	if(msg.getDeviceAddress() == SYSTEM_ADDRESS)
		handleSystemMessage(&msg);
	else
		handleDeviceMessage(&msg);
}

void Moppy2_Serial::process()
{
	if(Serial.available() >= 4)	
		parseSerial();
}

//Message Handling
//_______________________________________________________________________________________________________
void Moppy2_Serial::sendPong()
{
	uint8_t pongBytes[8] = {START_BYTE, 0x00, 0x00, 0x04, 0x81, _deviceAddress, _minSubAddress, _maxSubAddress};
	Serial.write(pongBytes, sizeof(pongBytes));
}

void Moppy2_Serial::handleSystemMessage(Moppy_Message* msg)
{
	switch(msg->getCommand())
	{
		case NETBYTE_SYS_PING:
			sendPong();
			break;
		default:
			if(_handleCustomSystem)
				_handleCustomSystem(msg);
			break;
	}
}

void Moppy2_Serial::handleDeviceMessage(Moppy_Message* msg)
{
	switch(msg->getCommand())
	{
		case NETBYTE_DEV_NOTEOFF:
			_midiHandler.noteOff(msg->getSubAddress(), msg->getPayloadByte(1), msg->getPayloadByte(2));
			break;
		case NETBYTE_DEV_NOTEON:
			_midiHandler.noteOn(msg->getSubAddress(), msg->getPayloadByte(1), msg->getPayloadByte(2));
			break;
		case NETBYTE_DEV_BENDPITCH:
		{ 
			//Scope this case so we can locally declare bendDeflection
			int16_t bendDeflection = msg->getPayloadByte(1) << 8 | msg->getPayloadByte(2); //Stolen from Moppy2/MoppyInstruments
			_midiHandler.pitchBend(msg->getSubAddress(), bendDeflection);
		}
			break;
		default:
			if(_handleCustomDevice)
				_handleCustomDevice(msg);
			break;
	}
}