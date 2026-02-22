#ifndef PitchNode_h
	#define PitchNode_h

	#include <Arduino.h>
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"	
	#include "../MIDI_Pitch/Base_MIDI_Pitch.h"
	
	//Forward declaration for compiling
	class Base_MIDI_Pitch_Collection;

	/// @brief Class to represent a node in our doubly linked list for devices (Base_MIDI_Pitch_Collection)
	/// @details Used in implementations of Base_MIDI_Pitch_Collection to store devices and simplify device note management for them.
	struct MIDI_Pitch_Node
	{
		//Give Base_MIDI_Pitch_Collection access to all private members
		friend class Base_MIDI_Pitch_Collection;
		
		/// @brief Gets the next connected node if it is set
		/// @retval nullptr if not set
		MIDI_Pitch_Node* const getNextNode();
		
		/// @brief Gets the previous connected node if it is set
		/// @retval nullptr if not set
		MIDI_Pitch_Node* const getPrevNode();
		
		/// @brief Plays the specified note
		/// @details Also sets the parent collection of this node as the device's last note assigner.
		///	This helps prevent devices from being acted on by the wrong collection after initial assignment.
		/// @param note MIDI note to play
		void playNote(uint8_t note);
		
		/// @brief Bends the current note by the specified factor
		/// @param pitchFactor Factor to bend the note by
		void bendNoteByFactor(float pitchFactor);
		
		/// @brief Stops the current note if applicable
		/// @details Only stops if the device's last note assigner is the parent collection of this node.
		void stopNote();
		
		/// @brief Attempts to play the specified note on the device in this node
		/// @details Only plays the note if the device is available (see Base_MIDI_Pitch::isAvailable()).
		/// @param note MIDI note to play
		/// @retval true if successfully assigned
		bool tryPlayNote(uint8_t note);
		
		/// @brief Attempts to stop the specified note on the device in this node
		/// @details Only stops the note if two conditions are met:
		/// 1. The specified note matches the note currently being played by the device.
		/// 2. The device's last note assigner is the parent collection of this node.
		/// @param note MIDI note to stop
		/// @retval true if successfully stopped
		bool tryStopNote(uint8_t note);
		
		private:
			inline static SerialDebug _debug = SerialDebug(DEBUG_PITCHNODE);
			
			MIDI_Pitch_Node(Base_MIDI_Pitch* d, Base_MIDI_Pitch_Collection *dc);
			
			Base_MIDI_Pitch_Collection* _parent = nullptr;			
			Base_MIDI_Pitch* _device = nullptr;
			MIDI_Pitch_Node* next = nullptr;
			MIDI_Pitch_Node* prev = nullptr;
	};
	
#endif