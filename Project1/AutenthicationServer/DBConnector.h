#pragma once
#include <mysql/jdbc.h>
#include <vector>

class DBConnector {
public:
	DBConnector();
	~DBConnector();

	bool Connect(std::string schema);
	void Disconnect();

	bool insertUser(std::string email, std::string salt, std::string password);
	int authenticateUser(std::string email, std::string password);
	std::string findUserSalt(std::string email);

private:
	sql::Driver*			m_pDriver;
	sql::Connection*		m_pConnection;
	sql::ResultSet*			m_pResultSet;
	sql::Statement*			m_pStatement;
	sql::PreparedStatement* m_InsertUserProcedure;
	sql::PreparedStatement* m_FindUserAuthDataStatement;
	sql::PreparedStatement* m_UpdateLastLoginStatement;
	sql::PreparedStatement* m_AuthenticateUserStatement;
};

