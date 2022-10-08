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

	// Call all procedures necessary to start-up a socket
	void Initialize();
	// Closes the Socket
	void Close();

protected:
	char*				m_port;				// Port (in case DEFAULT_PORT is overwritten)
	int					m_err;				// Functions Results will be store here - Error Check
	WSADATA				m_wsaData;
	struct addrinfo*	info;
	struct addrinfo		hints;
	SOCKET				m_socket;

	// Sockets Internal Procedures
	void Open();
	void Bind();
	void Listen();
	void Accept();
	void Read();
	void Write();
};

