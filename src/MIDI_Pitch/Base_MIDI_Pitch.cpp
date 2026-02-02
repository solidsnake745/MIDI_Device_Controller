#include "Base_MIDI_Pitch.h"
#include "../MIDI_Device_Controller.h" //Need the definition of methods

void Base_MIDI_Pitch::mdcStartPlaying(){ if(_parent) _parent->startPlaying(); }
void Base_MIDI_Pitch::mdcStopPlaying(){ if(_parent) _parent->stopPlaying(); }
bool Base_MIDI_Pitch::mdcIsPlayingNotes(){ if(_parent) { return _parent->isPlayingNotes(); } return false; }
void Base_MIDI_Pitch::mdcNoteAssigned(){ if(_parent) _parent->noteAssigned(); }