#pragma once
#include <string>
#include <map>

#include "Socket.h"

class ChatClient : public Socket {
public:
	short						 m_id;			// User ID
	std::string					 m_name;		// User Name
	std::map<std::string, short> m_rooms;		// Rooms joined by the user

	void StartUp(const char* domain);
	void Shutdown();
	void Write(const char* message);

private:
	void Connect();
	void IOControl();
};

