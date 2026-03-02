/// @file MIDI_Handler.h
/// @brief An optional header that defines the MIDI_Handler class
/// @details To include from the sketch level, use: @code #include <MIDI_Handler\MIDI_Handler.h> @endcode
/// Note: This also includes MIDI_Message.h.

#ifndef MIDI_Handler_h
	#define MIDI_Handler_h
	
	#include "MIDI_Message.h"	
	
	//MIDI handler method aliases
	
	/// @brief Note Off handler definition
	using NoteOffHandle = void(*)(uint8_t, uint8_t, uint8_t);
	
	/// @brief Note On handler definition
	using NoteOnHandle = void(*)(uint8_t, uint8_t, uint8_t);
	
	/// @brief Poly Pressure handler definition
	using PolyPressureHandle = void(*)(uint8_t, uint8_t, uint8_t);
	
	/// @brief Control Change handler definition
	using ControlChangeHandle = void(*)(uint8_t, uint8_t, uint8_t);
	
	/// @brief Program Change handler definition
	using ProgramChangeHandle = void(*)(uint8_t, uint8_t, uint8_t);
	
	/// @brief Channel Pressure handler definition
	using ChannelPressureHandle = void(*)(uint8_t, uint8_t, uint8_t);
	
	/// @brief Pitch Bend handler definition
	using PitchBendHandle = void(*)(uint8_t, int16_t);
	
	/// @brief A class to represent a MIDI message handler
	/// @details Used to simplify calling associated methods for handling MIDI events.<br>
	/// See example sketch for Hairless MIDI to see example usage.
	class MIDI_Handler
	{
		//Pointers to handlers, defaults to null to indicate no handler set
		NoteOffHandle _noteOff = nullptr;
		NoteOnHandle _noteOn = nullptr;  
		PolyPressureHandle _polyPressure = nullptr;  
		ControlChangeHandle _controlChange = nullptr;
		ProgramChangeHandle _programChange = nullptr;
		ChannelPressureHandle _channelPressure = nullptr;
		PitchBendHandle _pitchBend = nullptr;
		
		public:
			/// @brief Central method for handling MIDI messages
			/// @details Used to call the corresponding handler based on MIDI message type.<br>
			/// Be sure to have set relevant handlers before using this method.
			void handleMIDI(MIDI_Message msg);
			
			//Methods for setting handlers
			
			/// @brief Sets the handler for Note Off events
			inline void setNoteOffHandle(NoteOffHandle handle) { _noteOff = handle; };
			
			/// @brief Sets the handler for Note On events
			inline void setNoteOnHandle(NoteOnHandle handle) { _noteOn = handle; };
			
			/// @brief Sets the handler for Poly Pressure events
			inline void setPolyPressureHandle(PolyPressureHandle handle) { _polyPressure = handle; };
			
			/// @brief Sets the handler for Control Change events
			inline void setControlChangeHandle(ControlChangeHandle handle) { _controlChange = handle; };
			
			/// @brief Sets the handler for Program Change events
			inline void setProgramChangeHandle(ProgramChangeHandle handle) { _programChange = handle; };
			
			/// @brief Sets the handler for Channel Pressure events
			inline void setChannelPressureHandle(ChannelPressureHandle handle) { _channelPressure = handle; };
			
			/// @brief Sets the handler for Pitch Bend events
			inline void setPitchBendHandle(PitchBendHandle handle) { _pitchBend = handle; };
			
			//Direct access to calling handler methods if needed
			//Abbreviations:
			//ch = channel, n = note, v = velocity, p = pressure, cn = controller number,
			//d = data, pn = program number, u = unused, b = bend
			
			/// @brief Calls the handler for Note Off events if it's set
			/// @param ch Channel (0-16)
			/// @param n Note (0-127)
			/// @param v Velocity (0-127)
			inline void noteOff(uint8_t ch, uint8_t n, uint8_t v) { if(_noteOff)  _noteOff(ch, n, v); };
			
			/// @brief Calls the handler for Note On events if it's set
			/// @param ch Channel (0-16)
			/// @param n Note (0-127)
			/// @param v Velocity (0-127)
			inline void noteOn(uint8_t ch, uint8_t n, uint8_t v) { if(_noteOn)  _noteOn(ch, n, v); };
			
			/// @brief Calls the handler for Poly Pressure events if it's set
			/// @param ch Channel (0-16)
			/// @param n Note (0-127)
			/// @param p Pressure (0-127)
			inline void polyPressure(uint8_t ch, uint8_t n, uint8_t p) { if(_polyPressure)  _polyPressure(ch, n, p); };
			
			/// @brief Calls the handler for Control Change events if it's set
			/// @param ch Channel (0-16)
			/// @param cn Controller Number (0-127?)
			/// @param d Data (0-127)
			inline void controlChange(uint8_t ch, uint8_t cn, uint8_t d) { if(_controlChange)  _controlChange(ch, cn, d); };
			
			/// @brief Calls the handler for Program Change events if it's set
			/// @param ch Channel (0-16)
			/// @param pn Program Number (0-127?)
			/// @param u Unused
			inline void programChange(uint8_t ch, uint8_t pn, uint8_t u) { if(_programChange)  _programChange(ch, pn, u); };
			
			/// @brief Calls the handler for Channel Pressure events if it's set
			/// @param ch Channel (0-16)
			/// @param p Pressure (0-127)
			/// @param u Unused
			inline void channelPressure(uint8_t ch, uint8_t p, uint8_t u) { if(_channelPressure)  _channelPressure(ch, p, u); };
			
			/// @brief Calls the handler for Pitch Bend events if it's set
			/// @param ch Channel (0-16)
			/// @param b Pitch Bend value (0-16838) OR (-8192-8191)
			inline void pitchBend(uint8_t ch, int16_t b) { if(_pitchBend)  _pitchBend(ch, b); };
	};

#endif