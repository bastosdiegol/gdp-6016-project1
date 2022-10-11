#include "ChatClient.h"
#include "Buffer.h"
#include "ChatMessageProtocol.h"

#include <iostream>
#include <vector>
#include <sstream>

int main(int argc, char** argv) {
	ChatClient			cc;
	Buffer*				theBuffer;
	ChatMessageProtocol cmp;
	int					result;

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
	bool tryAgain = true;
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
			userid = theBuffer->ReadShort16LE();
			cc.m_id = userid;
			// Welcoming messages displayed in the console
			std::cout << "Welcome to the chat server, " << cc.m_name << "!" << std::endl;
			std::cout << "Type /join roomname to enter a room." << std::endl;
			std::cout << "Type /leave roomname to enter a room." << std::endl;
			std::cout << "Type /quit to leave the server." << std::endl;
			std::cout << "Type /roomname message to broadcast a message to the room." << std::endl;
			tryAgain = false;
		}
	}

	// Main Loop
	tryAgain = true;
	std::string message;
	std::string item;
	std::stringstream ss;
	while (tryAgain) {
		bufLen = 128;
		char recvbuf[128]{};
		result = recv(cc.m_socket, recvbuf, bufLen, 0);

		if (result != -1) {
			Buffer* theBuffer = new Buffer(result);
			//theBuffer = cmp.DecodeProtocol(buf);
			// Copies the buffer received to the class Buffer
			theBuffer->m_BufferData = std::vector<uint8_t>(&recvbuf[0], &recvbuf[bufLen]);
			std::cout << theBuffer->ReadStringLE(result);
		}

		std::cout << "\nMessage Box: ";
		// Reads the user console writing
		//std::cin >> message;
		std::getline(std::cin, message);
		if (message != "") {
			ss = std::stringstream(message);
			std::getline(ss, item, ' ');
			// Now we are gonna separate the channel name
			std::string roomName = item.substr(1, item.size());
			if (roomName != "leave" && roomName != "join" && roomName != "quit") {
				message = "/m " + roomName + " " + message.substr(item.size()+1, message.size());
			}
		}
		
		//if (item == "/leave") {
		//	// Iterator for the roomname roomid map
		//	std::map<std::string, short>::iterator it;
		//	// Reads the roomname
		//	std::getline(ss, item, ' ');
		//	// Finds it on the map
		//	it = cc.m_rooms.find(item);
		//	if (it != cc.m_rooms.end()) {
		//		// New message with ID instead
		//		message = "/leave " + it->second;
		//	}
		//}
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
		if(item == "/quit")
			tryAgain = false;
	}


	system("pause");
	cc.Shutdown();

}