#ifndef FirstAvailable_Collection_h
	#define FirstAvailable_Collection_h

	#include "../Base_MIDI_Pitch_Collection.h"

	/// @brief Direct assignment to all devices in the collection
	/// @details Assigns new notes to all devices in the collection.<br>
	/// Collection essentially acts like a single device.
	class FirstAvailable_Collection : public Base_MIDI_Pitch_Collection
	{
		public:
			/// @brief Does nothing for this collection type
			inline void reset() override { Base_MIDI_Pitch_Collection::reset(); }; //Exists for Doxygen documentation
			bool playNote(uint8_t note) override;
			void stopNote(uint8_t note) override;
	};
	
#endif