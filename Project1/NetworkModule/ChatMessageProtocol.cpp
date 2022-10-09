#include "ChatMessageProtocol.h"

#include <sstream>
#include <vector>

std::string ChatMessageProtocol::ApplyProtocol(const std::string &message, const short& userID) {
    std::stringstream           ss(message);    // Stringstream from message parameter
    std::string                 item;           // String split part from stringstream
    std::stringstream           finalMessage;   // Final Message with Protocal Applied

    // Gets the first command of the message
    std::getline(ss, item, ' ');

    // Message type JOIN_SERVER
    if (item == "/name") {
        finalMessage << JOIN_SERVER;
        finalMessage << ',';
        std::getline(ss, item, ' ');
        finalMessage << item;           // User name
    }
    // Message type LEAVE_SERVER
    if (item == "/quit") {
        finalMessage << LEAVE_SERVER;
        finalMessage << ',';
        finalMessage << userID;
    }
    // Message type JOIN_ROOM
    if (item == "/enter") {
        finalMessage << JOIN_ROOM;
        finalMessage << ',';
        finalMessage << userID;
        finalMessage << ',';
        std::getline(ss, item, ' ');
        finalMessage << item;           // Room Name
    }
    // Message type LEAVE_ROOM
    if (item == "/leave") {
        finalMessage << LEAVE_ROOM;
        finalMessage << ',';
        finalMessage << userID;
        finalMessage << ',';
        std::getline(ss, item, ' ');
        finalMessage << item;           // Room ID
    }
    // Message type MESSAGE
    if (item == "/m") {
        finalMessage << MESSAGE;
        finalMessage << ',';
        finalMessage << userID;
        finalMessage << ',';
        std::getline(ss, item);
        finalMessage << item;           // Complete Message
    }

    return finalMessage.str();
}
