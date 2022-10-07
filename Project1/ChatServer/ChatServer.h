#pragma once
#include <string>
#include <map>

#include "Socket.h"

typedef std::pair<std::string, SOCKET> ChatUser;

class ChatServer : public Socket {
public:
	fd_set					m_activeSockets;
	fd_set					m_socketsReadyForReading;
	struct timeval			m_tv;

	std::map<std::string, short>	m_chatRooms;	 // <ChatRoomName, ChatId>
	std::map<short, ChatUser>		m_chatUsers;	 // <UserId, <Username, SOCKET> >
	std::map<short, short>			m_chatRoomUsers; // <chatID, userID>
	

	void StartUp();
	void Shutdown();

	short JoinServer(std::string name, SOCKET userSocket);
	short JoinRoom(std::string name, short userID);
};

