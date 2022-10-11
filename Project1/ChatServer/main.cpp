#include "ChatServer.h"
#include "ChatMessageProtocol.h"
#include "Buffer.h"
#include <iostream>

int main(int argc, char** argv) {

	ChatServer cs;
	cs.StartUp();
	cs.Shutdown();

	return 0;
}