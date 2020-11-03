# DigiShow LINK
DigiShow is an easy-to-use software aims at controls for live performances and interactive shows with music, lights, displays, machines, robots and more digital things. With using DigiShow LINK app, a DJ can arrange cue actions in Ableton Live and control automations on the stage. A drummer can play beats with dynamic show lightings. A geek musician can make own DIY instruments played for highlights of the performance. Also, an artist can create new art works with more interactive media elements.

DigiShow LINK app enables signal transferring between MIDI, DMX, Modbus, Arduino, Philips Hue and more digital device interfaces.

- MIDI interface is typically for connecting your digital musical instruments, controllers, sequencers and digital music apps.
- DMX interface is typically for connecting your lights and fixtures on the stage. 
- Modbus interface is typically for connecting your industrial automations, robots and machines.
- Arduino is an open source electronic controller for making homebrew instruments, lights or gadgets for your interactive shows.
- Philips Hue is the popular smart home wireless-controlled lights.
- DigiShow can also control presenting videos, pictures and web contents on multiple screens dynamically.

DigiShow makes all digital things work together for your awesome show time, enjoys !!

## Official release downloads
Please visit http://robinz.org/labs/digishow to download the latest releases:
- DigiShow LINK for macOS (64bit)
- DigiShow LINK for windows (64bit)

## Install and run DigiShow LINK apps
macOS:
Unzip the downloaded file, copy app "DigiShow" to your Applications folder and run it.

Windows:
Unzip the downloaded file, copy folder "DigiShow LINK" to your disk and run "DigiShow.exe" in the folder.

## Extra downloads and resources
[ Arduino ] sketch for Arduino remote IO controls
Required to enable DigiShow LINK to configure and control IO on your Arduino units remotely.
[download](https://github.com/robinz-labs/rioc-arduino/releases)

[ DMX ] ENTTEC DMX USB Pro driver (FTDI VCP drivers) 
Required to enable DigiShow LINK to control DMX lightings through an ENTTEC adapter.
[download](https://www.ftdichip.com/Drivers/VCP.htm)

[ MIDI ] virtual MIDI bus driver 
Required to enable DigiShow LINK to communicate with other MIDI apps running on a same computer.
[learn](https://help.ableton.com/hc/en-us/articles/209774225-How-to-setup-a-virtual-MIDI-bus) 
 
[ MIDI ] virtual MIDI network driver 
Required to enable DigiShow LINK to communicate with other MIDI apps running on different computers.
[learn](https://help.ableton.com/hc/en-us/articles/209071169-How-to-setup-a-virtual-MIDI-network) 

[ MIDI ] MIDI signal monitor 
A utility that is helpful to understand MIDI Note, CC signals transferring between your MIDI devices and apps.  
[learn](https://support.native-instruments.com/hc/en-us/articles/209544729-How-to-Monitor-the-Input-of-a-MIDI-Controller)
 
## Developer resources
DigiShow is open-source. If you would like to rebuild this software using the source code we contributed, please visit https://github.com/robinz-labs/ .

Please use qmake tool or QtCreator app to build executables from the source code, and confirm these library dependencies are required:
- Qt 5.12 or higher http://www.qt.io
- RtMidi 4.0.0 http://www.music.mcgill.ca/~gary/rtmidi/

The source code can be compiled for target platforms compatible with:
- macOS 10.13 or higher
- windows 7, windows 10 or higher ( 64-bit version recommended )
- linux ( see Qt and RtMidi websites for compatibility details )
