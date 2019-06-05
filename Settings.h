#ifndef Settings_h
	#define Settings_h
	
	//Max limit of devices that can be managed by the controller
	//Number of devices correlates to more/less memory used by this library
	#define MAX_CHAINS 16

	//Max limit of devices that can be managed by the controller
	//Number of devices correlates to more/less memory used by this library
	#define MAX_DEVICES 8

	//Amount of time in seconds before stopping processing if using auto processing
	#define IDLE_TIMEOUT_DEFAULT 15

	//Longest any one device can be playing (must be in microseconds)
	#define MAX_DURATION_DEFAULT 10 * 1000000
#endif