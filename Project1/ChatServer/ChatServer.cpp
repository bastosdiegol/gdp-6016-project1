#include "ChatServer.h"
#include "ChatMessageProtocol.h"

#include <sstream>
#include "authcomm.pb.h"

//#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
//#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

void ChatServer::LifeCycle() {

	ChatMessageProtocol cmp;
	Buffer* theBuffer = new Buffer(128);
	const int buflen	= 128;

	int msgBufLen;			// Msg received buf lenght
	short msgType;			// Type of the msg
	std::string userName;	// Chat user name
	short userId;			// Chat user id
	std::string roomName;	// room name
	short roomId;			// room id
	std::string message;
	std::string email;
	std::string password;

	DEBUG_PRINT("ChatServer::LifeCycle()\n");
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
		for (int i = 0; i < m_chatUsers.size(); i++) {
			FD_SET(m_chatUsers[i].userSocket, &m_socketsReadyForReading);
		}

		m_err = select(0, &m_socketsReadyForReading, NULL, NULL, &m_tv);
		if (m_err == SOCKET_ERROR) {
			throw "Select() failed with error: %d\n", WSAGetLastError();
		}

		// Check if our ListenSocket is set. This checks if there is a 
		// new client trying to connect to the server using a "connect" 
		// function call.
		Accept();

		// Check if any of the currently connected clients have sent data
		for (int i = m_chatUsers.size() - 1; i >= 0; i--) {
			if (FD_ISSET(m_chatUsers[i].userSocket, &m_socketsReadyForReading)) {
				
				char buf[buflen]{};
				//theBuffer = new Buffer(128);

				int recvResult = recv(m_chatUsers[i].userSocket, buf, buflen, 0);

				// Saving some time to not modify a vector while 
				// iterating through it. Want remove the client
				// from the vector
				if (recvResult == 0) {
					// Chat user disconnected
					LeaveServer(m_chatUsers[i].id);
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
				case ChatMessageProtocol::MESSAGE_TYPE::JOIN_SERVER :
					userName = theBuffer->ReadStringBE(msgBufLen - 6);				// Reads the username { 6 = 4 (buflen) + 2 (msgtype) ... username }
					userId = this->JoinServer(userName, m_chatUsers[i].userSocket); // Adds the user on the server
					//theBuffer = new Buffer(2);									// Prepares a Buffer for the user id
					theBuffer->m_BufferData = std::vector<uint8_t>(2);				// Prepares a Buffer for the user id
					theBuffer->m_WriteBufferIndex = 0;								// Resets the write index
					theBuffer->WriteShort16BE(m_chatUsers[i].id);					// Writes the ID on the buffer
					responseBuf = (char*)&theBuffer->m_BufferData[0];				// Converts to the type accepted by send()
					send(m_chatUsers[i].userSocket, responseBuf, recvResult, 0);	// Sends back the user ID
					break;
				case ChatMessageProtocol::MESSAGE_TYPE::LEAVE_SERVER :
					this->LeaveServer(m_chatUsers[i].id);
					break;
				case ChatMessageProtocol::MESSAGE_TYPE::JOIN_ROOM :
					roomName = theBuffer->ReadStringBE(msgBufLen - 6);		// Reads the roomname { 6 = 4 (buflen) + 2 (msgtype) ... roomname ... }
					roomId = this->JoinRoom(roomName, m_chatUsers[i].id);	// Returns the Room id
					// Broadcast the new user to the room
					this->BroadcastMessage(roomId, "["+ roomName +"] " + m_chatUsers[i].name + " has joined the room.");
					break;
				case ChatMessageProtocol::MESSAGE_TYPE::LEAVE_ROOM :
					roomName = theBuffer->ReadStringBE(msgBufLen - 6);		// Reads the roomname { 6 = 4 (buflen) + 2 (msgtype) ... roomname ... }
					for (int k = 0; k < m_chatRooms.size(); k++) {
						if (m_chatRooms[k].name == roomName)
							roomId = m_chatRooms[k].id;
					}
					this->LeaveRoom(m_chatUsers[i].id, roomId);
					this->BroadcastMessage(roomId, "[" + roomName + "] " + m_chatUsers[i].name + " has left the room.");
					break;
				case ChatMessageProtocol::MESSAGE_TYPE::MESSAGE :
					{
						roomName = theBuffer->ReadStringBE(msgBufLen + 1 - 6); // Reads the roomname + " " + message { 6 = 4 (buflen) + 2 (msgtype) ... roomname ... }
						std::stringstream ss(roomName);
						std::getline(ss, roomName, ' ');
						for (int k = 0; k < m_chatRooms.size(); k++) {
							if (m_chatRooms[k].name == roomName)
								roomId = m_chatRooms[k].id;
						}
						std::getline(ss, message);
						this->BroadcastMessage(roomId, "[" + roomName + "] " + m_chatUsers[i].name + ": " + message);
						break;
					}
				case ChatMessageProtocol::MESSAGE_TYPE::REGISTER :
					{
					// Reads the email + " " + password { 6 = 4 (buflen) + email + ' ' + password ... }
						email = theBuffer->ReadStringBE(msgBufLen + 1 - 6); 
						std::stringstream ss(email);
						std::getline(ss, email, ' ');
						std::getline(ss, password);
						bool bAccCreation = this->RegisterNewUser(&m_chatUsers[i], email, password);
						
						msgBufLen = 6;
						Buffer respBuff(msgBufLen);
						respBuff.WriteInt32BE(msgBufLen);
						if (bAccCreation) {
							respBuff.WriteShort16BE(m_chatUsers[i].id);
						} else {
							respBuff.WriteShort16BE(-1);
						}
						responseBuf = (char*)&respBuff.m_BufferData[0];
						send(m_chatUsers[i].userSocket, responseBuf, respBuff.m_BufferSize, 0);
						break;
					}
				case ChatMessageProtocol::MESSAGE_TYPE::LOGIN :
					{
					// Reads the email + " " + password { 6 = 4 (buflen) + email + ' ' + password ... }
						email = theBuffer->ReadStringBE(msgBufLen + 1 - 6); 
						std::stringstream ss(email);
						std::getline(ss, email, ' ');
						std::getline(ss, password);
						bool bAccAuth = this->AuthenticateUser(&m_chatUsers[i], email, password);
						
						msgBufLen = 6;
						Buffer respBuff(msgBufLen);
						respBuff.WriteInt32BE(msgBufLen);
						if (bAccAuth) {
							respBuff.WriteShort16BE(m_chatUsers[i].id);
						} else {
							respBuff.WriteShort16BE(-1);
						}
						send(m_chatUsers[i].userSocket, (char*)&respBuff.m_BufferData[0], respBuff.m_BufferSize, 0);
						break;
					}
				}
			}
		}
	}
	Socket::Close();
}

void ChatServer::StartUp(){
	DEBUG_PRINT("ChatServer::StartUp()\n");
	//FD_ZERO(&m_activeSockets);
	FD_ZERO(&m_socketsReadyForReading);

	m_roomIdIndex = 0;
	m_userIdIndex = 0;

	m_tv.tv_sec	= 0;
	m_tv.tv_usec	= 500 * 1000; // 500 milliseconds, half a second

	//Socket::Initialize();
	ServerAddrinfoSetup();
	Open();
	Bind();
	Listen();

	m_serverStatus = true;

	LifeCycle();
}

void ChatServer::Shutdown(){
	DEBUG_PRINT("ChatServer::Shutdown()\n");
	m_serverStatus = false;
}

short ChatServer::JoinServer(std::string name, SOCKET userSocket) {
	DEBUG_PRINT("ChatServer::JoinServer()\n");
	//// Generates new ID for the user
	//short userIndex = m_userIdIndex;
	//// Adds the new user
	//m_chatUsers.push_back(ChatUser{ userIndex, name, userSocket });
	//// Increments the user id index
	//m_userIdIndex++;
	//// Returns the id of the new user
	
	// User lookup
	for (int i = 0; i < m_chatUsers.size(); i++) {
		// Checks for the correct socket
		if (m_chatUsers[i].userSocket == userSocket) {
			m_chatUsers[i].name = name; // setup username
			return m_chatUsers[i].id;   // return userid
		}
	}
	return -1;
}

short ChatServer::JoinRoom(std::string name, short userID){
	DEBUG_PRINT("ChatServer::JoinRoom()\n");
	// Tries to find the Chat Room by name
	bool isRoomFound = false;
	short roomIndex;
	for (int i = 0; i < m_chatRooms.size(); i++) {
		if (m_chatRooms[i].name == name) {
			isRoomFound = true;
			roomIndex = i;
		}
	}
	// Checks if Room with same name was found
	if (isRoomFound) {
		// Adds the user on the already existing room
		m_chatRooms[roomIndex].users.push_back(userID);
		// Room already exists, return ChatID
		return roomIndex;
	} else {
		// New room index
		roomIndex = m_roomIdIndex;
		// Creates the new room with the current user
		std::vector<short> users;
		users.push_back(userID);
		m_chatRooms.push_back(ChatRoom{ roomIndex
										, name
										, users });
		// Increments the RoomIdIndex
		m_roomIdIndex++;
		// Return the room id
		return roomIndex;
	}
}

void ChatServer::LeaveServer(short userID){
	DEBUG_PRINT("ChatServer::LeaveServer()\n");
	// Tries to remove the user from each room
	// int numRooms = m_chatRooms.size() - 1;  // We gonna use an index since we could be removing rooms
	for (int i = m_chatRooms.size()-1; i >= 0; i--) {	// Decrescent since we could be removing rooms
		LeaveRoom(userID, m_chatRooms[i].id);
	}
	// User lookup
	for (int i = 0; i < m_chatUsers.size(); i++) {
		if (userID == m_chatUsers[i].id) {
			// Removes the user from the server
			m_chatUsers.erase(m_chatUsers.begin() + i);
		}
	}
}

void ChatServer::LeaveRoom(short userID, short roomID) {
	DEBUG_PRINT("ChatServer::LeaveRoom()\n");
	// Room lookup
	for (int i = 0; i < m_chatRooms.size(); i++) {
		if (m_chatRooms[i].id == roomID) {
			// Is this user the last one?
			if (m_chatRooms[i].users.size() == 1) {
				// Remove the user
				m_chatRooms[i].users.pop_back();
				// Remove the room
				m_chatRooms.erase(m_chatRooms.begin()+i);
			} else {
				// User lookup
				for (int j = 0; j < m_chatRooms[i].users.size(); j++) {
					if (userID == m_chatRooms[i].users[j]) {
						// Removes only the user
						m_chatRooms[i].users.erase(m_chatRooms[i].users.begin() + j);
					}
				}
			}
		}
	}
}

std::string ChatServer::ListRooms(){
	DEBUG_PRINT("ChatServer::ListRooms()\n");
	std::string roomList;
	int roomQty = m_chatRooms.size();
	if (roomQty == 0) {
		roomList = "There's no room available.\nType /join roomName to create a new room.\n";
	} else {
		roomList = "Room(s) available:\n";
		for (int i = 0; i < roomQty; i++) {
			roomList += "Room " + std::to_string(i + 1) + ": " + m_chatRooms[i].name + "\n";
		}
		roomList += "Type /join roomName join a existing room or create a new one.\n";
	}
	return roomList;
}

void ChatServer::BroadcastMessage(short roomID, std::string message) {
	Buffer theBuffer(message.size());
	theBuffer.WriteStringBE(message);
	// Converts the Data from the Buffer Class Into variable accepted by send()
	char* buf = (char*)&theBuffer.m_BufferData[0];
	int bufLen = theBuffer.m_BufferSize;

	DEBUG_PRINT("ChatServer::BroadcastMessage()\n");
	// Room lookup
	for (int i = 0; i < m_chatRooms.size(); i++) {
		if (m_chatRooms[i].id == roomID) {
			// Sends a message to each user
			for (int j = 0; j < m_chatRooms[i].users.size(); j++) {
				// User lookup
				for (int k = 0; k < m_chatUsers.size(); k++) {
					if (m_chatUsers[k].id == m_chatRooms[i].users[j]) {
						send(m_chatUsers[k].userSocket, buf, bufLen, 0);
					}
				}
				
			}
		}
	}
}

bool ChatServer::RegisterNewUser(ChatUser* user, std::string email, std::string password) {
	DEBUG_PRINT("ChatServer::RegisterNewUser(%s, %s)\n", email.c_str(), password.c_str());
	// Creates the Protobuff Request
	auth::ChatServerRequest serverRequest;
	serverRequest.set_type(auth::ChatServerRequest::CREATEACC);
	serverRequest.set_id(user->id);
	serverRequest.set_email(email);
	serverRequest.set_password(password);
	// Serialize it
	std::string serializedString;
	serverRequest.SerializeToString(&serializedString);
	// Adds it to the Buffer with its size as header
	Buffer theBuffer(4);
	theBuffer.WriteInt32BE(serializedString.size());
	theBuffer.WriteStringBE(serializedString);
	// Sends to the AuthServer
	send(m_authClient.m_socket, (char*)&theBuffer.m_BufferData[0], theBuffer.m_BufferSize, 0);

	int result = -1;
	int	bufLen = 128;
	char* buf = new char[bufLen];
	// Wait to AuthServer Response
	do {
		// Receives server response for autenthication
		int result = recv(m_authClient.m_socket, buf, bufLen, 0);
		if (result == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
			} else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(m_authClient.m_socket);
				WSACleanup();
				return false;
			}
		} else {
			// Copies the buffer received to the class Buffer
			theBuffer.m_BufferData.resize(result);
			theBuffer.m_BufferSize = result;
			theBuffer.m_BufferData = std::vector<uint8_t>(&buf[0], &buf[result]);
			theBuffer.m_ReadBufferIndex = 0;
			// Reads pack size
			int packSize = theBuffer.ReadUInt32BE();
			auth::AuthServerResponse authResponse;
			std::string message = theBuffer.ReadStringBE(packSize);
			bool result = authResponse.ParseFromString(message);
			if (!result) {
				DEBUG_PRINT("Failed to parse server request\n");
				continue;
			}
			short msgType = serverRequest.type();
			// Switch to verify if the user was created or if it already exists on the DB
			switch (msgType) {
			case auth::AuthServerResponse_ResponseType_ACCCREATED :
				user->id = authResponse.userid();
				return true;
				break;
			case auth::AuthServerResponse_ResponseType_ACCEXISTS :
				DEBUG_PRINT("Could not authenticate or register\n");
				return false;
				break;
			}
		}
	} while (result > 0);

	return false;
}

bool ChatServer::AuthenticateUser(ChatUser* user, std::string email, std::string password) {
	DEBUG_PRINT("ChatServer::AuthenticateUser(%s, %s)\n", email.c_str(), password.c_str());
	// Creates the Protobuff Request
	auth::ChatServerRequest serverRequest;
	serverRequest.set_type(auth::ChatServerRequest::AUTHENTICATE);
	serverRequest.set_id(user->id);
	serverRequest.set_email(email);
	serverRequest.set_password(password);
	// Serialize it
	std::string serializedString;
	serverRequest.SerializeToString(&serializedString);
	// Adds it to the Buffer with its size as header
	Buffer theBuffer(4);
	theBuffer.WriteInt32BE(serializedString.size());
	theBuffer.WriteStringBE(serializedString);
	// Sends to the AuthServer
	send(m_authClient.m_socket, (char*)&theBuffer.m_BufferData[0], theBuffer.m_BufferSize, 0);

	int result = -1;
	int	bufLen = 128;
	char* buf = new char[bufLen];
	// Wait to AuthServer Response
	do {
		// Receives server response for autenthication
		int result = recv(m_authClient.m_socket, buf, bufLen, 0);
		if (result == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
			} else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(m_authClient.m_socket);
				WSACleanup();
				return false;
			}
		} else if(result > 0) {
			// Copies the buffer received to the class Buffer
			theBuffer.m_BufferData.resize(result);
			theBuffer.m_BufferSize = result;
			theBuffer.m_BufferData = std::vector<uint8_t>(&buf[0], &buf[result]);
			theBuffer.m_ReadBufferIndex = 0;
			// Reads pack size
			int packSize = theBuffer.ReadUInt32BE();
			auth::AuthServerResponse authResponse;
			std::string message = theBuffer.ReadStringBE(packSize);
			bool result = authResponse.ParseFromString(message);
			if (!result) {
				DEBUG_PRINT("Failed to parse server request\n");
				continue;
			}
			short msgType = authResponse.type();
			// Switch to verify if the user was created or if it already exists on the DB
			DEBUG_PRINT("Message Type from AuthResponse: %d\n", msgType);
			switch (msgType) {
			case auth::AuthServerResponse_ResponseType_AUTHSUCCESS:
				user->id = authResponse.userid();
				return true;
				break;
			case auth::AuthServerResponse_ResponseType_AUTHFAILURE:
				DEBUG_PRINT("Could not authenticate or register\n");
				return false;
				break;
			}
		}
	} while (result > 0);

	return false;
}

// Checks if theres a new Chat User trying to connect to the server
void ChatServer::Accept() {
	//DEBUG_PRINT("ChatServer::Accept()\n");
	// Check if our ListenSocket is set. This checks if there is a 
	// new client trying to connect to the server using a "connect" 
	// function call.
	if (FD_ISSET(m_socket, &m_socketsReadyForReading)) {
		SOCKET clientSocket = accept(m_socket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			throw "Accept failed with error: %d\n", WSAGetLastError();
		} else {
			// Setups the new user
			ChatUser newChatUser;
			newChatUser.id = m_userIdIndex;
			newChatUser.userSocket = clientSocket;
			// Adds the new user to the array of chat users
			m_chatUsers.push_back(newChatUser);
			// Increments the user id index
			m_userIdIndex++;
		}
	}
}