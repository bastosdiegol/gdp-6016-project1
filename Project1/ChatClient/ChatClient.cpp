#include "ChatClient.h"

#define DEBUG_LOG_ENABLED
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

void ChatClient::StartUp(const char* domain) {
	ClientAddrinfoSetup(domain);
	Open();
	Connect();
	IOControl();
}

void ChatClient::Shutdown() {
	m_err = shutdown(m_socket, SD_SEND);
	if (m_err == SOCKET_ERROR) {
		std::string error = "Shutdown failed with error: %d\n" + WSAGetLastError();
		closesocket(m_socket);
		WSACleanup();
		throw error;
	}

	closesocket(m_socket);
	WSACleanup();
}

void ChatClient::Write(const char* buf, int buflen) {
	DEBUG_PRINT("ChatClient::Write()\n");

	m_err = send(m_socket, buf, buflen, 0);
	if (m_err == SOCKET_ERROR) {
		std::string error = "Send failed with error: %d\n" + WSAGetLastError();
		closesocket(m_socket);
		WSACleanup();
		throw error;
	}
}

void ChatClient::Connect() {
	DEBUG_PRINT("ChatClient::Connect()\n");

	m_err = connect(m_socket, info->ai_addr, (int)info->ai_addrlen);
	if (m_err == SOCKET_ERROR) {
		std::string error = "Failed to connect to the server with error: %d\n" + WSAGetLastError();
		closesocket(m_socket);
		freeaddrinfo(info);
		WSACleanup();
		throw error;
	}
}

void ChatClient::IOControl() {
	// input output control socket
	DWORD NonBlock = 1;
	m_err = ioctlsocket(m_socket, FIONBIO, &NonBlock);
	if (m_err == SOCKET_ERROR) {
		std::string error = "ioctlsocket to failed with error: %d\n" + WSAGetLastError();
		closesocket(m_socket);
		freeaddrinfo(info);
		WSACleanup();
		throw error;
	}
}
