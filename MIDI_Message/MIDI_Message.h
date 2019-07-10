#ifndef MIDI_Message_h
	#define MIDI_Message_h

	//Standard MIDI message types
	//Message types range between 0-15 (not all values currently in use)
	//Limited to 4 bit values though stored in 8 bit field
	enum MIDI_MessageType_t : uint8_t
	{
		Undefined = 0,
		NoteOff = 8,
		NoteOn = 9,
		PolyPressure = 10,
		ControlChange = 11,
		ProgramChange = 12,
		ChannelPressure = 13,
		PitchBend = 14
	};
		
	//Standard MIDI message interface
	//MIDI messages are 3 byte messages 
	//http://www.songstuff.com/recording/article/midi_message_format/
	//http://www.electronics.dit.ie/staff/tscarff/Music_technology/midi/midi_messages.htm
	struct MIDI_Message
	{
		MIDI_Message(MIDI_MessageType_t st, uint8_t ch, uint8_t d1, uint8_t d2)
		{
			setStatus(st);
			setChannel(ch);
			setData1(d1);
			setData2(d2);
		}

		MIDI_Message(uint8_t st, uint8_t d1, uint8_t d2)
		{
			_status = st;
			_data1 = d1;
			_data2 = d2;
		}
		
		MIDI_Message(uint8_t st)
		{
			_status = st;
		}
		
		inline void setData(uint8_t da1, uint8_t da2 = 0) { setData1(da1); setData2(da2); };
		
		inline uint8_t getStatus() { return _status >> 4; };    
		inline uint8_t getChannel() { return _status & 0x0F; };    
		inline uint8_t getByte1() { return _status; };
		inline uint8_t getData1() { return _data1; };
		inline uint8_t getData2() { return _data2; };
		
		inline uint16_t getBendValue() { return (((uint16_t) getData2() << 8) + getData1()); };

		inline static void runTest()
		{
			//Initial set and check
			MIDI_Message test(B10001010, 54, 126);
			// PRINT2("Status: ", test.getStatus()); //8
			// PRINT2("Channel: ", test.getChannel()); //10
			// PRINT2("Data1: ", test.getData1()); //54
			// PRINT2("Data2: ", test.getData2()); //126

			//Increment all values by 1 and check
			test.setStatus(NoteOn);
			test.setChannel(11);
			test.setData1(55);
			test.setData2(127);
			// PRINT2("Status: ", test.getStatus()); //9
			// PRINT2("Channel: ", test.getChannel()); //11
			// PRINT2("Data1: ", test.getData1()); //55
			// PRINT2("Data2: ", test.getData2()); //127

			//Set bend value and check
			test.setBendValue(0x0107);
			// PRINT2("Bend Value: ", test.getBendValue()); //256 + 7 = 263
		};
		
		private: 
			uint8_t _status = 0x0;
			uint8_t _data1 = 0x0;
			uint8_t _data2 = 0x0;

			inline bool setStatus(MIDI_MessageType_t st)
			{
				uint8_t msbData;
				uint8_t lsbData = (_status & B00001111);

				switch(st)
				{
					case Undefined: msbData = 0; break;
					case NoteOff: msbData = 8 << 4; break;
					case NoteOn: msbData = 9 << 4; break;
					case PolyPressure: msbData = 10 << 4; break;
					case ControlChange: msbData = 11 << 4; break;
					case ProgramChange: msbData = 12 << 4; break;
					case ChannelPressure: msbData = 13 << 4; break;
					case PitchBend: msbData = 14 << 4; break;
					default:
						// DEBUG("Invalid setStatus")
						return false;
				}

				//Set new value      
				_status = msbData + lsbData;
				return true;
			};

			inline bool setChannel(uint8_t ch)
			{
				//Validate
				if (ch > 15)
				{
					//DEBUG2("Invalid setChannel: ", ch)
					return false;
				}

				//Calculate and set new value
				uint8_t newValue = (_status & B11110000) + ch;
				_status = newValue;
				return true;
			};

			inline void setData1(uint8_t da1) { _data1 = da1; };
			inline void setData2(uint8_t da2) { _data2 = da2; };
			
			inline void setBendValue(uint16_t bv)
			{
				//0 to 16,383
				setData1(bv & 0xFF); //LSB of bv
				setData2(bv >> 8); //MSB of bv
			};

	//    MIDIEvent(uint8_t rawData[])
	//    {
	//      if(sizeof(rawData) != 3)
	//      {
	//        DEBUG(F("Unusual MIDI data size"))
	//        _status = 0;
	//      }
	//      else
	//      {
	//        _status = rawData[0];
	//        _data1 = rawData[1];
	//        _data2 = rawData[2];
	//      }      
	//    }
	//    

	//    //TODO: Very specific use case, not sure if this is useful
	//    MIDIEvent(uint32_t rawData)
	//    {
	//      if(rawData & 0x0888 == 2048)
	//      {
	//        DEBUG("MIDI found")
	//      }
	//      else if((rawData >> 4) & 0x0888 == 2048)
	//      {
	//        DEBUG("MIDI found after shifiting")
	//      }
	//
	//      //TODO: Parse out information
	//    }
	};

#endif
