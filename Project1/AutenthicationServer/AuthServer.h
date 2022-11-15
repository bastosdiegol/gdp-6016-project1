#pragma once
#include "Socket.h"

class AuthServer : public Socket {
private:
	bool m_serverStatus;	// While True server LifeCycle should be alive
	// Server main Function after StartUp();
	void LifeCycle();
	// Checks if theres a new Chat User trying to connect to the server
	void Accept();
public:
	fd_set					m_socketsReadyForReading;
	struct timeval			m_tv;
	SOCKET					m_chatServerSocket;

	void StartUp();
	void Shutdown();

	bool RegisterNewUser(std::string email, std::string password);
	bool AuthenticateUser(std::string email, std::string password);
};

