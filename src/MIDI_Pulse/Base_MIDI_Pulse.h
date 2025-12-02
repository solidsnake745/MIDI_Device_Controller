#ifndef Base_MIDI_Pulse_h
	#define Base_MIDI_Pulse_h

	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	
	//Forward declaration for compiling	
	// class MIDI_Device_Controller;

	class Base_MIDI_Pulse
	{
		friend class MIDI_Device_Controller;
		
		// virtual void checkMaxDuration() = 0;		
		virtual void processNotes() = 0;
		
		protected:
			inline static SerialDebug _debug = SerialDebug(DEBUG_MIDIPULSE);
			
			uint8_t _id;
			MIDI_Device_Controller* _parent = nullptr;
			
			//Designated output mapping where -1 indicates no out is assigned
			int8_t _outNum = -1;
			IO_Device* _outIO = nullptr;
			
		public:
			virtual ~Base_MIDI_Pulse() {};
			
			inline uint8_t getID() { return _id; };
			inline int8_t getOutputNum() { return _outNum; };
			
			//Sets the associated output
			inline void setOutput(IOType type, int8_t outNum)
			{ 
				_outNum = outNum;
				if (_outNum < 0)
				{
					_outIO = nullptr;
					return;
				}
				
				_outIO = IOF.getIO(type);
				if(_outIO && _outIO->isValidMapping(_outNum))
				{
					//Below taken from MIDI_Pitch logic, but starting to lean towards refactoring out duration tracking in favor of relying on idle timeout to stop playback
					//_outIO->setMaxDuration(_outNum, 0);
				}
				else
					_outNum = -1;
			};
			
			inline bool getOutputState()
			{
				if(!_outIO)
				{
					_debug.debugln(7, F("%d - Pulse output not setup"), _id);
					return false;
				}
				
				return _outIO->getOutput(_outNum);
			};
			
			virtual void pulse() = 0;
			virtual void stopPulse() = 0;
	};
#endif