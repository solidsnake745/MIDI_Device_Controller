#ifndef Base_MIDI_Pitch_h
	#define Base_MIDI_Pitch_h

	#include "../Common/PitchBend.h"
	#include "../Common/NoteDuration.h"
	#include "../Common/MIDI_Periods.h"
	#include "../Common/SinWave.h"
	#include "../IO_Devices/IO_Device.h"
	#include "../IO_Factory/IO_Factory.h"
	#include "../Settings.h"
	#include "../Common/SerialDebug.h"
	
	//Forward declaration for compiling
	class Base_MIDI_Pitch_Direction;

	///MIDI device class for anything needing a pitch signal (FDD, HDD, Stepper motors, etc.)
	class Base_MIDI_Pitch
	{	
		//Give access to all private members to the below classes
		friend class MIDI_Device_Controller;		
		friend class MIDI_Pitch_Node;
		friend class Base_MIDI_Pitch_Collection;
		friend class Base_MIDI_Pitch_Direction;
		
		protected:
			enum Effect
			{
				None = 0,
				PitchBend = 1,
				Vibrato = 2
			};
			
			inline static SerialDebug _debug = SerialDebug(DEBUG_MIDIPITCH);			
			inline void callUpdateOutputs(IO_Device* io) { io->updateOutputs(); };
			inline virtual bool hasDirection() { return false; };
			
		public:
			inline virtual Base_MIDI_Pitch_Direction* asDir() { return nullptr; };
		
		//Constructors
		//_____________________________________________________________________________________________
		public:
			inline Base_MIDI_Pitch() {};
			virtual ~Base_MIDI_Pitch() {};
		
		//Configuration
		//_____________________________________________________________________________________________
		protected:
			uint8_t _id;	
			MIDI_Device_Controller* _parent = nullptr;
			void mdcStartPlaying();
			void mdcStopPlaying();
			bool mdcIsPlayingNotes();
			void mdcNoteAssigned();
			
		public:
			//Print out this device's configuration
			inline virtual void printStatus()
			{
				_debug.println(F("Status for device %d"), _id);
				_debug.println(F("  Is Enabled: %d"), isEnabled());
			};
			
			inline uint8_t getID() { return _id; };
			
			//Indicates whether a device is available for note assignment
			inline bool isAvailable() { return _currentNote == -1; };

		//Operation
		//_____________________________________________________________________________________________
		protected:
			//Calculated microperiods based on the set resolution for each note
			inline static uint16_t* _referencePeriods = MIDI_Periods::calculatedPeriods;
			
			//TODO: volatile modifier may not be necessary or useful, have to research further
			//Current assigned note where -1 indicates available and 0 indicates awaiting reset
			volatile int16_t _currentNote = -1;

			//Current period that should be playing
			volatile uint32_t _currentPeriod = 0;
			
			//Current tick value
			volatile uint32_t _currentTick = 0;

			//How long the device has been playing
			NoteDuration _currentDuration;
			
			//The last object to assign a note to this device
			void* _lastAssignedBy = nullptr;
			
			//Current effect being applied (pitchbend/vibrato currently)
			Effect _currentEffect = None;
			
			//Vibrato related properties
			//Used to track when to execute the next vibrato event
			uint16_t _vibratoTick = 0;
			
			//Used to track the degree of vibrato to apply next
			uint16_t _vibratoDegree = 0;
			
			//Amount of vibrato to apply in units of pitchbend (0 to 8192)
			uint16_t _vibratoAmount = 0;
			
			//Frequency of vibrato changes in microseconds (change occurs every _vibratoRate microseconds)
			uint16_t _vibratoRate = 0;
			
			//Controls whether vibrato is automatically applied after _vibratoDelay passes
			bool _vibratoAutoStart = false;
			
			//How much to delay before applying vibrato
			NoteDuration _vibratoDelay;
			
			//Resets a device's operational properties
			inline virtual void resetProperties(bool includeExtra = false)
			{
				_currentNote = -1;
				_currentPeriod = 0;
				_currentTick = 0;
				_currentDuration.reset();
				_currentEffect = None;
				_vibratoTick = 0;
				_vibratoDegree = 0;
				
				//if(!includeExtra) return;
				//Reset extra properties here
			};
			
			//Gets the base period of the note currently being played
			inline virtual uint32_t getBasePeriod() { return *(_referencePeriods + _currentNote); };
			
			//Gets the period currently being played
			inline uint32_t getCurrentPeriod() { return _currentPeriod; };
			
			//Executes before note playing begins
			inline virtual void startPlaying() {};
			
			//Executes after note playing stops
			inline virtual void stopPlaying() {};
			
			//Operates the devices output for one cycle/step (when current period has elapsed)
			virtual void cycleOutput() = 0;
			
			//Operates device per desired MIDI output
			//Operates device per desired MIDI output
			inline void processNotes()
			{
				if(_currentNote < 0)
				{
					_debug.debugln(20, F("%d - No note"), _id);
					return;
				}
				
				if(_currentNote == 0) //Reset pending
				{			
					_debug.debugln(20, F("%d - Resetting properties"), _id);
					resetProperties();
					return;
				}
				
				_currentTick++;
				if(_currentTick >= _currentPeriod) //Update pending
				{
					_debug.debugln(20, F("%d - Cycling output"), _id);
					cycleOutput();
					_currentTick = 0; //Reset counter
				}
				
				_currentDuration.addMicros(MIDI_Periods::getResolution());
				
				if(_currentEffect == Vibrato)
					_vibratoTick += MIDI_Periods::getResolution();
			};
			
			//Private methods for playing notes used by collections to indicate assignedBy
			inline void playNote(uint8_t note, void* assignedBy)
			{
				if(!isEnabled()) 
				{
					_debug.debugln(7, F("%d - Not enabled"), _id);
					return;
				}
				
				_currentNote = note;
				_currentPeriod = getBasePeriod();
				_lastAssignedBy = assignedBy;
				
				_debug.debugln(7, F("%d - Note %d (%d) assigned"), _id, _currentNote, _currentPeriod);
				
				mdcNoteAssigned();
			};
			
			inline void playPeriod(uint32_t period, void* assignedBy)
			{
				if(!isEnabled()) 
				{
					_debug.debugln(7, F("%d - Not enabled"), _id);
					return;
				}
				
				_currentNote = 255;	
				_currentPeriod = period;
				_lastAssignedBy = assignedBy;
				
				_debug.debugln(7, F("%d - Period %d assigned"), _id, _currentPeriod);

				mdcNoteAssigned();
			};
			
			inline bool shouldAutoStartVibrato()
			{				
				if(!_vibratoAutoStart || _currentEffect == Vibrato)
					return false; //Not set to use auto start or already processing vibrato
				
				if(_currentDuration <= _vibratoDelay)
					return false; //Vibrato delay has not yet been reached
				
				return true;
			};
			
			inline void processVibrato()
			{
				if(_currentEffect != Vibrato)
					return; //Vibrato not being applied
				
				if(_vibratoTick < _vibratoRate)
					return; //Not yet time to increment
				
				if(_vibratoDegree > 359)
					_vibratoDegree = 0;
				
				int16_t b = _vibratoAmount * SinWave::getSinValue(_vibratoDegree++);
				_currentPeriod = getBasePeriod() / PitchBend::calculateFactor(b);
				_vibratoTick = 0;
			}
			
		public:
			inline virtual bool isEnabled() = 0;
			inline int8_t getCurrentNote() { return _currentNote; };			
			inline void playNote(uint8_t note) { playNote(note, nullptr); };
			inline void playPeriod(uint32_t period) { playPeriod(period, nullptr); };
			
			inline void bendNote(int16_t bend, bool shiftRange = false)
			{
				if (!(_currentNote > 0 && _currentNote < 256))
					return;
				
				//If an effect other than pitchbend is in place, don't continue
				if(_currentEffect != None && _currentEffect != PitchBend)
					return;
				
				//Set the current effect so it's not interfered with
				//Or clear it if the bend is 0 (bend back to original note)
				_currentEffect = bend == 0 ? None : PitchBend;
				
				_debug.debugln(20, F("%d - Bending by value: %d"), _id, bend);
				
				float pitchFactor = PitchBend::calculateFactor(bend, shiftRange);
				_debug.debugln(20, F("%d - Factor: %f"), _id, pitchFactor);
				
				uint16_t basePeriod = getBasePeriod();
				_debug.debugln(20, F("%d - Base: %d"), _id, basePeriod);
				
				uint16_t newPeriod = basePeriod / pitchFactor;
				_debug.debugln(20, F("%d - New Period: %d"), _id, newPeriod);

				_currentPeriod = newPeriod;
			};
			
			inline void bendNoteByFactor(float pitchFactor)
			{
				if (!(_currentNote > 0 && _currentNote < 256))
					return;
				
				//If an effect other than pitchbend is in place, don't continue
				if(_currentEffect != None && _currentEffect != PitchBend)
					return;
				
				//Set the current effect so it's not interfered with
				//Or clear it if the factor is 1 (bend back to original note)
				_currentEffect = pitchFactor == 1.0 ? None : PitchBend;
				
				_debug.debugln(20, F("%d - Bending by factor: %f"), _id, pitchFactor);
				
				uint16_t basePeriod = getBasePeriod();
				_debug.debugln(20, F("%d - Base: %d"), _id, basePeriod);
				
				uint16_t newPeriod = basePeriod / pitchFactor;
				_debug.debugln(20, F("%d - New Period: %d"), _id, newPeriod);

				_currentPeriod = newPeriod;
			};
			
			inline void stopNote()
			{
				_debug.debugln(7, F("%d - Clearing note"), _id);
				_currentNote = 0;
			};
			
			//Sets vibrato properties: amount (of pitchbend, 0 to 8191) and rate (delay between updates in microseconds)
			inline void setVibrato(uint16_t amount, uint16_t rate) { _vibratoAmount = amount; _vibratoRate = rate; };
			
			//Sets auto start for vibrato where true enables it
			inline void setVibratoAutoStart(bool value) { _vibratoAutoStart = value; };
			
			//Sets a delay of when to apply vibrato after a note starts playing
			inline void setVibratoDelay(uint32_t us) { _vibratoDelay.reset(); _vibratoDelay.addMicros(us); }
			
			//Starts vibrato manually if setup, a note is playing, and another effect is not already in place
			inline bool startVibrato() 
			{
				if(_vibratoAmount == 0 || _vibratoRate == 0)
					return false; //Vibrato not completely setup
				
				if(_currentEffect != None)
					return false; //Some other effect in place
				
				if (!(_currentNote > 0 && _currentNote < 256))
					return false; //No note assigned
				
				_currentEffect = Vibrato;
				return true;
			};
			
			//Stops vibrato
			inline void stopVibrato()
			{ 
				if(_currentEffect != Vibrato)
					return;
				
				_currentEffect = None;
				_vibratoTick = 0;
				_vibratoDegree = 0;
			};
			
		//Testing/debug
		//_____________________________________________________________________________________________
		public:
			//Plays Do-Re-Mi scale via the interrupt process
			inline virtual void testDoReMi(uint8_t octave = 0, uint16_t noteDuration = 150, uint16_t noteGap = 50)
			{
				uint8_t noteShift = octave * 12;	
	
				_debug.debugln(7, F("%d - Starting DoReMi test"), _id);
				playNote(48 + noteShift); delay(noteDuration);
				stopNote(); delay(noteGap);
				
				playNote(50 + noteShift); delay(noteDuration);
				stopNote(); delay(noteGap);
				
				playNote(52 + noteShift); delay(noteDuration);
				stopNote(); delay(noteGap);
				
				playNote(53 + noteShift); delay(noteDuration);
				stopNote(); delay(noteGap);
				
				_debug.debugln(7, F("%d - Half point DoReMi test"), _id);
				playNote(55 + noteShift); delay(noteDuration);
				stopNote(); delay(noteGap);
				
				playNote(57 + noteShift); delay(noteDuration);
				stopNote(); delay(noteGap);
				
				playNote(59 + noteShift); delay(noteDuration);
				stopNote(); delay(noteGap);
				
				playNote(60 + noteShift); delay(noteDuration);
				stopNote(); delay(noteGap);
				_debug.debugln(7, F("%d - Finished DoReMi test"), _id);
			};
	};
	
#endif