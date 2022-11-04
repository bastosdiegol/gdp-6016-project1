#include "DBConnector.h"


int main(int argc, char** argv) {
	DBConnector* dbconn = new DBConnector();
	dbconn->Connect("authdb");
	dbconn->Disconnect();
}