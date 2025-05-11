Using AI to Control Lights in DigiShow

This project demonstrates how to use AI programming tools (such as Cursor, Trae, etc.) to generate Python scripts that control lights in DigiShow through the OSC protocol.


Project Files:
1. light_osc_server.dgs - DigiShow project file containing OSC server configuration and light control settings
2. AI_prompt.txt - Contains prompts for AI code generation
3. light_animation.py - AI-generated light animation Python script (you can also create your own version)


Demonstration Steps:
1. Open and start light_osc_server.dgs in DigiShow
2. Input the prompts from AI_prompt.txt into the AI tool, save the generated Python script as light_animation.py
3. Run the generated Python script through command line:

python3 light_animation.py


Note: To use OSC functionality in Python scripts, you typically need to install additional packages (such as python-osc):

pip3 install python-osc

Please refer to the AI conversation for details