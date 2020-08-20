#ifndef Base_MIDI_Device_Collection_h
	#define Base_MIDI_Device_Collection_h

	#include "../Settings.h"
	#include "../MIDI_Device_Controller/MIDI_Pitch.h"
	#include "../SerialDebug/SerialDebug.h"

	struct MIDI_Pitch_Node;

	//A doubly linked list of devices used for pooling note assignments
	class Base_MIDI_Pitch_Collection
	{
		//Give MIDI_Pitch_Node access to all private members
		friend class MIDI_Pitch_Node;
		
		void deleteNode(MIDI_Pitch_Node *node);
		
		protected:
			MIDI_Pitch_Node *start = NULL;
			MIDI_Pitch_Node *end = NULL;
			uint8_t _count = 0;
			static SerialDebug _debug;
			
		public:
			virtual ~Base_MIDI_Pitch_Collection();
			
			void addDevice(MIDI_Pitch *d);
			void addDevices(MIDI_Pitch devices[], uint16_t numDevices); //Adds the given set of devices
			void removeDevice(uint8_t id);
			
			//Overrideable methods
			virtual void printStatus();
			virtual bool playNote(uint8_t note);			
			virtual void bendNote(uint16_t bend);
			virtual void stopNote(uint8_t note);
			
			//Test methods
			void testPitchBend();
	};
#endif