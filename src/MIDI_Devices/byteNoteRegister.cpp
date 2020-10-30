#include "byteNoteRegister.h"

byteNoteRegister::byteNoteRegister()
{

}

bool byteNoteRegister::updateDurations(uint8_t period, noteDuration maxDuration)
{
	bool outputChanged = false;

	for(int x = 0; x < 8; x++)
	{
		if(!_data.getBit(x))
			continue;

		_durations[x].addMicroseconds(period);

		if(_durations[x] < maxDuration)
			continue;

		clearOutput(x);
		outputChanged = true;
	}

	return outputChanged;
};