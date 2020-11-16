#ifndef noteDurationTracker_h
  #define noteDurationTracker_h

  #include <stdint.h>  

  //@private
  struct noteDuration
  {
    uint16_t 
		seconds,
		millisec,
		microsec;

    noteDuration(){ reset(); }
    
	noteDuration(uint16_t micro, uint16_t milli = 0, uint16_t sec = 0)
	{
		microsec = micro;
		millisec = milli;
		seconds = sec;
	}
	
    void reset() 
    {
      seconds = 0;
      millisec = 0;
      microsec = 0;
    }
    
    bool operator <(noteDuration &nd) 
    {
      if(microsec < nd.microsec)
        return true;

      if(millisec < nd.millisec)
        return true;

      if(seconds < nd.seconds)
        return true;
        
      return false;
    }
	
	void addMicroseconds(uint32_t s)
	{
		//Add microseconds passed in
		while(s >= 1000000)
		{
			s -= 1000000;			
			seconds += 1;
		}
		
		while(s >= 1000)
		{
			s -= 1000;			
			millisec += 1;
		}
		
		microsec += s;
		
		//Handle carry over logic
		if(microsec < 1000)
			return;

		while(microsec >= 1000)
		{
			microsec -= 1000;
			millisec += 1;
		}

		if(millisec < 1000)
			return;

		while(millisec >= 1000)
		{
			millisec -= 1000;
			seconds += 1; 
		}
	};
  };

  //@private
  //Tracks durations of notes
  class noteDurationTracker
  {
    public:
      noteDurationTracker() {};
      inline void reset() { _duration.reset(); };
      inline int getSeconds() { return _duration.seconds; };
      inline int getMilliseconds() { return _duration.millisec; };
      inline int getMicroseconds() { return _duration.microsec; };
      inline void addMicroseconds(uint32_t s) { _duration.addMicroseconds(s); };
      
      inline bool operator <(noteDuration &nd) { return _duration < nd; };
      
    private:
       noteDuration _duration;
  };

#endif
