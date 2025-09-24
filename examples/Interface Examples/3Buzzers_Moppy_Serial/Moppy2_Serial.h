#ifndef Moppy2_Serial_h
	#define Moppy2_Serial_h

	#include <Arduino.h>
	#include <MIDI_Handler\MIDI_Handler.h>

	//Constants per the Moppy2 framework
  #define START_BYTE 0x4d
  #define SYSTEM_ADDRESS 0x00

  #define NETBYTE_SYS_PING 0x80
  #define NETBYTE_SYS_PONG 0x81
  #define NETBYTE_SYS_RESET 0xff
  #define NETBYTE_SYS_START 0xfa
  #define NETBYTE_SYS_STOP 0xfc

  #define NETBYTE_DEV_RESET 0x00
  #define NETBYTE_DEV_NOTEOFF 0x08
  #define NETBYTE_DEV_NOTEON 0x09
  #define NETBYTE_DEV_BENDPITCH 0x0e

  //Below are not used but included for consistency with the Moppy2 library
  // Microcontroller/device-specific commands (still defined here to prevent overlap)
  #define NETBYTE_DEV_SETTARGETCOLOR 0x61
  #define NETBYTE_DEV_SETBGCOLOR 0x62
  #define NETBYTE_DEV_SETMOVEMENT 0x64

  struct Moppy_Message
  {
    Moppy_Message(uint8_t deviceAddress, uint8_t subAddress, uint8_t payloadSize, char* payload)
    {
      _deviceAddress = deviceAddress;
      _subAddress = subAddress;
      _payloadSize = payloadSize;
      _payload = payload;
    }

    ~Moppy_Message()
    {
      //A word on memory management concerning _payload
      //We're pointing to an existing array so we can't manage it here
      //Ideally you're using an array that's on the stack and will go out of scope
      //Otherwise you should be using an array you're managing/persisting in code between calls
    }

    private:
      uint8_t _deviceAddress = 0;
      uint8_t _subAddress = 0;
      uint8_t _payloadSize = 0; //Size of the payload including the command byte (so data + 1)    
      char *_payload; //Pointer to the buffer array holding the payload data

    public:
      inline uint8_t getDeviceAddress() { return _deviceAddress; };
      inline uint8_t getSubAddress() { return _deviceAddress; };
      inline uint8_t getPayloadSize() { return _payloadSize; };
      inline uint8_t getCommand() { return (uint8_t)(*(_payload)); }; //Command byte is always the first byte in the payload
      inline char* getPayload() { return _payload; }
      inline uint8_t getPayloadByte(uint16_t index) 
      { 
        if(_payload && (index < _payloadSize))
          return (uint8_t)(*(_payload + index));

        return 0; //Have to return something even though this might look like valid data
      };
  };

	class Moppy2_Serial
	{
		//Constructors and instance management
		//_______________________________________________________________________________________________________
		private:
			Moppy2_Serial();
			static Moppy2_Serial* _instance;
			
		public:
			//Used to populate our single instance MS for consumption
			/// @private
			static Moppy2_Serial& getInstance();

    //Setup
    //_______________________________________________________________________________________________________
    private:
      uint8_t _deviceAddress = 0;
      uint8_t _minSubAddress = 0;
      uint8_t _maxSubAddress = 0;

    public:
      inline void setup(uint8_t deviceAddress, uint8_t minSubAddress, uint8_t maxSubAddress)
      {
        _deviceAddress = deviceAddress;
        _minSubAddress = minSubAddress;
        _maxSubAddress = maxSubAddress;
      };

		//Serial Handling
		//_______________________________________________________________________________________________________
		private:
			void parseSerial();
      void sendPong();
		
		public:	
			void process();
		
		//Message Handling
		//_______________________________________________________________________________________________________
    private:
			MIDI_Handler _midiHandler;
      typedef void (*CustomSystemMessage) (Moppy_Message*);
      typedef void (*CustomDeviceMessage) (Moppy_Message*);
      CustomSystemMessage _handleCustomSystem = NULL;
      CustomDeviceMessage _handleCustomDevice = NULL;
      void handleSystemMessage(Moppy_Message *msg);
      void handleDeviceMessage(Moppy_Message *msg);

		public:
      inline void setCustomSystemHandle(void (*handle)(Moppy_Message*)) { _handleCustomSystem = handle; };
      inline void setCustomDeviceHandle(void (*handle)(Moppy_Message*)) { _handleCustomDevice = handle; };
			inline void setNoteOnHandle(void (*noteOn)(uint8_t, uint8_t, uint8_t)) { _midiHandler.setNoteOnHandle(noteOn); };
			inline void setNoteOffHandle(void (*noteOff)(uint8_t, uint8_t, uint8_t)) { _midiHandler.setNoteOffHandle(noteOff); };
			inline void setPitchBendHandle(void (*pitchBend)(uint8_t, int)) { _midiHandler.setPitchBendHandle(pitchBend); };
	};

	//Defines a global singleton instance of our class for users to consume
	inline Moppy2_Serial M2S = Moppy2_Serial::getInstance();
#endif