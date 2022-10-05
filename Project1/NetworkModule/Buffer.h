#pragma once

#include <vector>

class Buffer {
public:
	// Default Constructor
	Buffer(size_t size);

	// Accepts Index and Value
	// Writes the value on the specified index
	void WriteInt32LE(std::size_t index, int32_t value);
	// Accepts Value
	// Writes the value on the latest Write Index
	void WriteInt32LE(int32_t value);

	// Accepts Index
	// Reads the value on the specified index
	// Returns the value
	uint32_t ReadUInt32LE(std::size_t index);
	// Accepts nothing
	// Reads the value on the latest Read Index
	// Returns the value
	uint32_t ReadUInt32LE();

private:
	// call m_Buffer.resize in your constructor
	std::vector<uint8_t> m_Buffer;

	// initialize these at 0
	int m_WriteBufferIndex;
	int m_ReadBufferIndex;
};
