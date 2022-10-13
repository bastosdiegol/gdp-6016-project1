#pragma once
#include <string>
#include <vector>
#include <map>

#include "Socket.h"

class ChatClient : public Socket {
public:

	short						 m_id;			// User ID
	std::string					 m_name;		// User Name
//	std::map<std::string, short> m_rooms;		// Rooms joined by the user
	std::vector<std::string>	 m_rooms;


	void StartUp(const char* domain);
	void Shutdown();
	void Write(const char* buf, int buflen);

private:
	void Connect();
	void IOControl();
};

