#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>

#define DEFAULT_PORT "5555"

class Socket{
public:
	// Constructors
	Socket();
	// Destructor
	~Socket();

	// Closes the Socket
	void Close();

	SOCKET				m_socket;
protected:
	char*				m_port;				// Port (in case DEFAULT_PORT is overwritten)
	int					m_err;				// Functions Results will be store here - Error Check
	WSADATA				m_wsaData;
	struct addrinfo*	info;
	struct addrinfo		hints;

	// Sockets Internal Procedures
	void ServerAddrinfoSetup();
	void ClientAddrinfoSetup(const char* domain);
	void Open();
	void Bind();
	void Listen();
	void Accept();
	void Read();
	void Write();
};

