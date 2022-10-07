#include "ChatServer.h"

void ChatServer::StartUp(){
	FD_ZERO(&m_activeSockets);
	FD_ZERO(&m_socketsReadyForReading);

	m_tv.tv_sec	= 0;
	m_tv.tv_usec	= 500 * 1000; // 500 milliseconds, half a second

	Socket::Initialize();
}

void ChatServer::Shutdown(){
	Socket::Close();
}

short ChatServer::JoinServer(std::string name, SOCKET userSocket) {
	// Gets the number of chat users
	short numUsers = m_chatUsers.size();
	// Adds the new user
	m_chatUsers.push_back(ChatUser{ numUsers, name, userSocket });
	// Returns the id of the new user
	return numUsers;
}

short ChatServer::JoinRoom(std::string name, short userID){
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
	}
	else {
		// Number of rooms
		roomIndex = m_chatRooms.size();
		// Creates the new room with the current user
		std::vector<short> users;
		users.push_back(userID);
		m_chatRooms.push_back(ChatRoom{ roomIndex
										, name
										, users });
		// Return the room id
		return roomIndex;
	}
}

void ChatServer::LeaveServer(short userID)
{
}

void ChatServer::LeaveRoom(short userID, short roomID){
	//for(int i=0; i<)
}
