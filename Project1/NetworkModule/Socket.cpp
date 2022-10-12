#include "Socket.h"

//#define DEBUG_LOG_ENABLED
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

Socket::Socket(){
	DEBUG_PRINT("Socket::Socket()\n");
	m_port = (char*)DEFAULT_PORT;
	info = nullptr;
	m_socket = INVALID_SOCKET;

	// Initialize Winsock
	m_err = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	// Checks WSAStartup
	if (m_err != 0) {
		throw "WSAStartup failed with error %d\n", m_err;
	}
	// Confirms that the WinSock DLL supports 2.2
	// Suggestion from https://learn.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-wsadata
	if (LOBYTE(m_wsaData.wVersion) != 2 ||
		HIBYTE(m_wsaData.wVersion) != 2) {
		WSACleanup();
		throw "WSAStartup could not find an usable WinSock DLL %d\n", m_err;
	}
}

Socket::~Socket(){
	DEBUG_PRINT("Socket::~Socket()\n");
}

void Socket::ServerAddrinfoSetup() {
	// Prepare addrinfo structure
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;		// IPV4
	hints.ai_socktype = SOCK_STREAM;	// Stream
	hints.ai_protocol = IPPROTO_TCP;	// TCP
	hints.ai_flags = AI_PASSIVE;

	// Creating addrinfo
	m_err = getaddrinfo(NULL, m_port, &hints, &info);
	if (m_err != 0) {
		WSACleanup();
		throw "getaddrinfo() failed with error: %d\n", m_err;
	}
}

void Socket::ClientAddrinfoSetup(const char* domain) {
	// Prepare addrinfo structure
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;		// IPV4
	hints.ai_socktype = SOCK_STREAM;	// Stream
	hints.ai_protocol = IPPROTO_TCP;	// TCP

	// Creating addrinfo
	m_err = getaddrinfo(domain, m_port, &hints, &info);
	if (m_err != 0) {
		WSACleanup();
		throw "getaddrinfo() failed with error: %d\n", m_err;
	}
}

void Socket::Open(){
	DEBUG_PRINT("Socket::Open()\n");
	// Socket Creation
	m_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (m_socket == INVALID_SOCKET) {
		std::string error = "Socket failed with error: %d\n" + WSAGetLastError();
		freeaddrinfo(info);
		WSACleanup();
		throw error;
	}
}

void Socket::Bind(){
	DEBUG_PRINT("Socket::Bind()\n");
	// 12,14,1,3:80				Address lengths can be different
	// 123,111,230,109:55555	Must specify the length
	// Bind our socket [Bind]
	m_err = bind(m_socket, info->ai_addr, (int)info->ai_addrlen);
	if (m_err == SOCKET_ERROR) {
		std::string error = "Bind failed with error: %d\n" + WSAGetLastError();
		freeaddrinfo(info);
		closesocket(m_socket);
		WSACleanup();
		throw error;
	}
}

void Socket::Listen(){
	DEBUG_PRINT("Socket::Listen()\n");
	// Listen
	m_err = listen(m_socket, SOMAXCONN);
	if (m_err == SOCKET_ERROR) {
		std::string error = "listen failed with error: %d\n" + WSAGetLastError();
		freeaddrinfo(info);
		closesocket(m_socket);
		WSACleanup();
		throw error;
	}
}

//void Socket::Accept(){}

//void Socket::Read(){}

//void Socket::Write(){}

void Socket::Close(){
	DEBUG_PRINT("Socket::Close()\n");
	freeaddrinfo(info);
	closesocket(m_socket);
	WSACleanup();
}
