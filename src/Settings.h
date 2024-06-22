#ifndef Settings_h
	#define Settings_h	
	
	#define MIN_RESOLUTION 1
	#define MAX_RESOLUTION 200
	#define DEFAULT_RESOLUTION 100
	
	//Max limit of devices that can be managed by the MIDI_DeviceController
	//Number of devices correlates to more/less memory used by this library
	#define MAX_PITCH_DEVICES 8

	//Max limit of device collections that can be managed by the DeviceChain_Controller
	//Number of collections correlates to more/less memory used by this library
	#define MAX_COLLECTIONS 8
	
	//Amount of time in seconds before stopping processing if using auto processing
	#define IDLE_TIMEOUT_DEFAULT 15

	//Longest any one device can be playing (must be in microseconds)
	#define MAX_DURATION_DEFAULT 10 * 1000000
	
	//Pitchbend settings (Would advise not changing)
	#define BEND_SEMITONES 2
	#define SEMITONES_PER_OCTAVE 12
		
	//Setting this to 1 includes various test methods for verifying logic
	//Tests output information to serial and rely on SerialDebug debug methods being enabled
	//Below are some examples of calling those tests
	#define INCLUDE_TESTS 0
	//NoteDuration::runTest();
	//MIDI_Message::runTest();
	//ByteNoteRegister::runTest();
	//PitchBend::runTest();
	//SerialDebug::runTest();
	
	//Debugging areas and levels
	#define DEBUG_DEVICECONTROLLER 0
	#define DEBUG_MIDIPITCH 0
	#define DEBUG_MIDIPERIODS 0	
	
	//The SerialDebug instance for this is shared with derived classes so you'll get any logging from the derived classes
	//No additional logging currently exists in derived classes, but just FYI future me
	#define DEBUG_BASEPITCHCOLLECTION 0
	
	#define DEBUG_PITCHNODE 0
	#define DEBUG_COLLECTIONCONTROLLER 0	
	#define DEBUG_COLLECTION_FACTORY 0
	
	#define DEBUG_DIGITALIO 0
	#define DEBUG_74HC595 0
	#define DEBUG_PULSECONTROLLER 0
	
	#define DEBUG_BYTENOTEREGISTER 0
#endif