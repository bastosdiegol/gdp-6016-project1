#include "ChatServer.h"

#define DEBUG_LOG_ENABLED
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

void ChatServer::LifeCycle() {
	DEBUG_PRINT("ChatServer::LifeCycle()\n");
	while (m_serverStatus) {
		// SocketsReadyForReading will be empty here
		FD_ZERO(&m_activeSockets);

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
				// Act as a ping server 
				const int buflen = 128;
				char buf[buflen];

				int recvResult = recv(m_chatUsers[i].userSocket, buf, buflen, 0);

				// Saving some time to not modify a vector while 
				// iterating through it. Want remove the client
				// from the vector
				if (recvResult == 0) {
					// Chat user disconnected
					LeaveServer(m_chatUsers[i].id);
					continue;
				}

				// Buffer HERE
				// Identify Message Command
				// DO Things
				printf("Message From the client:\n%s\n", buf);
				// Check if successful.. 
				// Check for errors..

				int sendResult = send(m_chatUsers[i].userSocket, buf, recvResult, 0);
				// Check if successful.. 
				// Check for errors..
			}
		}
	}
	Socket::Close();
}

void ChatServer::StartUp(){
	DEBUG_PRINT("ChatServer::StartUp()\n");
	FD_ZERO(&m_activeSockets);
	FD_ZERO(&m_socketsReadyForReading);

	m_roomIdIndex = 0;
	m_userIdIndex = 0;

	m_tv.tv_sec	= 0;
	m_tv.tv_usec	= 500 * 1000; // 500 milliseconds, half a second

	Socket::Initialize();

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
	DEBUG_PRINT("ChatServer::BroadcastMessage()\n");
	// Room lookup
	for (int i = 0; i < m_chatRooms.size(); i++) {
		if (m_chatRooms[i].id == roomID) {
			// Sends a message to each user
			for (int j = 0; j < m_chatRooms[i].users.size(); j++) {
				// User lookup
				for (int k = 0; k < m_chatUsers.size(); k++) {
					if (m_chatUsers[k].id == m_chatRooms[i].users[j]) {
						SendMessage(message, m_chatUsers[k].userSocket);
					}
				}
				
			}
		}
	}
}

void ChatServer::SendMessage(std::string message, SOCKET userID) {
	DEBUG_PRINT("ChatServer::SendMessage()\n");
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

void ChatServer::Read() {
	DEBUG_PRINT("ChatServer::Read()\n");
}

void ChatServer::Write() {
	DEBUG_PRINT("ChatServer::Write()\n");
}
