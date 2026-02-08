#ifndef PitchNode_h
	#define PitchNode_h

	#include <Arduino.h>
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"	
	#include "../MIDI_Pitch/Base_MIDI_Pitch.h"
	
	//Forward declaration for compiling
	//class Base_MIDI_Pitch;
	class Base_MIDI_Pitch_Collection;

	/// @private
	struct MIDI_Pitch_Node
	{
		//Give Base_MIDI_Pitch_Collection access to all private members
		friend class Base_MIDI_Pitch_Collection;
		
		inline static SerialDebug _debug = SerialDebug(DEBUG_PITCHNODE);
		
		private:
			inline MIDI_Pitch_Node(Base_MIDI_Pitch* d, Base_MIDI_Pitch_Collection *dc)
			{
				_device = d;
				_parent = dc;
			};
			
			Base_MIDI_Pitch_Collection* _parent = NULL;		
			
		public:
			//Devices exist and are stored in MIDI_DeviceController
			Base_MIDI_Pitch* _device = NULL;
			MIDI_Pitch_Node* next = NULL;
			MIDI_Pitch_Node* prev = NULL;
			
			//Millisecond timestamp when this was last assigned a note
			// uint32_t lastAssignStamp = 0;
			
			inline void playNote(uint8_t note) { _device->playNote(note, _parent); };
			inline void bendNoteByFactor(float pitchFactor)
			{
				if(_device->_lastAssignedBy == _parent)
					_device->bendNoteByFactor(pitchFactor);
			};
			
			inline void stopNote() 
			{
				if(_device->_lastAssignedBy == _parent)
					_device->stopNote();
			};
			
			inline bool tryPlayNote(uint8_t note)
			{
				if(_device->isAvailable())
				{
					_device->playNote(note, _parent);
					return true;
				}
				
				return false;
			};
			
			inline bool tryStopNote(uint8_t note)
			{
				if(_device->getCurrentNote() == note && _device->_lastAssignedBy == _parent)
				{
					_device->stopNote();
					return true;
				}
				
				return false;
			};
	};
	
#endif