#include "ChatMessageProtocol.h"

#include <sstream>
#include <vector>

Buffer* ChatMessageProtocol::ApplyProtocol(const std::string &message, const short& userID) {
    std::stringstream           ss(message);    // Stringstream from message parameter
    std::string                 item;           // String split part from stringstream
    std::stringstream           finalMessage;   // Final Message with Protocal Applied
    Buffer*                     bufferData;
    int                         finalBufLen;

    // Reads the first command of the message (msgtype)
    std::getline(ss, item, ' ');

    // Message type JOIN_SERVER
    if (item == "/name") {
        std::getline(ss, item, ' ');                // Reads the username
        finalBufLen = 6 + item.length();            // {int 4bytes | short 2bytes  | char* 1byte*lenght}
        bufferData = new Buffer(finalBufLen);       // {int buflen | short msgtype | char* username}
        bufferData->WriteInt32LE(finalBufLen);      // final buffer size
        bufferData->WriteShort16LE(JOIN_SERVER);    // msgtype
        bufferData->WriteStringLE(item);            // username

    } else if (item == "/quit") {   // Message type LEAVE_SERVER
        std::getline(ss, item, ' ');                // Reads the userid
        finalBufLen = 8;                            // {int 4bytes | short 2bytes  | short 2bytes}
        bufferData = new Buffer(finalBufLen);       // {int buflen | short msgtype | short userid}
        bufferData->WriteInt32LE(finalBufLen);      // final buffer size
        bufferData->WriteShort16LE(LEAVE_SERVER);   // msgtype
        bufferData->WriteShort16LE(userID); // userid

    } else if (item == "/join") { // Message type JOIN_ROOM
        std::getline(ss, item, ' ');                // Reads the room name
        finalBufLen = 6 + item.length();            // {int 4bytes | short 2bytes  | char* 1byte*lenght }
        bufferData = new Buffer(finalBufLen);       // {int buflen | short msgtype | char* roomname     }
        bufferData->WriteInt32LE(finalBufLen);      // final buffer size
        bufferData->WriteShort16LE(JOIN_ROOM);      // msgtype
        bufferData->WriteStringLE(item);            // roomname

    } else if (item == "/leave") { // Message type LEAVE_ROOM
        std::getline(ss, item, ' ');                // Reads the room name
        finalBufLen = 6 + item.length();            // {int 4bytes | short 2bytes  | char* 1byte*lenght}
        bufferData = new Buffer(finalBufLen);       // {int buflen | short msgtype | char* roomname}
        bufferData->WriteInt32LE(finalBufLen);      // final buffer size
        bufferData->WriteShort16LE(LEAVE_ROOM);     // msgtype
        bufferData->WriteStringLE(item);            // roomname

    }else if (item == "/m") { // Message type MESSAGE
        std::getline(ss, item, ' ');                // Reads the room name
        std::string roomname = item;
        //std::getline(ss, item, ' ');                // Reads the message
        std::getline(ss, item);
        finalBufLen = 6 + roomname.length() + 1 + item.length(); // {int 4bytes | short 2bytes  | char* 1byte*lenght | " " | char* 1byte*lenght}
        bufferData = new Buffer(finalBufLen);                    // {int buflen | short msgtype | char* roomname     | " " | char* message}
        bufferData->WriteInt32LE(finalBufLen);                   // buffer << final buffer size
        bufferData->WriteShort16LE(MESSAGE);                     // buffer << msgtype
        bufferData->WriteStringLE(roomname + " " + item);        // buffer << roomname + " " + message

    } else {
        return nullptr;
    }

    return bufferData;
}