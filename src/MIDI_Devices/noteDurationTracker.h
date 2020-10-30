#ifndef noteDurationTracker_h
  #define noteDurationTracker_h

  #include <stdint.h>  

  //@private
  struct noteDuration
  {
    unsigned int 
      seconds : 10,
      millisec : 11,
      microsec : 11;

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
  };

  //@private
  //Tracks durations of notes
  class noteDurationTracker
  {
    public:
      noteDurationTracker();
      inline void reset() { _duration.reset(); };
      inline int getSeconds() { return _duration.seconds; };
      inline int getMilliseconds() { return _duration.millisec; };
      inline int getMicroseconds() { return _duration.microsec; };
      void addMicroseconds(uint32_t s) ;
      
      bool operator <(noteDuration &nd) 
      { return _duration < nd; };
      
    private:
       noteDuration _duration;
  };

#endif
