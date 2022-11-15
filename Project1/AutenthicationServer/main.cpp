#include "DBConnector.h"
#include "AuthServer.h"


int main(int argc, char** argv) {

	AuthServer authServer("5556");
	DBConnector* dbconn = new DBConnector();
	dbconn->Connect("authdb");


	authServer.StartUp();


	dbconn->Disconnect();
	authServer.Shutdown();
}