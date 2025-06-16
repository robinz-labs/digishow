# DigiShow for Beginners

If you are using DigiShow for the first time, after successfully installing the software on your computer, we can try the following operations to learn and understand DigiShow's features.

## 1. Configure Interfaces:
Set up control interfaces (e.g., MIDI, DMX, Arduino) for audio, lighting, and interactive devices in the Interface Manager. 

- Click the "Interface Manager" button in the top-right corner of the main window, select "Screen", click "+" to add a screen presentation interface, and set it as "Preview Window" type. Close the "Interface Manager" dialog when done.

- In the following experiment, we won't need to connect physical lighting fixtures. Instead, we'll use a screen window to simulate an RGB full-color dimming light.

## 2. Add Signal Links:
Add signal bars in the Signal Link Table and assign specific outputs to them, where adjust output values in real time using faders. 

- Click the "+" button in the top-left corner of the main window three times to create three blank signal bars in the Signal Link Table. Select the first signal bar, and in the bottom-right of the main window, set "Output Destination" to "Screen (Preview Window)" + "Backlight" + "Red", then click the "Apply" button.

- Continue selecting the second and third signal bars in the Signal Link Table and set their "Output Destination" to "Screen (Preview Window)" + "Backlight". Change the color options to "Green" and "Blue" respectively, then click the "Apply" button.

- Click the play button at the top of the main window to start our first DigiShow project. You'll see a window with a black background appear on the screen. Move it to the side for observation.

- Drag the three faders on the signal bars to change the color and brightness of the preview window. The three faders correspond to the red, green, and blue color channels respectively.

## 3. Save Scenes:
Save output values in the grouped signal bars as Presets for specific scenarios. Store timeline-based signal output curves in the Cue Player. 

- Click the grid button at the top of the main window, and a Preset Launcher panel with many buttons will appear. Right-click the "Preset 1" button and select "Create Preset" from the popup context menu.

- At this point, you'll notice a "+CUE" button appears below the fader in each signal bar. Click this button to display the timeline editing window of the Cue Player. Click on the lines in the chart to add polyline control points (right-click for Bezier curves), and drag the control points to design an output variation curve for the current signal bar. When finished, click the "Save" button and close the timeline editing window.

- Complete the output curve design for the red, green, and blue signal bars using this method, then click the "Save Preset" button. After this, clicking the "Preset 1" button will start the Cue Player to execute the RGB dynamic lighting program we just designed.

- After completing "Preset 1" design, we can continue designing "Preset 2", "Preset 3"... using the method described above. Each preset contains a different set of signal output curves, corresponding to different dynamic lighting programs. If you want the launched program content to play in a loop, click "Cue Options" in the bottom-right corner of the timeline editing window and check "Repeat".

## 4. Switch Scenes (Manual):
Activate the saved Presets and Cue Players by tapping the buttons in the Preset Launcher or a remote web page in another smart phone. 

- You can click buttons in the Preset Launcher at any time to start various pre-designed scenes.

- During performance, you might want to launch presets through your phone or tablet. Right-click any preset button, select "Remote Control" from the popup context menu, and click the "Start" button. Take out your phone and scan the QR code on the screen. Make sure your phone is using the same local network WiFi as your computer, and a page showing the same preset buttons will appear on your phone. Pressing buttons on your phone will have the same effect as on your computer.

## 5. Trigger Scenes (Automatic):
Alternatively, connects scene switching to specific sensor triggers, by selecting the sensors as inputs and Presets as outputs in the signal bars. 

- We usually use Arduino to connect various sensors, but in this experiment for beginners, we won't connect any external hardware. Instead, we'll use keyboard keys to simulate external signal triggers. Please click the "+" button in the top-left corner of the main window to create another blank signal bar in the Signal Link Table. In the top-right corner of the main window, select "Input Source" as "Hot Key" + "(none)" + "(none)" + "A", then click the "Apply" button. This way, the signal bar can receive input signals when users press the A key on the keyboard.

- Then, in the bottom-right corner of the main window, select "Output Destination" as "Preset Launcher" + "Preset 1", and click the "Apply" button. This completes a signal mapping binding on the signal bar. Pressing key "A" will trigger the launch of "Preset 1".

- Once the signal mapping between key "A" and "Preset 1" is established, you can also create more key-to-preset mappings following the same procedure. This way, pressing specific keys will trigger specific presets to launch various scenes.

## Complete
Thus, a live performance lighting console or an interactive art installation is ready. 

After completing this experiment, you can continue to explore more signal types and conversion parameters during signal mapping. For example, you can try setting the signal bar's output end to "Audio Player" and adding a synchronized sound to your scene. 

For more learning, please read  [TUTORIALS](https://github.com/robinz-labs/digishow/blob/master/guides/tutorials.md) 
