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
        bufferData->WriteShort16LE((short)std::stoi(item)); // userid

    } else if (item == "/enter") { // Message type JOIN_ROOM
        std::getline(ss, item, ' ');                // Reads the room name
        finalBufLen = 8 + item.length();            // {int 4bytes | short 2bytes  | char* 1byte*lenght | short 2bytes}
        bufferData = new Buffer(finalBufLen);       // {int buflen | short msgtype | char* roomname     | short userid}
        bufferData->WriteInt32LE(finalBufLen);      // final buffer size
        bufferData->WriteShort16LE(JOIN_ROOM);      // msgtype
        bufferData->WriteStringLE(item);            // roomname
        std::getline(ss, item, ' ');                // Reads the userid
        bufferData->WriteShort16LE((short)std::stoi(item)); // userid

    } else if (item == "/leave") { // Message type LEAVE_ROOM
        std::getline(ss, item, ' ');                // Reads the roomid
        finalBufLen = 10;                           // {int 4bytes | short 2bytes  | short 2bytes | short 2bytes}
        bufferData = new Buffer(finalBufLen);       // {int buflen | short msgtype | short roomid | short userid}
        bufferData->WriteInt32LE(finalBufLen);      // final buffer size
        bufferData->WriteShort16LE(LEAVE_ROOM);     // msgtype
        std::getline(ss, item, ' ');                // Reads the roomid
        bufferData->WriteShort16LE((short)std::stoi(item)); // roomdid
        std::getline(ss, item, ' ');                // Reads the userid
        bufferData->WriteShort16LE((short)std::stoi(item)); // userid

    }else if (item == "/m") { // Message type MESSAGE
        std::getline(ss, item, ' ');                // Reads the roomid
        short roomid = (short)std::stoi(item);
        std::getline(ss, item, ' ');                // Reads the userid
        short userid = (short)std::stoi(item);
        std::getline(ss, item, ' ');                // Reads the message
        finalBufLen = 10 + item.length();           // {int 4bytes | short 2bytes  | short 2bytes | short 2bytes  | char* 1byte*lenght}
        bufferData = new Buffer(finalBufLen);       // {int buflen | short msgtype | short roomid | short userid  | char* message}
        bufferData->WriteInt32LE(finalBufLen);      // buffer << final buffer size
        bufferData->WriteShort16LE(MESSAGE);        // buffer << msgtype
        bufferData->WriteShort16LE(roomid);         // buffer << roomdid
        bufferData->WriteShort16LE(userid);         // buffer << userid
        bufferData->WriteStringLE(item);            // buffer << message

    } else {
        throw "Message not typified.";
    }

    return bufferData;
}