#ifndef Direct_Collection_h
	#define Direct_Collection_h
	
	#include "../Base_MIDI_Pitch_Collection.h"

	///Assigns notes to all available devices in the collection
	
	/// @brief First available assignment to all devices in the collection
	/// @details Assigns new notes to the first available device in the collection.<br>
	/// If no devices are available, does not assign.<br>
	/// Searches in the order devices were added.
	class Direct_Collection : public Base_MIDI_Pitch_Collection
	{	
		public:		
			/// @brief Does nothing for this collection type
			inline void reset() override { Base_MIDI_Pitch_Collection::reset(); }; //Exists for Doxygen documentation
			bool playNote(uint8_t note) override;
			void stopNote(uint8_t note) override;
	};
	
#endif