# DigiShow JavaScript Expression Reference
 
JS expression is a formula that can dynamically change the value of a signal. The following variables and functions can be used in the expression written in JavaScript syntax. 
 
 
## Common variables in expressions
 
**value** 
The original value of this signal before the expression is calculated. 
 
**range** 
The maximum value of this signal in the range. 
 
**lastValue** 
The last value stored in this signal after the expression is calculated. 
 
 
## Common functions in expressions
 
**inputValueOf(name)** 
Get the input value of the signal link bar with the title _name_ . 
 
**outputValueOf(name)** 
Get the output value of the signal link bar with the title _name_ . 
 
**alert(message)** 
Display an information dialog box with the text _message_, and close it after pressing the "OK" button. 
 
**toast(message)** 
Display a notification prompt box with the text _message_, which will automatically disappear after staying for a few seconds. 

An expression can also contain multiple statements, and the statements need to be separated by ; semicolons. The last statement is used to return the value calculated by the expression. For example: 
_alert("value = " + value); value_ 
  
In addition, there are more functions that can be called in expressions and the attached script file of DigiShow projects. 
 
 
## More functions
 
**setOutputValueOf(name, value)** 
Change the output value of the signal link bar with the title _name_ to _value_. Returns true if the call succeeds, false if it fails. 
 
**appVersion()** 
Returns the DigiShow application version number string. 
 
**appFilePath(filename)** 
Returns the absolute path of the file named _filename_ in the directory where the DigiShow project file is located. 
 
**appDataPath(filename)** 
Returns the absolute path of the file named _filename_ in the DigiShow configuration data directory. 
 
(For more that can be called in JS scripts, see the functions marked with Q_INVOKABLE in the digishow_environment.h header file) 
 
 
## Functions of the app object
 
**app.restart()** 
Restart the current DigiShow project. 
 
**app.startLaunch(name)** 
Start the specified preset (launch) item, the parameter _name_ can usually be 'launch1', 'launch2', 'launch3' ... 
 
(For more about the app object that can be called in JS scripts, see the functions marked with Q_INVOKABLE in the digishow_app.h header file) 
 
 
## Functions of the utilities object
 
**utilities.runInShell(program, arguments)** 
Run the program program in the operating system shell, and specify the various parameters of the program in the array arguments.
For example: _utilities.runInShell('shutdown', ['-f', '-s', '-t', '0'])_ 
It is equivalent to running _shutdown -f -s -t 0_ in the command line. 

**utilities.httpRequest(url)**
Invoke an http request to the target _url_ with the GET method and obtain the content returned by the server. 
 
**utilities.httpRequest(url, method, contents, timeout)** 
Invoke an http request to the target _url_ with the GET / POST / PUT method and obtain the content returned by the server. 
Parameter _method_ can be 'get', 'post' or 'put'. 
Parameter _contents_ is the data content posted or put to the http server. 
Parameter _timeout_ is in milliseconds. When the timeout of waiting for the http request reply is exceeded, the function will return an empty string. 
 
**utilities.delay(timeout)** 
The program pauses for a specific time and then continues after the timeout. The parameter _timeout_ is in milliseconds. During the pause, the DigiShow main program remains active and signals are sent and received normally. 
 
(For more about the utilities object that can be called in JS scripts, see the functions marked with Q_INVOKABLE in the app_utilities.h header file) 
 
 
## Properties of the metronome object
 
The metronome object can be used to implement various controls on the beat maker. 
 
**metronome.isRunning** 
The running status of the beat maker, the value can be true or false, this property is readable and writable. 
 
**metronome.isSoundEnabled** 
Whether the beat maker makes a sound when it beats, the value can be true or false, this property is readable and writable. 
 
**metronome.isLinkEnabled** 
Whether the beat maker keeps the beat synchronization with other software that supports the Ableton Link interface, the value can be true or false, this property is readable and writable. 
 
**metronome.bpm** 
The tempo status of the beat maker, the bpm value is used to mark the number of beats per minute, this property is readable and writable. 
 
**metronome.quantum** 
The number of beats per measure of the beat maker, the default is 4, this property is readable and writable. 
 
**metronome.beat** 
The current total beat count of the running beat maker, this property is read-only. 
 
**metronome.phase** 
The number of beats in the current measure of the running beat maker, such as: 1, 2, 3, 4. This property is read-only. 
 
(For more about the metronome object that can be called in JS scripts, see the functions marked with Q_INVOKABLE in the digishow_metronome.h header file) 
