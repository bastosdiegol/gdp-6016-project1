#include "AuthServer.h"
#include "ChatMessageProtocol.h"
#include "authcomm.pb.h"

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

	dbconn = new DBConnector();
	dbconn->Connect("authdb");

	// Random salt generator
	rng = csprng_create();
	if (!rng) {
		DEBUG_PRINT("Error creating CSPRNG. Exiting application.\n");
		return;
	}

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
			// Reads the proto message
			message = theBuffer->ReadStringBE(msgBufLen);

			auth::ChatServerRequest serverRequest;
			bool result = serverRequest.ParseFromString(message);
			if (!result) {
				DEBUG_PRINT("Failed to parse server request\n");
				continue;
			}
			msgType = serverRequest.type();

			switch (msgType) {
			case auth::ChatServerRequest::CREATEACC : //ChatMessageProtocol::MESSAGE_TYPE::REGISTER:
			{
				auth::AuthServerResponse authResponse;
				authResponse.set_id(serverRequest.id());
				// Tries to insert the user on the database
				// RegisterNewUser will properly create Salt and Hash the password
				bool registerResponse = RegisterNewUser(serverRequest.email(), serverRequest.password());
				// Sets the Type of AuthServer Response based on the DB return
				if (registerResponse == true) {
					authResponse.set_type(auth::AuthServerResponse_ResponseType_ACCCREATED);
					// Grabs the new userid
					authResponse.set_userid(dbconn->findUserId(serverRequest.email()));
				} else {
					authResponse.set_type(auth::AuthServerResponse_ResponseType_ACCEXISTS);
				}

				std::string serializedString;
				authResponse.SerializeToString(&serializedString);
				
				Buffer respBuff(4);
				// Writes the buffer size
				respBuff.WriteInt32BE(serializedString.size());
				respBuff.WriteStringBE(serializedString);
				// Sends the response to the ChatServer
				send(m_chatServerSocket, (char*)&respBuff.m_BufferData[0], respBuff.m_BufferSize, 0);
				break;
			}
			case auth::ChatServerRequest::AUTHENTICATE : //ChatMessageProtocol::MESSAGE_TYPE::LOGIN:
			{
				auth::AuthServerResponse authResponse;
				authResponse.set_id(serverRequest.id());
				// Tries to insert the user on the database
				// RegisterNewUser will properly create Salt and Hash the password
				bool registerResponse = AuthenticateUser(serverRequest.email(), serverRequest.password());
				// Sets the Type of AuthServer Response based on the DB return
				if (registerResponse == true) {
					authResponse.set_type(auth::AuthServerResponse_ResponseType_AUTHSUCCESS);
					// Grabs the userid
					authResponse.set_userid(dbconn->findUserId(serverRequest.email()));
				} else {
					authResponse.set_type(auth::AuthServerResponse_ResponseType_AUTHFAILURE);
				}

				std::string serializedString;
				authResponse.SerializeToString(&serializedString);

				Buffer respBuff(4);
				// Writes the buffer size
				respBuff.WriteInt32BE(serializedString.size());
				respBuff.WriteStringBE(serializedString);
				// Sends the response to the ChatServer
				send(m_chatServerSocket, (char*)&respBuff.m_BufferData[0], respBuff.m_BufferSize, 0);
				break;
			}
			}
		}
	}
	Socket::Close();
	delete dbconn;
	delete rng;
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

	// Creates new salt
	long salt = csprng_get_int(rng);
	// Appends salt to the password
	std::string newPassword = std::to_string(salt).append(password);
	// Hashes new salted password
	std::string hashedPasswordToString = sha256(newPassword);

	return dbconn->insertUser(email, std::to_string(salt), hashedPasswordToString);
}

int AuthServer::AuthenticateUser(std::string email, std::string password) {

	std::string salt = dbconn->findUserSalt(email);
	if (salt == "") {
		return false;
	}
	// Appends salt to the password
	std::string newPassword = salt.append(password);
	// Hashes salted password
	std::string hashedPasswordToString = sha256(newPassword);

	return dbconn->authenticateUser(email, hashedPasswordToString);

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