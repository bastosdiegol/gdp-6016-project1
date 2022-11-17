#include "DBConnector.h"
#include "AuthServer.h"
#include "sha256.h"


int main(int argc, char** argv) {


	AuthServer authServer("5556");
	for(int i = 0; i < 50; i++)
		authServer.RegisterNewUser("","");

	SHA256 sha256;
	// hashing an std::string
	std::cout << sha256("Hello World") << std::endl;

	/*DBConnector* dbconn = new DBConnector();
	dbconn->Connect("authdb");

	authServer.StartUp();


	dbconn->Disconnect();*/
	authServer.Shutdown();
}