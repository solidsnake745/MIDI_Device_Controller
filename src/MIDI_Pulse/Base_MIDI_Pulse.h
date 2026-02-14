#ifndef Base_MIDI_Pulse_h
	#define Base_MIDI_Pulse_h

	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	
	//Forward declaration for compiling	
	class MIDI_Device_Controller;

	class Base_MIDI_Pulse
	{
		//Give access to all private members to the below classes
		friend class MIDI_Device_Controller;

		virtual void processNotes() = 0;
		
		protected:
			inline static SerialDebug _debug = SerialDebug(DEBUG_MIDIPULSE);
			inline void callUpdateOutputs(IO_Device* io) { io->updateOutputs(); };
			
			uint8_t _id;
			MIDI_Device_Controller* _parent = nullptr;
			void noteAssigned();
			
			//Designated output mapping where -1 indicates no out is assigned
			int8_t _outNum = -1;
			IO_Device* _outIO = nullptr;
			
		public:
			inline Base_MIDI_Pulse() {};
			inline Base_MIDI_Pulse(IOType type, int8_t outNum) { setOutput(type, outNum); };
			virtual ~Base_MIDI_Pulse() {};
			
			inline uint8_t getID() { return _id; };
			inline int8_t getOutputNum() { return _outNum; };
			
			//Sets the associated output
			inline void setOutput(IOType type, int8_t outNum)
			{ 
				_outNum = outNum;
				if (_outNum < 0)
				{
					_debug.println(F("%d - Clearing currently configured output"), _id);
					_outIO = nullptr;
					return;
				}
				
				_outIO = IOF.getIO(type);
				if(!_outIO || _outIO->isValidMapping(_outNum))
					_outNum = -1;
			};
			
			inline bool getOutputState()
			{
				if(!_outIO)
				{
					_debug.println(F("%d - Pulse output not setup"), _id);
					return false;
				}
				
				return _outIO->getOutput(_outNum);
			};
			
			virtual void pulse() = 0;
			virtual void stopPulse() = 0;
	};
	
#endif