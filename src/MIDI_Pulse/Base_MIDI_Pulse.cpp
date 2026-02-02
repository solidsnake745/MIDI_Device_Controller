#include "Base_MIDI_Pulse.h"
#include "../MIDI_Device_Controller.h" //Need the definition of noteAssigned()

void Base_MIDI_Pulse::noteAssigned(){ if(_parent) _parent->noteAssigned(); }