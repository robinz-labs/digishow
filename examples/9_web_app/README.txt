Make A Web App for DigiShow
===========================

When you need to develop a web app to work in coordination with DigiShow, please first read the README.md in the guide directory. You can also refer to the examples in the pipe_demo and smile_finder directories.


AI Coding
=========

To generate a web app via AI vibe coding, have the AI agent read guide/README.txt.

Steps:

1. Send guide/README.md to your AI agent.

2. Prompt, as example: "Create a web app that uses your computer camera to detect smiles and sends the smile status (boolean) to Virtual Pipe — Binary Channel 1 in DigiShow."

3. In DigiShow's Interface Manager, set Virtual Pipe Mode to "Local Pipe", and "Remote Link Service" to "Enable (Multiple)". Then add an input signal to the signal link: Virtual Pipe → Binary → Channel 1.

4. Start the DigiShow project and open the web app.
