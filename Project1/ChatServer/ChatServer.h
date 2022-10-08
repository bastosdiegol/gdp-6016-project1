#pragma once
#include <string>
#include <vector>

#include "Socket.h"

struct ChatUser {
	short		id;			// User ID
	std::string name;		// User Name
	SOCKET		userSocket; // Socket Utilized by this user
};

struct ChatRoom {
	short				id;		// ChatRoom ID
	std::string			name;	// ChatRoom Name
	std::vector<short>	users;	// Vector of UsersID of this ChatRoom
};

class ChatServer : public Socket {
public:
	fd_set					m_activeSockets;
	fd_set					m_socketsReadyForReading;
	struct timeval			m_tv;

	short					m_roomIdIndex;
	std::vector<ChatRoom>	m_chatRooms;
	short					m_userIdIndex;
	std::vector<ChatUser>	m_chatUsers;

	void StartUp();
	void Shutdown();

	// User joins the server
	// Returns the new UserID
	short JoinServer(std::string name, SOCKET userSocket);
	// User joins the room
	// Returns the RoomID, if Room doesn't exist creates it
	short JoinRoom(std::string name, short userID);
	// User leaves the server
	// User leaves all rooms
	void LeaveServer(short userID);
	// User leaves the room
	// If theres no user left removes the room
	void LeaveRoom(short userID, short roomID);
};

