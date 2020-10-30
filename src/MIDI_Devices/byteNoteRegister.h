#ifndef byteNoteRegister_h
  #define byteNoteRegister_h

  #include "byteBits.h"
  #include "noteDurationTracker.h"

  class byteNoteRegister
  {
    public:
      byteNoteRegister();
      bool updateDurations(uint8_t period, noteDuration maxDuration) ;

      inline bool getOutput(uint8_t i) { return _data.getBit(i); };
      inline uint8_t getOutputs() { return _data.value; };
      inline void setOutput(uint8_t i) { _data.setBit(i); };
      inline void clearOutput(uint8_t i) 
      {
        _data.clearBit(i);
        _durations[i].reset();
      };

    private:
      byteBits _data;
      noteDurationTracker _durations[8];
  };
  
#endif
