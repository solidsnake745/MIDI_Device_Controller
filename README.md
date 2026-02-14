<h1>MIDI_Device_Controller</h1>

An Arduino based library for controlling various devices via the MIDI interface
Developed while making <a href='https://www.youtube.com/mrsolidsnake745'>cover videos here</a>

![GitHub Release](https://img.shields.io/github/v/release/solidsnake745/MIDI_Device_Controller)
![GitHub License](https://img.shields.io/github/license/solidsnake745/MIDI_Device_Controller?label=licence)
![GitHub issue custom search in repo](https://img.shields.io/github/issues-search/solidsnake745/MIDI_Device_Controller?query=is%3Aopen%20-label%3Aself-inflicted&label=Open%20issues%20by%20users)

<h3>Supported devices (instruments)</h3>
 - Most floppy drives
 - Stepper motors
 - Hard drives
 - Piezo buzzers
 - Solenoids
 - Servos
 - More with adaptation

<h3>Supported boards</h3>
Tested on:
 - Teensy 3.0 and above
 - ESP32
 - *Arduino AVR boards (Uno/Nano/Mega/Leonardo/etc.)

<small>*Does work, but performance is very poor for versions 2.0 and above. Version 1.0 works well, but is missing features and has different syntax that is not documented.</small>

Should work on other Arduino based boards, but may need slight adaption.

<h3>Supported IOs</h3>
 - Direct pins (digitalWrite)
 - Shift registers (74HC595, etc.)
 - (TODO) various IO expanders

<h3>Global objects added to Arduino sketch</h3>
 - MDC - MIDI Device Controller - For controlling and operating pitch and pulse devices
 - IOF - I/O Factory - For creating and managing I/O devices
 - MCF - MIDI Collection Factory - For creating device collection objects
 - MCC - MIDI Collection Controller - For controlling and operating collections of devices
 - MPC - MIDI Pulse Controller - For mapping pulse devices to MIDI notes

<h3>Device classes</h3>
Pitch
 - MIDI_SquareWave
 - MIDI_SquareWave_Direction
 - MIDI_Unipolar
 
Pulse
 - MIDI_Pulse
 - MIDI_Servo
 - MIDI_SoftPWM
 - MIDI_Toggle
 
TODO (2/14/26): Document how to use