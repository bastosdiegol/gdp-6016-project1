#include "DBConnector.h"

//#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
//#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

DBConnector::DBConnector() {
	DEBUG_PRINT("DBConnector::DBConnector()\n");
	m_pDriver					= nullptr;
	m_pConnection				= nullptr;
	m_pResultSet				= nullptr;
	m_pStatement				= nullptr;
	m_InsertUserProcedure		= nullptr;
	m_FindUserAuthDataStatement = nullptr;
	m_UpdateLastLoginStatement	= nullptr;
	m_AuthenticateUserStatement = nullptr;
}

DBConnector::~DBConnector() {
	DEBUG_PRINT("DBConnector::~DBConnector()\n");
	Disconnect();
}

bool DBConnector::Connect(std::string schema) {
	DEBUG_PRINT("DBConnector::Connect(%s)\n", schema);
	// Connection call here
	try {
		m_pDriver = sql::mysql::get_driver_instance();
	}
	catch (sql::SQLException e) {
		printf("Failed to get_driver_instance: %s\n", e.what());
		return false;
	}
	printf("Successfully retrieved our ccp-conn-sql driver!\n");

	try {
		sql::SQLString hostName("127.0.0.1:3306");
		sql::SQLString username("root");
		sql::SQLString password("root");
		m_pConnection = m_pDriver->connect(hostName, username, password);
		m_pConnection->setSchema(schema);
	}
	catch (sql::SQLException e) {
		printf("Failed to connect to our database: %s\n", e.what());
		return false;
	}
	printf("Successfully connected to our Database & Schema!\n");

	try {
		m_pStatement = m_pConnection->createStatement();
		m_InsertUserProcedure = m_pConnection->prepareStatement("CALL add_user(?, ?, ?)");
		m_FindUserAuthDataStatement = m_pConnection->prepareStatement(
			"SELECT `web_auth`.`salt`, `web_auth`.`hashed_password`, `web_auth`.`user_id` FROM `authdb`.`web_auth` WHERE  `web_auth`.`email` = ?;");
		m_UpdateLastLoginStatement = m_pConnection->prepareStatement(
			"UPDATE `authdb`.`user` SET `last_login` = NOW() WHERE `id` = ?;");
		m_AuthenticateUserStatement = m_pConnection->prepareStatement(
			"SELECT `web_auth`.`user_id` FROM `authdb`.`web_auth` WHERE  `web_auth`.`email` = ? AND `web_auth`.`hashed_password` = ?;");
	}
	catch (sql::SQLException e) {
		printf("Failed to create statements: %s\n", e.what());
		return false;
	}

	return true;
}

void DBConnector::Disconnect() {
	DEBUG_PRINT("DBConnector::Disconnect()\n");
	try {
		m_pConnection->close();
	}
	catch (sql::SQLException e) {
		printf("Failed to close the connection to our database: %s\n", e.what());
		return;
	}
	printf("Successfully closed the connection to our Database!\n");

	delete m_pStatement;
	delete m_pResultSet;
	delete m_InsertUserProcedure;
	delete m_FindUserAuthDataStatement;
	delete m_UpdateLastLoginStatement;
	delete m_AuthenticateUserStatement;
}

bool DBConnector::insertUser(std::string email, std::string salt, std::string password) {
	DEBUG_PRINT("DBConnector::insertUser(%s, %s, %s)\n", email.c_str(), salt.c_str(), password.c_str());

	try {
		// Tries to find the user
		m_FindUserAuthDataStatement->setString(1, email);
		m_pResultSet = m_FindUserAuthDataStatement->executeQuery();
		DEBUG_PRINT("rowsCount() find result %d\n", (int)m_pResultSet->rowsCount());

		// Checks if user was found
		if (m_pResultSet->next()) {
			// User found
			int id = m_pResultSet->getInt("user_id");
			DEBUG_PRINT("User found, user_id: %d", id);
			return false;
		} else {
			// User not found - Insert it
			m_InsertUserProcedure->setString(1, email);
			m_InsertUserProcedure->setString(2, salt);
			m_InsertUserProcedure->setString(3, password);
			m_InsertUserProcedure->execute();
			return true;
		}
	}
	catch (sql::SQLException e) {
		DEBUG_PRINT("Failed to add a user to our database: %s\n", e.what());
		return false;
	}
}

int DBConnector::authenticateUser(std::string email, std::string password) {
	DEBUG_PRINT("DBConnector::authenticateUser(%s, %s)\n", email.c_str(), password.c_str());
	try {
		// Try to find the user by email with specified hashed password
		m_AuthenticateUserStatement->setString(1, email);
		m_AuthenticateUserStatement->setString(2, password);
		m_pResultSet = m_AuthenticateUserStatement->executeQuery();
		DEBUG_PRINT("rowsCount() find result %d\n", (int)m_pResultSet->rowsCount());

		// Checks if user was found
		if (m_pResultSet->next()) {
			// Grabs the userid
			int userid = m_pResultSet->getInt("user_id");
			// Updates the Last login
			m_UpdateLastLoginStatement->setInt(1, userid);
			m_UpdateLastLoginStatement->execute();
			// Returns the userid
			return userid;
		} else {
			// User not found, returns -1
			return -1;
		}
	}
	catch (sql::SQLException e) {
		DEBUG_PRINT("Failed to authenticate a user: %s\n", e.what());
		return -1;
	}
}

std::string DBConnector::findUserSalt(std::string email) {
	DEBUG_PRINT("DBConnector::findUserAuthData(%s)\n", email);
	try {
		// Tries to find the user
		m_FindUserAuthDataStatement->setString(1, email);
		m_pResultSet = m_FindUserAuthDataStatement->executeQuery();
		DEBUG_PRINT("rowsCount() find result %d\n", (int)m_pResultSet->rowsCount());

		// Checks if user was found
		if (m_pResultSet->next()) {
			// User found
			std::string salt = m_pResultSet->getString("salt");
			return salt;
		} else {
			return "";
		}
	}
	catch (sql::SQLException e) {
		DEBUG_PRINT("Failed to find user salt: %s\n", e.what());
		return "";
	}
}

int DBConnector::findUserId(std::string email) {
	DEBUG_PRINT("DBConnector::findUserId(%s)\n", email.c_str());
	try {
		// Try to find the user by email with specified hashed password
		m_FindUserAuthDataStatement->setString(1, email);
		m_pResultSet = m_FindUserAuthDataStatement->executeQuery();
		DEBUG_PRINT("rowsCount() find result %d\n", (int)m_pResultSet->rowsCount());

		// Checks if user was found
		if (m_pResultSet->next()) {
			// Grabs the userid
			int userid = m_pResultSet->getInt("user_id");
			// Returns the userid
			return userid;
		} else {
			// User not found, returns -1
			return -1;
		}
	}
	catch (sql::SQLException e) {
		DEBUG_PRINT("Failed to find a user on our database: %s\n", e.what());
		return -1;
	}
}
