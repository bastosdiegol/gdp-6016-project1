# Network Programming Project #2: Authentication Service

This solution was created using Visual Studio Community 2019 Version 16 Toolset v142.

The project is currently configured to be build in Debug and Release mode, both on x64 architecture.

It creates two separate executable files Client and Server.

You can run the this project through Visual Studio Solution or directly executing its .exe

Make sure to run Client and Server on the same Build.

The server expects no inputs.

These are the user inputs expected during the use of the chat client:
- Type /join roomname to enter a room.
- Type /leave roomname to enter a room.
- Type /quit to leave the server.
- Type /roomname message to broadcast a message to the room.
- Press ESC or BS to clear the message box.

Known issues:
- On Sing In, currently there's a bug on the socket communication flow that requires more than 1 attempt to receive the correct response from the server.
- While it's not fixed you can follow Debug messages to see that the Server is ready to authenticate a user even though on a later try.