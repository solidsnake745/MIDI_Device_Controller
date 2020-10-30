#ifndef byteBits_h
  #define byteBits_h

  #include <stdint.h>  
  
  struct eightBits
  {
    volatile unsigned 
    b0 : 1,
    b1 : 1,
    b2 : 1,
    b3 : 1,
    b4 : 1,
    b5 : 1,
    b6 : 1,
    b7 : 1;
  };
  
  union byteBits
  {
    uint8_t value;
    eightBits bits;
  
    bool getBit(uint8_t index) volatile
    {
      switch(index)
      {
        case 0: return bits.b0;
        case 1: return bits.b1;
        case 2: return bits.b2;
        case 3: return bits.b3;
        case 4: return bits.b4;
        case 5: return bits.b5;
        case 6: return bits.b6;
        case 7: return bits.b7;
        default: return false;
      }
    }
      
    void setBit(uint8_t index) volatile
    {
      switch(index)
      {
        case 0: bits.b0 = 1; break;
        case 1: bits.b1 = 1; break;
        case 2: bits.b2 = 1; break;
        case 3: bits.b3 = 1; break;
        case 4: bits.b4 = 1; break;
        case 5: bits.b5 = 1; break;
        case 6: bits.b6 = 1; break;
        case 7: bits.b7 = 1; break;
        default: return;
      }
    }
    
    void clearBit(uint8_t index) volatile
    {
      switch(index)
      {
        case 0: bits.b0 = 0; break;
        case 1: bits.b1 = 0; break;
        case 2: bits.b2 = 0; break;
        case 3: bits.b3 = 0; break;
        case 4: bits.b4 = 0; break;
        case 5: bits.b5 = 0; break;
        case 6: bits.b6 = 0; break;
        case 7: bits.b7 = 0; break;
        default: return;
      }
    }
  };

#endif
