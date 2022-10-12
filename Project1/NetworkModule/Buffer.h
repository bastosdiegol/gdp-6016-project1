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

	// Stores the index of Write and Read Operations
	size_t m_WriteBufferIndex;
	size_t m_ReadBufferIndex;

	// Accepts Index and Value
	// Writes the LE Int32 value on the specified index
	void WriteInt32LE(size_t index, int32_t value);
	// Accepts Value
	// Writes the LE Int32 value on the latest write Index
	void WriteInt32LE(int32_t value);
	// Accepts Value
	// Writes the LE Short16 value on the latest write Index
	void WriteShort16LE(int16_t value);
	// Accepts Value
	// Writes the LE String value on the latest write Index
	void WriteStringLE(std::string value);
	// Accepts Value
	// Writes the BE Int32 value on the latest write Index
	void WriteInt32BE(int32_t value);
	// Accepts Value
	// Writes the BE Short16 value on the latest write Index
	void WriteShort16BE(int16_t value);
	// Accepts Value
	// Writes the BE String value on the latest write Index
	void WriteStringBE(std::string value);

	// Accepts Index
	// Reads the LE Int32 value on the latest Read Index
	// Returns the Int32
	uint32_t ReadUInt32LE(size_t index);
	// Accepts nothing
	// Reads the LE Int32 value on the latest Read Index
	// Returns the Int32
	uint32_t ReadUInt32LE();
	// Accepts nothing
	// Reads the LE Short16 value on the latest Read Index
	// Returns the Short16
	int16_t	ReadShort16LE();
	// Accepts String size
	// Reads the LE String value on the latest Read Index
	// Returns the String
	std::string ReadStringLE(size_t lenght);
	// Accepts nothing
	// Reads the BE Int32 value on the latest Read Index
	// Returns the Int32
	uint32_t ReadUInt32BE();
	// Accepts nothing
	// Reads the BE Short16 value on the latest Read Index
	// Returns the Short16
	int16_t	ReadShort16BE();
	// Accepts String size
	// Reads the BE String value on the latest Read Index
	// Returns the String
	std::string ReadStringBE(size_t lenght);

private:
};
