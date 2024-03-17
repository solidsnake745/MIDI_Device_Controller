#ifndef noteDurationTracker_h
  #define noteDurationTracker_h

  #include <stdint.h>  

  //@private
  struct noteDuration
  {
    public:
      noteDuration(){};
    
      //Create new object with microseconds, milliseconds, and/or seconds
      noteDuration(uint16_t us, uint16_t ms = 0, uint16_t s = 0)
      {
        addSeconds(s);
        addMilliseconds(ms);
        addMicroseconds(us);
      };
    
      void reset() 
      {
        seconds = 0;
        millisec = 0;
        microsec = 0;
      };
      
      bool isZero() 
      {
        return seconds == 0 && millisec == 0 && microsec == 0;
      };

      bool operator <(noteDuration &nd) 
      {
        if(seconds > nd.seconds)
          return false;

        if(millisec > nd.millisec)
          return false;
          
        if(microsec > nd.microsec)
          return false;  

        return true;
      };

      void addSeconds(uint32_t s) { addMilliseconds(s * 1000); };
      void addMilliseconds(uint32_t ms) { addMicroseconds(ms * 1000); };

      void addMicroseconds(uint32_t us)
      {
        //Add microseconds passed in
        while(us >= 1000000)
        {
          us -= 1000000;			
          seconds += 1;
        }
        
        while(us >= 1000)
        {
          us -= 1000;			
          millisec += 1;
        }
        
        microsec += us;
        
        //Handle carry over logic
        while(microsec >= 1000)
        {
          microsec -= 1000;
          millisec += 1;
        }

        while(millisec >= 1000)
        {
          millisec -= 1000;
          seconds += 1; 
        }
      };

      uint16_t seconds = 0, millisec = 0, microsec = 0;

    private:
      //Max value is 65535, class is meant to track note duration so shouldn't need a higher value
      //uint16_t seconds = 0, millisec = 0, microsec = 0;
  };

  //@private
  //Tracks durations of notes
  // class noteDurationTracker
  // {
  //   public:
  //     noteDurationTracker() {};
  //     inline void reset() { _duration.reset(); };
  //     inline int getSeconds() { return _duration.seconds; };
  //     inline int getMilliseconds() { return _duration.millisec; };
  //     inline int getMicroseconds() { return _duration.microsec; };
  //     inline void addMicroseconds(uint32_t s) { _duration.addMicroseconds(s); };
      
  //     inline bool operator <(noteDuration &nd) { return _duration < nd; };
      
  //   private:
  //      noteDuration _duration;	   
  // };

#endif
