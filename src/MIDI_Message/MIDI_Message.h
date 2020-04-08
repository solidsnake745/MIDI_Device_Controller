#ifndef MIDI_Message_h
	#define MIDI_Message_h
	
	#include "../MIDI_Enums/MIDI_Enums.h"
	// #include <stdint.h>
	#include <Arduino.h>
	
	//Standard MIDI message interface
	//MIDI messages are 3 byte messages 
	//http://www.songstuff.com/recording/article/midi_message_format/
	//http://www.electronics.dit.ie/staff/tscarff/Music_technology/midi/midi_messages.htm
	struct MIDI_Message
	{
		MIDI_Message(MsgType t, uint8_t ch, uint8_t d1, uint8_t d2) : MIDI_Message(d1, d2)
		{
			setType(t);
			setChannel(ch);
		}

		MIDI_Message(uint8_t st, uint8_t d1, uint8_t d2) : MIDI_Message(d1, d2)
		{
			setStatus(st);
		}
		
		MIDI_Message(uint8_t d1, uint8_t d2)
		{
			setData1(d1);
			setData2(d2);
		}
		
		MIDI_Message(uint8_t st)
		{
			setStatus(st);
		}
		
		void setData(uint8_t da1, uint8_t da2 = 0);
		
		MsgType getType();
		
		uint8_t getChannel();   
		uint8_t getStatus();
		uint8_t getData1();
		uint8_t getData2();
		
		uint16_t getBendValue();

		static void runTest();
		
		private: 
			uint8_t _status = 0x0;
			uint8_t _data1 = 0x0;
			uint8_t _data2 = 0x0;

			void setType(MsgType t);

			bool setChannel(uint8_t ch);
			
			void setStatus(uint8_t st);

			void setData1(uint8_t da1);
			void setData2(uint8_t da2);
			
			void setBendValue(uint16_t bv);
	};

#endif
