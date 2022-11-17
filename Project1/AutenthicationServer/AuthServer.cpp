#include "AuthServer.h"
#include "ChatMessageProtocol.h"

#include "duthomhas/csprng.h"

#include <sstream>
#include <iostream>

//#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
//#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

void AuthServer::LifeCycle() {
	DEBUG_PRINT("AuthServer::LifeCycle()\n");

	ChatMessageProtocol cmp;
	Buffer* theBuffer = new Buffer(128);
	const int buflen = 128;
	char buf[buflen];

	short msgType;			// Type of the msg
	short userId;			// Chat user id
	int msgBufLen;
	std::string message;
	std::string email;
	std::string password;

	while (m_serverStatus) {
		// SocketsReadyForReading will be empty here
		FD_ZERO(&m_socketsReadyForReading);
		//FD_ZERO(&m_socketsReadyForReading);

		// Add all the sockets that have data ready to be recv'd 
		// to the socketsReadyForReading

		// 1. Add the listen socket, to see if there are any new connections
		FD_SET(m_socket, &m_socketsReadyForReading);

		// 2. Add all the connected sockets, to see if the is any information
		//    to be recieved from the connected clients.
		if(m_chatServerSocket != 0) {
			FD_SET(m_chatServerSocket, &m_socketsReadyForReading);
		}

		m_err = select(0, &m_socketsReadyForReading, NULL, NULL, &m_tv);
		if (m_err == SOCKET_ERROR) {
			throw "Select() failed with error: %d\n", WSAGetLastError();
		}

		// Check if our ListenSocket is set. This checks if there is a 
		// new client trying to connect to the server using a "connect" 
		// function call.
		Accept();

		// Check if ChatServer have sent data
		if (FD_ISSET(m_chatServerSocket, &m_socketsReadyForReading)) {

			char buf[buflen]{};
			//theBuffer = new Buffer(128);

			int recvResult = recv(m_chatServerSocket, buf, buflen, 0);

			// Saving some time to not modify a vector while 
			// iterating through it. Want remove the client
			// from the vector
			if (recvResult == 0) {
				// Chat user disconnected
				continue;
			}

			//theBuffer = cmp.DecodeProtocol(buf);
			// Copies the buffer received to the class Buffer
			theBuffer->m_BufferData = std::vector<uint8_t>(&buf[0], &buf[buflen]);
			// Resets the Read Index
			theBuffer->m_ReadBufferIndex = 0;
			// Reads the size of the buffer
			msgBufLen = theBuffer->ReadUInt32BE();
			// Reads the type of the message
			msgType = theBuffer->ReadShort16BE();
			// Variable utilized for response when needed
			char* responseBuf;

			switch (msgType) {
			case ChatMessageProtocol::MESSAGE_TYPE::REGISTER:
			{
				email = theBuffer->ReadStringBE(msgBufLen + 1 - 6); // Reads the roomname + " " + message { 6 = 4 (buflen) + 2 (msgtype) ... roomname ... }
				std::stringstream ss(email);
				std::getline(ss, email, ' ');
				std::getline(ss, password);
				this->RegisterNewUser(email, password);
				// ----------
				// TODO: Treatment of RegisterNewUser return and feedback to the user trying to register
				//message = "Could not register new user.";
				msgBufLen = 6;
				Buffer respBuff(msgBufLen);
				respBuff.WriteInt32BE(msgBufLen);
				respBuff.WriteShort16BE(-1);
				//respBuff.WriteStringBE(message);
				responseBuf = (char*)&respBuff.m_BufferData[0];
				send(m_chatServerSocket, responseBuf, respBuff.m_BufferSize, 0);
				break;
			}
			case ChatMessageProtocol::MESSAGE_TYPE::LOGIN:
			{
				email = theBuffer->ReadStringBE(msgBufLen + 1 - 6); // Reads the roomname + " " + message { 6 = 4 (buflen) + 2 (msgtype) ... roomname ... }
				std::stringstream ss(email);
				std::getline(ss, email, ' ');
				std::getline(ss, password);
				this->AuthenticateUser(email, password);
				// ----------
				// TODO: Treatment of RegisterNewUser return and feedback to the user trying to login
				//message = "Could not authenticate new user.";
				msgBufLen = 6;
				Buffer respBuff(msgBufLen);
				respBuff.WriteInt32BE(msgBufLen);
				respBuff.WriteShort16BE(-1);
				//responseBuf = (char*)&respBuff.m_BufferData[0];
				send(m_chatServerSocket, (char*)&respBuff.m_BufferData[0], respBuff.m_BufferSize, 0);
				break;
			}
			}
		}
	}
	Socket::Close();
}

void AuthServer::StartUp() {
	DEBUG_PRINT("AuthServer::StartUp()\n");

	//Socket::Initialize();
	ServerAddrinfoSetup();
	Open();
	Bind();
	Listen();

	m_serverStatus = true;

	LifeCycle();
}

void AuthServer::Shutdown() {
	DEBUG_PRINT("AuthServer::Shutdown()\n");
	m_serverStatus = false;
}

bool AuthServer::RegisterNewUser(std::string email, std::string password) {
	CSPRNG rng = csprng_create();
	if (!rng) {
		return 1;
	}
	long n = csprng_get_int(rng);
	std::cout << "Random int using CSPRNG: " << n << "\n";

	return false;
}

bool AuthServer::AuthenticateUser(std::string email, std::string password) {
	return false;
}

// Checks if Chat Server trying to connect to the Auth server
void AuthServer::Accept() {
	// Check if our ListenSocket is set. This checks if there is a 
	// new client trying to connect to the server using a "connect" 
	// function call.
	if (FD_ISSET(m_socket, &m_socketsReadyForReading)) {
		SOCKET clientSocket = accept(m_socket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			throw "Accept failed with error: %d\n", WSAGetLastError();
		} else {
			DEBUG_PRINT("AuthServer::Accept()\n");
			// Setups Chat Server Socket
			m_chatServerSocket = clientSocket;
		}
	}
}