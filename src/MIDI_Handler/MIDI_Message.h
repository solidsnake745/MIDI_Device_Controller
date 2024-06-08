#ifndef MIDI_Message_h
	#define MIDI_Message_h
	
	#include "MIDI_Enums.h"
	#include <Arduino.h>
	#include "../SerialDebug/SerialDebug.h"
	
	//Standard MIDI message interface
	//MIDI messages are 3 byte messages 
	//http://www.songstuff.com/recording/article/midi_message_format/
	//http://www.electronics.dit.ie/staff/tscarff/Music_technology/midi/midi_messages.htm
	/// @private
	struct MIDI_Message
	{
		private:
			inline static SerialDebug _debug = SerialDebug(5);
			uint8_t _status = 0, _data1 = 0, _data2 = 0;		

			void setType(MsgType t);
			bool setChannel(uint8_t ch);
			void setStatus(uint8_t st);
			void setData1(uint8_t da1);
			void setData2(uint8_t da2);
			void setBendValue(uint16_t bv);
		
		public:
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
			
			MsgType getType();
			inline uint8_t getChannel() { return _status & 0x0F; };   
			inline uint8_t getStatus() { return _status; };
			inline uint8_t getData1() { return _data1; };
			inline uint8_t getData2() { return _data2; };
			
			//Pitchbend data is a 14 bit value made up of data 1 and data 2
			//MIDI data packets are 7 bits stored in a byte (8 bits)
			//The LSB is in data 1 and MSB in data 2
			//Combine data by shifting the MSB 7 bits and adding the LSB
			inline uint16_t getBendValue() { return (((uint16_t) getData2() << 7) + getData1()); };

			inline void setData(uint8_t da1, uint8_t da2 = 0) { setData1(da1); setData2(da2); };
			
			#if INCLUDE_TESTS
			inline static void runTest()
			{
				//Initial set and check
				_debug.debugln(5, F("Initial set of a message"));				
				MIDI_Message test(B10001010, 54, 126);
				_debug.debugln(5, F("Status: %d"), test.getType()); //8
				_debug.debugln(5, F("Channel: %d"), test.getChannel()); //10
				_debug.debugln(5, F("Data1: %d"), test.getData1()); //54
				_debug.debugln(5, F("Data2: %d"), test.getData2()); //126

				//Increment all values by 1 and check
				_debug.debugln(5, F("Increment all previous values by 1"));
				test.setType(NoteOn);
				test.setChannel(11);
				test.setData1(55);
				test.setData2(127);
				_debug.debugln(5, F("Status: %d"), test.getType()); //9
				_debug.debugln(5, F("Channel: %d"), test.getChannel()); //11
				_debug.debugln(5, F("Data1: %d"), test.getData1()); //55
				_debug.debugln(5, F("Data2: %d"), test.getData2()); //127

				//Set bend value and check
				_debug.debugln(5, F("Set bend value to 127"));
				test.setBendValue(127);
				_debug.debugln(5, F("Bend value: %d"), test.getBendValue()); //MSB B00000000 + LSB B01111111 = 127
				
				_debug.debugln(5, F("Set bend value to 128"));
				test.setBendValue(128);
				_debug.debugln(5, F("Bend value: %d"), test.getBendValue()); //MSB B00000001 + LSB B00000000 = 128
				
				_debug.debugln(5, F("Set bend value to 256"));
				test.setBendValue(256);
				_debug.debugln(5, F("Bend value: %d"), test.getBendValue()); //MSB B00000010 + LSB B00000000 = 256
			};
			#endif
	};

#endif
