#pragma once
#include <string>
#include <vector>

#include "AuthClient.h"
#include "Socket.h"
#include "Buffer.h"
#include "ChatMessageProtocol.h"

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
private:
	bool m_serverStatus;	// While True server LifeCycle should be alive

	// Server main Function after StartUp();
	void LifeCycle();	
	// Checks if theres a new Chat User trying to connect to the server
	void Accept();
public:
	//fd_set					m_activeSockets;
	fd_set					m_socketsReadyForReading;
	struct timeval			m_tv;

	short					m_roomIdIndex;
	std::vector<ChatRoom>	m_chatRooms;
	short					m_userIdIndex;
	std::vector<ChatUser>	m_chatUsers;

	AuthClient				m_authClient;

	void StartUp();
	void Shutdown();

	// User sends his username to the server after the the Accept()
	// Looksup for the userSocket already registered and saves his username
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
	// List all rooms available
	// Returns a string containing all information available
	std::string ListRooms();
	// Sends a message submited by a user to all users of the same channel
	void BroadcastMessage(short roomID, std::string message);
	/// <summary>
	/// Sends a protoMessage to Auth Server
	/// To register a new user
	/// </summary>
	/// <param name="email">Receives user email</param>
	/// <param name="password">Receives user password</param>
	/// <returns>Returns True if user was registred</returns>
	bool RegisterNewUser(ChatUser* user, std::string email, std::string password);
	/// <summary>
	/// Sends a protoMessage to Auth Server
	/// To authenticate a user
	/// </summary>
	/// <param name="email">Receives user email</param>
	/// <param name="password">Receives user password</param>
	/// <returns>Returns true if user is authenticated</returns>
	bool AuthenticateUser(ChatUser* user, std::string email, std::string password);
};

