#include "DBConnector.h"
#include "AuthServer.h"
#include "sha256.h"
#include "duthomhas/csprng.h"


int main(int argc, char** argv) {


	AuthServer authServer("5556");

	//CSPRNG rng = csprng_create();
	//if (!rng) {
	//	return 1;
	//}
	//long salt = csprng_get_int(rng);
	//for(int i = 0; i < 50; i++)
	//	authServer.RegisterNewUser("","");
	//SHA256 sha256;
	// hashing an std::string
	//std::cout << sha256("Hello World") << std::endl;
	//DBConnector* dbconn = new DBConnector();
	//dbconn->Connect("authdb");
	//std::string newPassword = std::to_string(salt).append("123456");
	//std::string hashedPasswordToString = sha256(newPassword);
	//
	//std::cout << "newPassword " << newPassword << std::endl;
	//std::cout << "hashedPasswordToString " << hashedPasswordToString << std::endl;
	//
	//dbconn->insertUser("email1@email.com", std::to_string(salt), hashedPasswordToString);
	//int userid = dbconn->authenticateUser("email1@email.com", "04bc99ce694162c7c33864f3a732c2bf72acffdc14b19b2244cc3c2a2fd025f9");
	//std::string dbsalt = dbconn->findUserSalt("email1@email.com");
	//std::cout << "User salt: " << dbsalt << std::endl;

	authServer.StartUp();


	//dbconn->Disconnect();
	authServer.Shutdown();
}