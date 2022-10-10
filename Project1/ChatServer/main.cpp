#include "ChatServer.h"
#include "ChatMessageProtocol.h"
#include "Buffer.h"
#include <iostream>

int main(int argc, char** argv) {

	//ChatMessageProtocol cmp;
	////cmp.ApplyProtocol("/m My new message! Hello!");
	//cmp.ApplyProtocol("/name Diego", 0);

	//Buffer b1(6);
	//b1.WriteInt32LE(500);		// 00110010
	//b1.WriteShort16LE(16);	// 00000001 11110100	
	//b1.WriteStringLE("test");

	//int readValue500 = b1.ReadUInt32LE();
	//printf("readValue500: %d\n", readValue500);

	//short readValue16 = b1.ReadShort16LE();
	//printf("readValue16: %d\n", readValue16);

	//std::string readValuetest = b1.ReadStringLE(4);
	//std::cout << "readValuetest: " << readValuetest << std::endl;

	//b1.WriteInt32LE(10);		// 00110010
	//b1.WriteInt32LE(13, 100);		// 00000001 11110100	

	//int readValue10 = b1.ReadUInt32LE();
	//int readValue100 = b1.ReadUInt32LE(13);

	ChatServer cs;
	cs.StartUp();
	cs.Shutdown();

	//SOCKET a = NULL;
	//cs.JoinServer("A", a);
	//cs.JoinServer("B", a);
	//cs.JoinServer("C", a);
	//cs.JoinServer("D", a);
	//cs.JoinServer("E", a);

	//cs.JoinRoom("Room A", 0);
	//cs.JoinRoom("Room A", 1);
	//cs.JoinRoom("Room A", 2);

	//cs.JoinRoom("Room B", 1);
	////cs.JoinRoom("Room B", 2);
	////cs.JoinRoom("Room B", 3);
	////cs.JoinRoom("Room B", 4);

	//cs.JoinRoom("Room C", 2);
	//cs.JoinRoom("Room C", 3);
	//cs.JoinRoom("Room C", 4);

	//cs.JoinRoom("Room B", 0);

	////cs.LeaveRoom(1, 1);
	////cs.LeaveRoom(0, 1);

	////cs.JoinRoom("Room B", 1);

	//std::cout << cs.ListRooms() << std::endl;

	//cs.LeaveServer(0);
	//cs.LeaveServer(1);
	//cs.LeaveServer(2);
	//cs.LeaveServer(3);
	//cs.LeaveServer(4);

	//std::cout << cs.ListRooms() << std::endl;
	
	//cs.Shutdown();
	return 0;
}