#ifndef PitchNode_h
	#define PitchNode_h

	#include <Arduino.h>
	#include "../SerialDebug/SerialDebug.h"
	
	//Forward declaration for compiling
	class MIDI_Pitch;
	class Base_MIDI_Pitch_Collection;

	/// @private
	struct MIDI_Pitch_Node
	{
		//Give Base_MIDI_Pitch_Collection access to all private members
		friend class Base_MIDI_Pitch_Collection;
		
		static SerialDebug _debug;
		
		private:
			MIDI_Pitch_Node(MIDI_Pitch *d, Base_MIDI_Pitch_Collection *dc);
			Base_MIDI_Pitch_Collection *parent = NULL;		
			
		public:
			//Devices exist and are stored in MIDI_DeviceController
			MIDI_Pitch *device = NULL;
			MIDI_Pitch_Node *next = NULL;
			MIDI_Pitch_Node *prev = NULL;
			
			//Millisecond timestamp when this was last assigned a note
			// uint32_t lastAssignStamp = 0;
			
			void playNote(uint8_t note);
			void stopNote();
			bool tryPlayNote(uint8_t note);
			bool tryStopNote(uint8_t note);
	};
#endif