#pragma once
#include "Socket.h"
#include "duthomhas/csprng.h"
#include "sha256.h"
#include "DBConnector.h"

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
	CSPRNG					rng;
	SHA256					sha256;
	DBConnector*			dbconn;

	AuthServer(const char* port) : Socket::Socket(port) {}

	void StartUp();
	void Shutdown();

	/// <summary>
	/// Creates a RNG salt and appends it to the password before Hashing
	/// Then tries to insert the new user to the databse
	/// </summary>
	/// <param name="email">Receives the user email</param>
	/// <param name="password">Receives the unhashed user password</param>
	/// <returns>True if Database insertion was OK</returns>
	bool RegisterNewUser(std::string email, std::string password);
	/// <summary>
	/// Locates the user salt on the database and re-hash its password
	/// Compare if the hashed password matches with the one stored on the db
	/// </summary>
	/// <param name="email">Receives user email</param>
	/// <param name="password">Receives unhashed user password</param>
	/// <returns>The userid. -1 if no password matches</returns>
	int AuthenticateUser(std::string email, std::string password);
};

