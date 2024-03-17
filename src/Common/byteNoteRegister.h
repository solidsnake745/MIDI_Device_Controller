#ifndef byteNoteRegister_h
  #define byteNoteRegister_h

  #include "byteBits.h"
  #include "noteDuration.h"

  class byteNoteRegister
  {
    public:
      byteNoteRegister() {};
      bool updateDurations(uint8_t period)
      {
        bool outputChanged = false;

        for(int x = 0; x < 8; x++)
        {
          if(!_data.getBit(x))
            continue;

          _durations[x].addMicroseconds(period);

          if(_maxDurations[x].isZero())
            continue;

          if(_durations[x] < _maxDurations[x])
            continue;
          
          clearBit(x);
          outputChanged = true;
        }

        return outputChanged;
      };

      inline uint8_t getByteValue() { return _data.value; };
      inline bool getBit(uint8_t i) { return _data.getBit(i); };
      inline void setBit(uint8_t i) { _data.setBit(i); };    
      
      inline void clearBit(uint8_t i)
      {
        _data.clearBit(i);
        _durations[i].reset();
      };
      
      inline void setBitValue(uint8_t i, bool value) 
      { 
        _data.setBitValue(i, value);
        if(!value) _durations[i].reset();
      };
      
      inline void setMaxDuration(uint8_t i, uint32_t us)
      { 
        _maxDurations[i].reset(); 
        _maxDurations[i].addMicroseconds(us); 
      };
	      
    private:
      byteBits _data;
      noteDuration _durations[8];
	    noteDuration _maxDurations[8];
  };
  
#endif
