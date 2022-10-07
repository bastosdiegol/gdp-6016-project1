#include "ChatServer.h"

int main(int argc, char** argv) {
	ChatServer cs;
	cs.StartUp();

	SOCKET a = NULL;
	cs.JoinServer("A", a);
	cs.JoinServer("B", a);
	cs.JoinServer("C", a);
	cs.JoinServer("D", a);
	cs.JoinServer("E", a);

	cs.JoinRoom("Room A", 0);
	cs.JoinRoom("Room A", 1);
	cs.JoinRoom("Room A", 2);

	cs.JoinRoom("Room B", 1);
	cs.JoinRoom("Room B", 2);
	cs.JoinRoom("Room B", 3);
	cs.JoinRoom("Room B", 4);

	cs.JoinRoom("Room C", 2);
	cs.JoinRoom("Room C", 3);
	cs.JoinRoom("Room C", 4);

	cs.JoinRoom("Room B", 0);

	cs.Shutdown();
	return 0;
}