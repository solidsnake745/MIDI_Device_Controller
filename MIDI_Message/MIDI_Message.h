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
		MIDI_Message(MIDI_MessageType_t t, uint8_t ch, uint8_t d1, uint8_t d2) : MIDI_Message(d1, d2)
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
		
		inline void setData(uint8_t da1, uint8_t da2 = 0) { setData1(da1); setData2(da2); };
		
		inline MIDI_MessageType_t getType() 
		{
			switch(_status >> 4)
			{
				case 0: return Undefined;
				case 8: return NoteOff;
				case 9: return NoteOn;
				case 10: return PolyPressure;
				case 11: return ControlChange;
				case 12: return ProgramChange;
				case 13: return ChannelPressure;
				case 14: return PitchBend;
				default: return Undefined;
			}
		};
		
		inline uint8_t getChannel() { return _status & 0x0F; };    
		inline uint8_t getStatus() { return _status; };
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
			test.setType(NoteOn);
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

			inline void setType(MIDI_MessageType_t t)
			{
				uint8_t msbData;
				uint8_t lsbData = (_status & B00001111);

				switch(t)
				{
					case Undefined: msbData = 0; break;
					case NoteOff: msbData = 8 << 4; break;
					case NoteOn: msbData = 9 << 4; break;
					case PolyPressure: msbData = 10 << 4; break;
					case ControlChange: msbData = 11 << 4; break;
					case ProgramChange: msbData = 12 << 4; break;
					case ChannelPressure: msbData = 13 << 4; break;
					case PitchBend: msbData = 14 << 4; break;
					default: msbData = 0; break;
				}

				//Set new value      
				_status = msbData + lsbData;
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
			
			inline void setStatus(uint8_t st)
			{
				uint8_t type = st >> 4;
				if(type > 7 && type < 15)					
					setType(static_cast<MIDI_MessageType_t>(type));
		
				setChannel(st & 0x0F);
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
