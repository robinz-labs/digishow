Remote Virtual Pipe
===================  

We provide several examples to demonstrate how to use the remote Virtual Pipe to synchronize signal data across multiple DigiShow instances, which can run on different computers within a network.  

By utilizing WebSocket communication, you can also connect your own HTML5, Python, or other applications to the remote Virtual Pipe provided by DigiShow to achieve the same signal data synchronization.  

1. Open the project file remote_pipe_demo.dgs in DigiShow and start it.

2. Select "New Instance" from the DigiShow menu. In the new DigiShow instance, open the project file remote_pipe_client.dgs located in the client directory. Once started, the virtual pipe connection and signal data synchronization between the two DigiShow instances will be established.

3. The client/html5 directory contains a simple HTML web page example, and the client/python directory contains a simple Python script example. These examples are designed to connect to the previously started remote_pipe_demo project, allowing you to read and write signal data within the DigiShow virtual pipe.