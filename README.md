<h1>MIDI_Device_Controller</h1>
An Arduino based library for controlling various devices via the MIDI interface
Developed while making <a href='https://www.youtube.com/mrsolidsnake745'>cover videos here</a>.

[![Arduino Library](https://www.ardu-badge.com/badge/MIDI%20Device%20Controller.svg)](https://www.ardu-badge.com/MIDI%20Device%20Controller)
![GitHub Release](https://img.shields.io/github/v/release/solidsnake745/MIDI_Device_Controller)
![GitHub License](https://img.shields.io/github/license/solidsnake745/MIDI_Device_Controller?label=licence)
![GitHub issue custom search in repo](https://img.shields.io/github/issues-search/solidsnake745/MIDI_Device_Controller?query=is%3Aopen%20-label%3Aself-inflicted&label=Open%20issues%20by%20users)
<h3>Supported devices (instruments)</h3>
<ul>
	<li>Most floppy drives</li>
	<li>Stepper motors</li>
	<li>Hard drives</li>
	<li>Piezo buzzers</li>
	<li>Solenoids</li>
	<li>Servos</li>
	<li>More with adaptation</li>
</ul>
<h3>Supported boards</h3>
<ul>
	<li>Teensy 3.2 and above</li>
	<li>ESP32</li>
	<li><s>*Arduino AVR boards (Uno/Nano/Mega/Leonardo/etc.)</s> Currently broken TODO: fix it</li>
</ul>
<sub><s>*Does work, but performance is very poor for versions 2.0 and above.</s> Version 1.0 works well, but is missing features and has different syntax that is not documented.</sub><br>

Should work on other Arduino based boards, but may need slight adaption. TODO: define how to create issues for adding support
<h3>Supported IOs</h3>
<ul>
	<li>Direct pins (digitalWrite)</li>
	<li>Shift registers (74HC595, etc.)</li>
	<li>TODO: various IO expanders</li>
</ul>
<h3>Global objects added to Arduino sketch</h3>
<ul>
	<li>MDC - MIDI Device Controller - For controlling and operating pitch and pulse devices</li>
	<li>IOF - I/O Factory - For creating and managing I/O devices</li>
	<li>MCF - MIDI Collection Factory - For creating device collection objects</li>
	<li>MCC - MIDI Collection Controller - For controlling and operating collections of devices</li>
	<li>MPC - MIDI Pulse Controller - For mapping pulse devices to MIDI notes</li>
</ul>
<h3>Device classes</h3>
<ul>
	<li>Pitch
		<ul>
			<li>MIDI_SquareWave</li>
			<li>MIDI_SquareWave_Direction</li>
			<li>MIDI_Unipolar</li>
		</ul>
	</li>
	<li>Pulse
		<ul>
			<li>MIDI_Pulse</li>
			<li>MIDI_Servo</li>
			<li>MIDI_SoftPWM</li>
			<li>MIDI_Toggle</li>
		</ul>
	</li>	
</ul>

TODO: (2/14/26): Document how to use