#include "ChatClient.h"
#include "Buffer.h"
#include "ChatMessageProtocol.h"

#include <iostream>

int main(int argc, char** argv) {
	ChatClient			cc;
	Buffer*				theBuffer;
	ChatMessageProtocol cmp;

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
	const int bufLen = 128;

	// Sends the username to the server
	send(cc.m_socket, buf, theBuffer->m_BufferSize, 0);

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
			tryAgain = false;
		}
	}

	// Main Loop
	tryAgain = false;
	while (tryAgain) {
		tryAgain = false;
	}


	system("pause");
	cc.Shutdown();

}