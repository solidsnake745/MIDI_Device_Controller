#ifndef RoundRobin_Collection_h
	#define RoundRobin_Collection_h
	
	#include "../Base_MIDI_Pitch_Collection.h"
	
	/// @brief Round robin assignment to all devices in the collection
	/// @details Assigns new notes to the next available device in the collection.<br>
	/// If no devices are available, does not assign.<br>
	/// Distributes in the order devices were added.
	class RoundRobin_Collection : public Base_MIDI_Pitch_Collection
	{	
		MIDI_Pitch_Node* lastAssign = nullptr;
			
		public:
			/// @brief Clears the last node assigned to restart distribution from the beginning
			void reset() override;
			bool playNote(uint8_t note) override;
			void stopNote(uint8_t note) override;
	};
	
#endif