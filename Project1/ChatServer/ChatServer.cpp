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

short ChatServer::JoinServer(std::string name, SOCKET userSocket){
	// Creates the new user Pair
	ChatUser newUser = std::make_pair(name, userSocket);
	// Gets the number of chat users
	short numUsers = m_chatUsers.size();
	// Adds the new user
	m_chatUsers.try_emplace(numUsers, newUser);
	// Returns the id of the new user
	return numUsers;
}

short ChatServer::JoinRoom(std::string name, short userID){
	// Tries to find the Chat Room by name
	const std::map<std::string, short>::iterator it = m_chatRooms.find(name);
	// Checks if Room with same name was found
	if (it == m_chatRooms.end()) {
		// Number of rooms
		short numRooms = m_chatRooms.size();
		// Creates the new room
		m_chatRooms.try_emplace(name, numRooms);
		// Sets the User on the new room
		RoomUserId newPair = std::make_pair(numRooms, userID);
		m_chatRoomUsers.push_back(newPair);
		// Return the room id
		return numRooms;
	}
	else {
		// Adds the user on the already existing room
		RoomUserId newPair = std::make_pair(it->second, userID);
		m_chatRoomUsers.push_back(newPair);
		// Room already exists, return ChatID
		return it->second;
	}
}
