#include "DBConnector.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

DBConnector::DBConnector() {
	DEBUG_PRINT("DBConnector::DBConnector()");
	m_pDriver					= nullptr;
	m_pConnection				= nullptr;
	m_pResultSet				= nullptr;
	m_pStatement				= nullptr;
	m_InsertUserStatement		= nullptr;
	m_InsertWebAuthStatement	= nullptr;
	m_FindUserAuthDataStatement = nullptr;
	m_UpdateLastLoginStatement	= nullptr;
}

DBConnector::~DBConnector() {
	DEBUG_PRINT("DBConnector::~DBConnector()");
	Disconnect();
}

bool DBConnector::Connect(std::string schema) {
	DEBUG_PRINT("DBConnector::Connect(%s)", schema);
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
		m_InsertUserStatement = m_pConnection->prepareStatement(
			"INSERT INTO `authdb`.`user` SET `creation_date` = CURDATE();");
		m_InsertWebAuthStatement = m_pConnection->prepareStatement(
			"INSERT INTO `authdb`.`web_auth` (`email`, `salt`, `hashed_password`, `user_id`) VALUES (?, ?, ?, ?);");
		m_FindUserAuthDataStatement = m_pConnection->prepareStatement(
			"SELECT `web_auth`.`salt`, `web_auth`.`hashed_password`, `web_auth`.`user_id` FROM `authdb`.`web_auth` WHERE  `web_auth`.`email` = ?;");
		m_UpdateLastLoginStatement = m_pConnection->prepareStatement(
			"UPDATE `authdb`.`user` SET `last_login` = ? WHERE `id` = ?;");
	}
	catch (sql::SQLException e) {
		printf("Failed to create statements: %s\n", e.what());
		return false;
	}

	return true;
}

void DBConnector::Disconnect() {
	DEBUG_PRINT("DBConnector::Disconnect()");
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
	delete m_InsertUserStatement;
	delete m_InsertWebAuthStatement;
	delete m_FindUserAuthDataStatement;
	delete m_UpdateLastLoginStatement;
}

sql::ResultSet* DBConnector::insertUser(std::string email, std::string salt, std::string password) {
	DEBUG_PRINT("DBConnector::insertUser(%s, %s, %s)", email, salt, password);
	return nullptr;
}

sql::ResultSet* DBConnector::findUserAuthData(std::string email) {
	DEBUG_PRINT("DBConnector::findUserAuthData(%s)", email);
	return nullptr;
}
