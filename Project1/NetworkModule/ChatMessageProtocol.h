#pragma once
#include <string>

typedef short MESSAGE_TYPE;
#define JOIN_SERVER		0;
#define LEAVE_SERVER	1;
#define JOIN_ROOM		2;
#define LEAVE_ROOM		4;
#define MESSAGE			5;

class ChatMessageProtocol {
public:
	int				m_lenght;
	MESSAGE_TYPE	m_msgType;
	short			m_senderID;
	short			m_roomID;
	std::string		m_message;

	ChatMessageProtocol() : m_lenght(0)
							, m_msgType(-1)
							, m_senderID(-1)
							, m_roomID(-1)
							, m_message(""){
	};

	std::string ApplyProtocol(const std::string &message, const short &userID);
};

