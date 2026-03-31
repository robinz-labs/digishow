Using JavaScript Expressions and Scripts in DigiShow
====================================================

expression.dgs
Bind JS expressions to inputs/outputs to dynamically change signal values. For example, turning a standard button into different mode-switch buttons — commonly used in interactive installations for triggering logic.

script.dgs
This project includes a script file (script.scriptable.txt) containing a custom onValueChanged() function. You can call this function by binding expressions to inputs/outputs. In this example, moving the Input fader changes the RGB light's preset color.

time.dgs
Use JavaScript date and time functions to assign signal values, enabling clock and timed triggers in DigiShow.

shell_command
Write JavaScript code to call operating system command-line tools from DigiShow. For example, safely shutting down Windows.

http_request
Write JavaScript code to send HTTP requests — such as controlling the external media player Kodi.

code_sender
Write JavaScript code to send G-code commands to target devices via serial, TCP, or UDP.

