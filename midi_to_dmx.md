# DigiShow for Beginners: Mapping MIDI Notes to DMX Lighting Channels

1. Let's get started with DigiShow. Connect a MIDI keyboard and an ENTTEC DMX USB Pro adapter to the USB ports of your computer. ENTTEC adapter is used to connect DMX lights. 

2. Open the DigiShow LINK app, and click 'Interface Manager' button in the upper right corner of the window. 

![interface_manager_button](images/screenshot_interface_manager_button.png)

Interface Manager dialog box appears, select the MIDI tab item and click the + button to create a new interface section labeled 'MIDI 1', where select your MIDI keyboard model. 

![screenshot_interface_midi](images/screenshot_interface_midi.png)

Select the DMX tab item and click the + button to create a new interface section labeled 'DMX 1'. Close the Interface Manager dialog when finished all. 

![screenshot_interface_dmx](images/screenshot_interface_dmx.png)

3. Now let's try to create a signal link to connect your MIDI keyboard input with the DMX light output, click the + button in the upper left corner of the window, a blank link item will be added to the left list. Alongside, you need to set the input source, output destination and mapping transformation parameters for the signal link on the right. 

![screenshot_new](images/screenshot_new.png)

4. Click the 'Select Source' drop-down menu, select your MIDI keyboard in it, and set the input parameters to 'Channel 1' 'MIDI Note' 'C3', and click the Apply button. 

![screenshot_set_source_midi](images/screenshot_set_source_midi.png)

Click the 'Select Destination' drop-down menu, select DMX in it, set the output parameter to 'Channel 1', and click the Apply button. 

![screenshot_set_destination_dmx](images/screenshot_set_destination_dmx.png)

5. Click the ▶︎ button in top bar to start the signal link session. At this point, when you press C3 (Middle C) on the MIDI keyboard, the lighting changes in DMX channel 1 will be trigged synchronously. 

![screenshot_running](images/screenshot_running.png)

6. Modify the settings in 'Input-out Mapping' and 'Output Envelope' to change the effect of the mapping transformation in real time. For example, setting 'Attack' to 300ms and 'Release' to 700ms makes the lighting fade in and out. 

![screenshot_set_transformation](images/screenshot_set_transformation.png)

7. And, add more signal links in this way. 
