#ifndef Settings_h
	#define Settings_h
	
	//Settings wrapped in ifndef allow overriding from sketch level
	//Place #define above library include, example below
	//#define DEBUG_DEVICECONTROLLER 0
	//#include <MIDI_Device_Controller.h>
	
	//LED_BUILTIN is not defined in the ESP32 code as it is in other boards (Arduino/Teensy)
	#if ARDUINO_ARCH_ESP32
	  #define LED_BUILTIN 2  //This is accurate for at least the ESP32 dev module
	#endif
	
	#define MIN_RESOLUTION 1
	#define MAX_RESOLUTION 200
	
	//Determine default resolution based on the board we're compiling for
	#if ARDUINO_ARCH_AVR
		//This library trades optimization for configurability and advanced features
		//The Uno/Nano are pretty old and vastly out performed by recent microcontrollers that are better suited for this library
		//My recommendation is to get a Teensy
		//#warning "Compiling for Uno/Nano/etc."
		#define DEFAULT_RESOLUTION 100
	#elif defined(CORE_TEENSY)
		#if ARDUINO_TEENSY32
			//#warning "Compiling for Teensy 3.2"
			#define DEFAULT_RESOLUTION 20
		#elif ARDUINO_TEENSY40 || ARDUINO_TEENSY41
			//#warning "Compiling for Teensy 4.0/4.1"
			#define DEFAULT_RESOLUTION 5
		#else
			//#warning "Compiling for some Teensy board"
			//Default to a standard value for other teensy boards
			//Can probably use a lower value (use MDC.setResolution(x))
			#define DEFAULT_RESOLUTION 40
		#endif
	#elif ARDUINO_ARCH_ESP32
		//#warning "Compiling for ESP32"
		#define DEFAULT_RESOLUTION 10
	#else
		//#warning "Compiling for some unrecognized board"
		//Catch all for anything else
		//Can probably use a lower value as recent boards are much faster than the Arduino Uno/Nano
		#define DEFAULT_RESOLUTION 100
	#endif
	
	//Max limit of pitch devices that can be managed by the MIDI_DeviceController
	//Number of devices correlates to more/less memory used by this library
	#define MAX_PITCH_DEVICES 8

	//Max limit of pulse devices that can be managed by the MIDI_DeviceController
	//Number of devices correlates to more/less memory used by this library
	#define MAX_PULSE_DEVICES 16
	
	//Max limit of device collections that can be managed by the DeviceChain_Controller
	//Number of collections correlates to more/less memory used by this library
	#define MAX_COLLECTIONS 16
	
	//Amount of time in seconds before stopping processing if using auto processing
	#define IDLE_TIMEOUT_DEFAULT 30

	//Pitchbend settings (Would advise against changing unless you have a specific need)
	//BEND_MODE Values:
	//	0: factor = pow(2.0, bend / 8192.0)
	//	1: factor = pow(2.0, ((BEND_SEMITONES/SEMITONES_PER_OCTAVE) * (bend / 8192.0)))
	#define BEND_MODE 0
	#define BEND_SEMITONES 2
	#define SEMITONES_PER_OCTAVE 12
	
	#define MIDDLE_C_NOTE 60
	
	//Controls delay (in microseconds) between output state changes during manual operations
	//I.E. The delay between toggling the output pin when calling test methods or methods like resetPosition/calibratePosition
	//Mainly aesthetic - for synchronizing/tuning the sound of these operations
	#define MANUAL_CHANGE_DELAY 5000
	
	//Setting this to 1 includes various test methods for verifying logic
	//Tests output information to serial and rely on SerialDebug debug methods being enabled
	//Below are some examples of calling those tests
	#define INCLUDE_TESTS 0
	//NoteDuration::runTest();
	//MIDI_Message::runTest();
	//ByteNoteRegister::runTest();
	//PitchBend::runTest();
	//SerialDebug::runTest();
	
	//Log levels: OFF, DEBUG, TRACE, ISR, ISR_TRACE, ALL
	//Debugging areas and levels
	#ifndef DEBUG_TIMER
		#define DEBUG_TIMER OFF
	#endif
	#ifndef DEBUG_DEVICECONTROLLER
		#define DEBUG_DEVICECONTROLLER OFF
	#endif
	#ifndef DEBUG_MIDIPITCH
		#define DEBUG_MIDIPITCH OFF
	#endif
	#ifndef DEBUG_MIDIPULSE
		#define DEBUG_MIDIPULSE OFF
	#endif
	#ifndef DEBUG_MIDIPERIODS
		#define DEBUG_MIDIPERIODS OFF
	#endif
	#ifndef DEBUG_BASEPITCHCOLLECTION
		//The SerialDebug instance for this is shared with derived classes so you'll get any logging from the derived classes
		//No additional logging currently exists in derived classes, but just FYI future me
		#define DEBUG_BASEPITCHCOLLECTION OFF
	#endif
	#ifndef DEBUG_PITCHNODE
		#define DEBUG_PITCHNODE OFF
	#endif
	#ifndef DEBUG_COLLECTIONCONTROLLER
		#define DEBUG_COLLECTIONCONTROLLER OFF
	#endif
	#ifndef DEBUG_COLLECTION_FACTORY
		#define DEBUG_COLLECTION_FACTORY OFF
	#endif
	#ifndef DEBUG_DIGITALIO
		#define DEBUG_DIGITALIO OFF
	#endif
	#ifndef DEBUG_74HC595
		#define DEBUG_74HC595 OFF
	#endif
	#ifndef DEBUG_PULSECONTROLLER
		#define DEBUG_PULSECONTROLLER OFF
	#endif
	#ifndef DEBUG_BYTENOTEREGISTER
		#define DEBUG_BYTENOTEREGISTER OFF
	#endif
	#ifndef DEBUG_PITCHBEND
		#define DEBUG_PITCHBEND OFF
	#endif
	#ifndef DEBUG_SINWAVE
		#define DEBUG_SINWAVE OFF
	#endif
	
#endif