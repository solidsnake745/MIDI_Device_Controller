/// @file MIDI_Message.h
/// @brief An optional header that defines a standard MIDI message data type
/// @details To include from the sketch level, use: @code #include <MIDI_Handler\MIDI_Message.h> @endcode
/// Lots of frameworks provide some version of this so made a conscious decision not to tie the library to any specific one.<br>
///	Can be used in conjunction with MIDI_Handler to implement a automatic handler mechanism for incoming MIDI data/events.<br>

#ifndef MIDI_Message_h
	#define MIDI_Message_h
	
	#if !defined(MIDIMSG_INCLUDE_SERIALDEBUG)
		#define MIDIMSG_INCLUDE_SERIALDEBUG 0
	#endif
	
	#include <Arduino.h>
	#if MIDIMSG_INCLUDE_SERIALDEBUG
		#include "../Common/SerialDebug.h"
	#endif
	
	/// @brief Standard MIDI message types
	/// @details Message types range between 0-15 (not all values currently in use).<br>
	/// Also can't match the class name so underscore is not included.
	enum MIDI_MsgType
	{
		/// @brief Default representing invalid/no value
		Undefined = -1,
		/// @brief Note Off
		NoteOff = 8,
		/// @brief Note On
		NoteOn = 9,
		/// @brief Poly Pressure
		PolyPressure = 10,
		/// @brief Control Change (CC)
		ControlChange = 11,
		/// @brief Program Change
		ProgramChange = 12,
		/// @brief Channel Pressure
		ChannelPressure = 13,
		/// @brief Pitch Bend
		PitchBend = 14
	};
	
	/// @brief A data class to represent a MIDI message
	/// @details Used to store MIDI data and help facilitate handling incoming events.<br>
	///	3 byte long messages consisting of status, data1, and data2.<br>
	///	Implementation based off of details from:<br>
	/// http://www.songstuff.com/recording/article/midi_message_format/<br>
	/// http://www.electronics.dit.ie/staff/tscarff/Music_technology/midi/midi_messages.htm<br>
	/// TODO: Consider updating implementation to set MSB (bit) of status, data1, data2 to 1 as it is in actual MIDI data.
	struct MIDI_Message
	{
		private:
		#if MIDIMSG_INCLUDE_SERIALDEBUG
			inline static SerialDebug _debug = SerialDebug(OFF);
		#endif
			
			//Data members
			uint8_t _status = 0, _data1 = 0, _data2 = 0;		

			//Setters
			void setType(MIDI_MsgType t);
			bool setChannel(uint8_t ch);
			void setStatus(uint8_t st);
			void setData1(uint8_t da1);
			void setData2(uint8_t da2);
			void setBendValue(uint16_t bv);
		
		public:
			/// @param t Message Type (MIDI_MsgType)
			/// @param ch Message Channel (0 - 16)
			/// @param d1 Message Data1 (0 - 127)
			/// @param d2 Message Data2 (0 - 127)
			MIDI_Message(MIDI_MsgType t, uint8_t ch, uint8_t d1, uint8_t d2);
			
			/// @param st Message Status (0 - 127)
			/// @param d1 Message Data1 (0 - 127)
			/// @param d2 Message Data2 (0 - 127)
			MIDI_Message(uint8_t st, uint8_t d1, uint8_t d2);
			
			/// @param d1 Message Data1 (0 - 127)
			/// @param d2 Message Data2 (0 - 127)
			MIDI_Message(uint8_t d1, uint8_t d2);
			
			/// @param st Message Status (0 - 127)
			MIDI_Message(uint8_t st);
			
			/// @brief Gets the type of this message from the status byte
			MIDI_MsgType getType();
			
			/// @brief Gets the channel of this message from the status byte
			inline uint8_t getChannel() { return _status & 0x0F; };
			
			/// @brief Gets the type of the status byte of this message
			inline uint8_t getStatus() { return _status; };
			
			/// @brief Gets the data1 byte of this message
			inline uint8_t getData1() { return _data1; };
			
			/// @brief Gets the data2 byte of this message
			inline uint8_t getData2() { return _data2; };
			
			/// @brief Gets the pitch bend value of this message by combining data1 and data2
			/// @details Pitchbend data is a 14 bit value made up of data 1 and data 2.<br>
			/// MIDI data packets are 7 bits stored in a byte (8 bits).<br>
			/// The LSB is in data 1 and MSB in data 2.<br>
			/// Combine data by shifting the MSB 7 bits and adding the LSB.
			/// @return Returns a value between 0 to 16838 per original MIDI data. Note that calculations are expecting -8192 to 8191.
			inline uint16_t getBendValue() { return (((uint16_t) getData2() << 7) + getData1()); };

			/// @brief Gets the pitch bend value of this message by combining data1 and data2
			/// @param da1 Byte for data1
			/// @param da2 Optional byte for data2
			inline void setData(uint8_t da1, uint8_t da2 = 0) { setData1(da1); setData2(da2); };
			
			#if INCLUDE_TESTS
			inline static void runTest()
			{
				//Initial set and check
				_debug.println(F("Initial set of a message"));				
				MIDI_Message test(0b10001010, 54, 126);
				_debug.println(F("Status: %d"), test.getType()); //8
				_debug.println(F("Channel: %d"), test.getChannel()); //10
				_debug.println(F("Data1: %d"), test.getData1()); //54
				_debug.println(F("Data2: %d"), test.getData2()); //126

				//Increment all values by 1 and check
				_debug.println(F("Increment all previous values by 1"));
				test.setType(NoteOn);
				test.setChannel(11);
				test.setData1(55);
				test.setData2(127);
				_debug.println(F("Status: %d"), test.getType()); //9
				_debug.println(F("Channel: %d"), test.getChannel()); //11
				_debug.println(F("Data1: %d"), test.getData1()); //55
				_debug.println(F("Data2: %d"), test.getData2()); //127

				//Set bend value and check
				_debug.println(F("Set bend value to 127"));
				test.setBendValue(127);
				_debug.println(F("Bend value: %d"), test.getBendValue()); //MSB B00000000 + LSB B01111111 = 127
				
				_debug.println(F("Set bend value to 128"));
				test.setBendValue(128);
				_debug.println(F("Bend value: %d"), test.getBendValue()); //MSB B00000001 + LSB B00000000 = 128
				
				_debug.println(F("Set bend value to 256"));
				test.setBendValue(256);
				_debug.println(F("Bend value: %d"), test.getBendValue()); //MSB B00000010 + LSB B00000000 = 256
			};
			#endif
	};

#endif
