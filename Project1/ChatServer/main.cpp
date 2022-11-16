#include "ChatServer.h"
#include "AuthClient.h"
#include "ChatMessageProtocol.h"
#include "authcomm.pb.h"
#include "Buffer.h"
#include <iostream>

void protoTest() {
	auth::ChatServerRequest serverRequest;
	serverRequest.set_type(auth::ChatServerRequest::AUTHENTICATE);
	serverRequest.set_id(1);
	serverRequest.set_email("bastosdiegol@gmail.com");
	serverRequest.set_password("123456");

	std::string serializedString;
	serverRequest.SerializeToString(&serializedString);

	std::cout << serializedString << std::endl;
	for (int idxString = 0; idxString < serializedString.length(); idxString++) {
		printf("%02X ", serializedString[idxString]);
	}
	printf("\n");

	auth::ChatServerRequest deserializedServerRequest;
	bool result = deserializedServerRequest.ParseFromString(serializedString);
	if (!result) {
		std::cout << "Failed to parse server request" << std::endl;
	}
	std::cout << "Parsing successful" << std::endl;
	std::cout << "Request Type: " << deserializedServerRequest.type() << std::endl;
	std::cout << "ID: " << deserializedServerRequest.id() << std::endl;
	std::cout << "E-mail: " << deserializedServerRequest.email() << std::endl;
	std::cout << "Password: " << deserializedServerRequest.password() << std::endl;
}

int main(int argc, char** argv) {

	//protoTest();

	ChatServer cs;
	AuthClient authClient("5556");

	cs.m_authClient = authClient;
	cs.m_authClient.StartUp("127.0.0.1");

	cs.StartUp();
	cs.Shutdown();

	return 0;
}