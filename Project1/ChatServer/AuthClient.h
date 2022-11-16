#pragma once
#include <Socket.h>
class AuthClient :
    public Socket {
public:
	AuthClient(const char* port = "5556") : Socket::Socket(port) {}

	void StartUp(const char* domain);
	void Shutdown();
	void Write(const char* buf, int buflen);

private:
	void Connect();
	void IOControl();
};

