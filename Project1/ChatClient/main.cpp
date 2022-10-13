#include "ChatClient.h"
#include "Buffer.h"
#include "ChatMessageProtocol.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <conio.h>
#include <algorithm>

int main(int argc, char** argv) {
	ChatClient			cc;
	Buffer*				theBuffer;
	ChatMessageProtocol cmp;
	int					result;		// Socket result
	std::stringstream   messageHistory;
	std::stringstream	ss;			// stringstrea used to split message
	std::string			item;		// string iterator
	std::string			message;	// broadcast message
	std::string			messageTemp;// message being typed
	int					key;		// Keyboard key pressed
	bool				tryAgain = true; // main loops exit variable
	bool				newMessage = false;
	std::vector<std::string>::iterator it; // Iterator for roomlookup

	// Beginning of the Program - Ask username
	std::cout << "Enter your username: ";
	std::cin >> cc.m_name;
	// Writes the username on the Buffer class
	// Uses the Apply Protocol to comunicate to the Chat Server
	theBuffer = cmp.ApplyProtocol("/name " + cc.m_name, cc.m_id);

	// Connects to the ChatServer
	cc.StartUp("127.0.0.1");

	//cc.Write((char*)&theBuffer->m_BufferData, theBuffer->m_BufferSize);
	// Converts the Data from the Buffer Class Into variable accepted by send()
	char* buf = (char*)&theBuffer->m_BufferData[0];
	int bufLen = theBuffer->m_BufferSize;

	// Sends the username to the server
	send(cc.m_socket, buf, bufLen, 0);

	// Tries to receive the User ID from the ChatServer
	while (tryAgain) {
		int userid = recv(cc.m_socket, buf, bufLen, 0);
		if (userid == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
			} else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(cc.m_socket);
				WSACleanup();
				return 1;
			}
		} else {
			// Copies the buffer received to the class Buffer
			theBuffer->m_BufferData = std::vector<uint8_t>(&buf[0], &buf[bufLen]);
			// Reads the userid
			userid = theBuffer->ReadShort16BE();
			cc.m_id = userid;
			// Welcoming messages displayed in the console
			messageHistory << "\033[2J\033[1;1H"; // Clear Screen
			messageHistory << "Welcome to the chat server, " << cc.m_name << "!" << std::endl;
			messageHistory << "Type /join roomname to enter a room." << std::endl;
			messageHistory << "Type /leave roomname to enter a room." << std::endl;
			messageHistory << "Type /quit to leave the server." << std::endl;
			messageHistory << "Type /roomname message to broadcast a message to the room." << std::endl;
			
			std::cout << messageHistory.str();
			tryAgain = false;
		}
	}
	// **********
	// 
	//	Main Loop
	// 
	// **********
	tryAgain = true;
	message = "";	// message
	messageTemp = "";
	while (tryAgain) {
		bufLen = 128;		// max message len 
		char recvbuf[128]{};

		// Checks if theres new message from the server
		result = recv(cc.m_socket, recvbuf, bufLen, 0);
		if (result != -1) {
			Buffer* theBuffer = new Buffer(result);
			//theBuffer = cmp.DecodeProtocol(buf);
			// Copies the buffer received to the class Buffer
			theBuffer->m_BufferData = std::vector<uint8_t>(&recvbuf[0], &recvbuf[bufLen]);
			item = theBuffer->ReadStringBE(result);
			messageHistory << item << std::endl;
			newMessage = true;
		}

		// Reads the user console writing
		//std::cin >> message;
		//while (true) {
		if(newMessage){
			std::cout << messageHistory.str() << std::endl;
			std::cout << messageTemp << std::endl;
			newMessage = false;
		}
		if (_kbhit()) {
			key = _getch();
			if (key == 13) { // ENTER KEY
				message = messageTemp;
				messageTemp = "";
				//break;
			} else if(key == 27){ // ESCAPE KEY
				messageTemp = "";
				//break;
			} else {
				messageTemp += (char)key;
			}
			newMessage = true;
		}
		//}
		
		// Conditional to adjust the message to be broadcasted
		if (message != "") {
			ss = std::stringstream(message);
			std::getline(ss, item, ' '); // gets the first command
			if (item == "/quit"){ // leaving the loop
				tryAgain = false;
				continue;
			}
			// Now we are gonna separate the channel name from the /
			std::string roomName = item.substr(1, item.size());
			// Checks its not other channel commands
			if (roomName != "leave" && roomName != "join" && roomName != "quit") {
				if (std::find(cc.m_rooms.begin(), cc.m_rooms.end(), roomName) != cc.m_rooms.end()) // tries to find if the user is inside the room
					// Apply Protocolfor /m channalname message
					message = "/m " + roomName + " " + message.substr(item.size() + 1, message.size());
				else
					message = "";
			} else if (roomName == "join") {
				std::getline(ss, item, ' '); // gets the roomname
				cc.m_rooms.push_back(item);
			} else if (roomName == "leave") {
				std::getline(ss, item, ' '); // gets the roomname
				it = std::find(cc.m_rooms.begin(), cc.m_rooms.end(), item);
				if (it != cc.m_rooms.end()) {
					cc.m_rooms.erase(it);
					messageHistory << "You have left the [" << item << "] room." << std::endl;
					newMessage = true;
				}
			}

			// Applies the Message Protocol to the message typed by the usar
			theBuffer = cmp.ApplyProtocol(message, cc.m_id);
			if (theBuffer != nullptr) {
				// Updates buffer and buffer lenght variables
				buf = (char*)&theBuffer->m_BufferData[0];
				bufLen = theBuffer->m_BufferSize;
				// Sends the message to the server
				send(cc.m_socket, buf, bufLen, 0);
			}
			// Transform into stringstream
			std::stringstream ss(message);
			// Breaks first part
			std::getline(ss, item, ' ');
			
			message = ""; // CLEARS the message
		}
	}
	cc.Shutdown();
}