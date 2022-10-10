#include "ChatClient.h"
#include <iostream>

int main(int argc, char** argv) {
	ChatClient cc;
	std::cout << "Enter your username: ";
//	std::cin >> cc.m_name;

	cc.StartUp("127.0.0.1");
	system("pause");
	cc.Shutdown();

}