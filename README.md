# digishow
DigiShow is an easy-to-use software aims at controls for live performances and interactive shows with music, lights, displays, machines, robots and more digital things. With using DigiShow LINK app, a DJ can arrange cue actions in Ableton Live and control automations on the stage. A drummer can play drums with dynamic show lightings. A geek musician can make own DIY instruments played for highlights of the performance. Also, an artist can create new art works with more interactive media elements.

DigiShow LINK app enables signal transferring between MIDI, DMX, Modbus, Arduino, Philips Hue and more digital device interfaces.

- MIDI interface is typically for connecting your digital musical instruments, controllers, sequencers and digital music apps.
- DMX interface is typically for connecting your lights and fixtures on the stage. 
- Modbus interface is typically for connecting your industrial automations, robots and machines.
- Arduino is an open source electronic controller for making homebrew instruments, lights or gadgets for your interactive shows.
- Philips Hue is the popular smart home wireless-controlled lights.
- DigiShow can also control presenting videos, pictures and web contents on multiple screens dynamically.

DigiShow makes all things digital work together for your awesome show time, enjoys !!

## Executable app downloads
- DigiShow LINK for macOS (64bit)
http://robinz.org/labs/downloads/digishow_mac_20200907.zip
- DigiShow LINK for windows (64bit)
http://robinz.org/labs/downloads/digishow_win_20200912.zip

## Extra downloads and resources
- [ MIDI ] virtual MIDI bus driver [learn](https://help.ableton.com/hc/en-us/articles/209774225-How-to-setup-a-virtual-MIDI-bus) 
- [ MIDI ] virtual MIDI network driver [learn](https://help.ableton.com/hc/en-us/articles/209071169-How-to-setup-a-virtual-MIDI-network) 
- [ MIDI ] MIDI signal monitor [learn](https://support.native-instruments.com/hc/en-us/articles/209544729-How-to-Monitor-the-Input-of-a-MIDI-Controller)
- [ DMX ] Enttec Open DMX USB driver (FTDI VCP drivers) [download](https://www.ftdichip.com/Drivers/VCP.htm)
- [ Arduino ] sketch for Arduino remote IO controls [download](http://robinz.org/labs/downloads/rioc_arduino_20200909.zip)

## Library dependencies
Please use qmake tool or QtCreator app to build executables from the source code, and these libraries are required.
- Qt 5.12 or higher http://www.qt.io
- RtMidi 4.0.0 http://www.music.mcgill.ca/~gary/rtmidi/

## Target operating systems
The source code can be compiled for platforms compatible with:
- macOS 10.13 or higher
- windows 7, windows 10 or higher ( 64-bit version recommended )
- linux ( see Qt and RtMidi websites for compatibility details )
