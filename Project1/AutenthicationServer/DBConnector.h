#pragma once
#include <mysql/jdbc.h>
#include <vector>

class DBConnector {
public:
	DBConnector();
	~DBConnector();

	bool Connect(std::string schema);
	void Disconnect();

	sql::ResultSet* insertUser(std::string email, std::string salt, std::string password);
	sql::ResultSet* findUserAuthData(std::string email);

private:
	sql::Driver*			m_pDriver;
	sql::Connection*		m_pConnection;
	sql::ResultSet*			m_pResultSet;
	sql::Statement*			m_pStatement;
	sql::PreparedStatement* m_InsertUserStatement;
	sql::PreparedStatement* m_InsertWebAuthStatement;
	sql::PreparedStatement* m_FindUserAuthDataStatement;
	sql::PreparedStatement* m_UpdateLastLoginStatement;
};

