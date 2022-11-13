#pragma once
#include <string>

#include "Buffer.h"

typedef short MESSAGE_TYPE;

class ChatMessageProtocol {
public:
	int				m_lenght;
	MESSAGE_TYPE	m_msgType;
	short			m_senderID;
	short			m_roomID;
	std::string		m_message;

	const MESSAGE_TYPE JOIN_SERVER	= 0;
	const MESSAGE_TYPE LEAVE_SERVER	= 1;
	const MESSAGE_TYPE JOIN_ROOM	= 2;
	const MESSAGE_TYPE LEAVE_ROOM	= 3;
	const MESSAGE_TYPE MESSAGE		= 4;
	const MESSAGE_TYPE LOGIN		= 5;
	const MESSAGE_TYPE REGISTER		= 6;

	ChatMessageProtocol() : m_lenght(0)
							, m_msgType(-1)
							, m_senderID(-1)
							, m_roomID(-1)
							, m_message(""){
	};

	Buffer* ApplyProtocol(const std::string &message, const short &userID);
	Buffer* DecodeProtocol(char* buf, int buflen);
};

