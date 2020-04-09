#ifndef Settings_h
	#define Settings_h
	
	#define MIN_RESOLUTION 1
	#define MAX_RESOLUTION 200
	#define DEFAULT_RESOLUTION 100
	
	//Max limit of device collections that can be managed by the DeviceChain_Controller
	//Number of collections correlates to more/less memory used by this library
	#define MAX_COLLECTIONS 8

	//Max limit of devices that can be managed by the MIDI_DeviceController
	//Number of devices correlates to more/less memory used by this library
	#define MAX_DEVICES 8

	//Amount of time in seconds before stopping processing if using auto processing
	#define IDLE_TIMEOUT_DEFAULT 15

	//Longest any one device can be playing (must be in microseconds)
	#define MAX_DURATION_DEFAULT 10 * 1000000
	
	//Debugging areas and levels
	#define DEBUG_MIDIDEVICE_CONTROLLER 0
	#define DEBUG_DEVICE 0
	#define DEBUG_MIDI_PERIODS 0
	
	#define DEBUG_DEVICECHAIN_CONTROLLER 0
	#define DEBUG_DEVICENODE 0
	#define DEBUG_DEVICECHAIN_BASE 0
	//#define DEBUG_DEVICECHAIN_DIRECT 0
	//#define DEBUG_DEVICECHAIN_FA 0
	//#define DEBUG_DEVICECHAIN_RR 0
	
	#define DEBUG_DEVICECOLLECTION_FACTORY 5
	
	#define DEBUG_MIDISERIAL 0
#endif