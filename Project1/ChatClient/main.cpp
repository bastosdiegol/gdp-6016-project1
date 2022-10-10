#include "ChatClient.h"
#include "Buffer.h"
#include "ChatMessageProtocol.h"

#include <iostream>

int main(int argc, char** argv) {
	ChatClient			cc;
	Buffer*				theBuffer;
	ChatMessageProtocol cmp;

	std::cout << "Enter your username: ";
	std::cin >> cc.m_name;
	theBuffer = cmp.ApplyProtocol("/name " + cc.m_name, cc.m_id);

	cc.StartUp("127.0.0.1");

	cc.Write((char*)&theBuffer->m_BufferData, theBuffer->m_BufferSize);
	system("pause");
	cc.Shutdown();

}