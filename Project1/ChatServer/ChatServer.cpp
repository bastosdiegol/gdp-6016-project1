#include "ChatServer.h"

void ChatServer::StartUp(){
	FD_ZERO(&m_activeSockets);
	FD_ZERO(&m_socketsReadyForReading);

	m_roomIdIndex = 0;
	m_userIdIndex = 0;

	m_tv.tv_sec	= 0;
	m_tv.tv_usec	= 500 * 1000; // 500 milliseconds, half a second

	Socket::Initialize();
}

void ChatServer::Shutdown(){
	Socket::Close();
}

short ChatServer::JoinServer(std::string name, SOCKET userSocket) {
	// Generates new ID for the user
	short userIndex = m_userIdIndex;
	// Adds the new user
	m_chatUsers.push_back(ChatUser{ userIndex, name, userSocket });
	// Increments the user id index
	m_userIdIndex++;
	// Returns the id of the new user
	return userIndex;
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
	// Tries to remove the user from each room
	int numRooms = m_chatRooms.size() - 1;  // We gonna use an index since we could be removing rooms
	for (int i = numRooms; i >= 0; i--) {	// Decrescent since we could be removing rooms
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
	// Room lookup
	for (int i = 0; i < m_chatRooms.size(); i++) {
		if (m_chatRooms[i].id == roomID) {
			// Is this user the last one?
			if (m_chatRooms[i].users.size() == 1) {
				// Remove the user
				m_chatRooms[i].users.pop_back();
				// Remove the room
				m_chatRooms.erase(m_chatRooms.begin()+i);
			}
			else {
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
