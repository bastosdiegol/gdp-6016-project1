#include "DBConnector.h"
#include "AuthServer.h"


int main(int argc, char** argv) {

	AuthServer authServer;
	DBConnector* dbconn = new DBConnector();
	dbconn->Connect("authdb");


	authServer.StartUp();


	dbconn->Disconnect();
	authServer.Shutdown();
}