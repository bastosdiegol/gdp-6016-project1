#pragma once

#include <vector>
#include <string>

class Buffer {
public:
	// call m_Buffer.resize in your constructor
	std::vector<uint8_t> m_BufferData;
	// Stores the size of the buffer
	size_t m_BufferSize;

	// Default Constructor
	Buffer(size_t size);

	// Accepts Index and Value
	// Writes the value on the specified index
	void WriteInt32LE(size_t index, int32_t value);
	// Accepts Value
	// Writes the value on the latest Write Index
	void WriteInt32LE(int32_t value);
	void WriteShort16LE(int16_t value);
	void WriteStringLE(std::string value);

	void WriteInt32BE(int32_t value);
	void WriteShort16BE(int16_t value);
	void WriteString8BE(int8_t* value, int lenght);

	// Accepts Index
	// Reads the value on the specified index
	// Returns the value
	uint32_t ReadUInt32LE(size_t index);
	// Accepts nothing
	// Reads the value on the latest Read Index
	// Returns the value
	uint32_t ReadUInt32LE();
	int16_t	ReadShort16LE();
	std::string ReadStringLE(size_t lenght);

	uint32_t ReadUInt32BE();
	int16_t	ReadShort16BE();
	int8_t* ReadString8BE();

private:
	// Stores the index of Write and Read Operations
	size_t m_WriteBufferIndex;
	size_t m_ReadBufferIndex;
};
