#include "Buffer.h"

#include <iostream>

// Default Constructor
Buffer::Buffer(size_t size) {
	m_BufferData.resize(size);
	m_BufferSize		= size;
	m_WriteBufferIndex	= 0;
	m_ReadBufferIndex	= 0;
}

// Accepts Index and Value
// Writes the LE Int32 value on the specified index
void Buffer::WriteInt32LE(size_t index, int32_t value) {
	// Checks if the Write index is valid to Write an int32_t to the buffer
	if ((index + 4) > m_BufferSize) {
		// Doubles the buffer size
		m_BufferSize *= 2;
		m_BufferData.resize(m_BufferSize);
	}
	m_BufferData[index]		= value;
	m_BufferData[++index]	= value >> 8;
	m_BufferData[++index]	= value >> 16;
	m_BufferData[++index]	= value >> 24;
	m_WriteBufferIndex	= ++index;
}

// Accepts Value
// Writes the LE Int32 value on the latest write Index
void Buffer::WriteInt32LE(int32_t value) {
	// Checks if the Write index is at the end of the buffer
	if (m_BufferSize == m_WriteBufferIndex) {
		// Doubles the buffer size
		m_BufferSize *= 2;
		m_BufferData.resize(m_BufferSize);
	}
	m_BufferData[m_WriteBufferIndex]   = value;
	m_BufferData[++m_WriteBufferIndex] = value >> 8;
	m_BufferData[++m_WriteBufferIndex] = value >> 16;
	m_BufferData[++m_WriteBufferIndex] = value >> 24;
	m_WriteBufferIndex++;
}

// Accepts Value
// Writes the LE Short16 value on the latest write Index
void Buffer::WriteShort16LE(int16_t value) {
	// Checks if the Write index is at the end of the buffer
	if (m_BufferSize == m_WriteBufferIndex) {
		// Doubles the buffer size
		m_BufferSize *= 2;
		m_BufferData.resize(m_BufferSize);
	}
	m_BufferData[m_WriteBufferIndex] = value;
	m_BufferData[++m_WriteBufferIndex] = value >> 8;
	m_WriteBufferIndex++;
}

// Accepts Value
// Writes the LE String value on the latest write Index
void Buffer::WriteStringLE(std::string value) {
	// Checks if the buffer array has space for the string
	if ((m_BufferSize - m_WriteBufferIndex) < value.size()) {
		m_BufferSize += value.size();
		m_BufferData.resize(m_BufferSize);
	}
	for(int i = value.size()-1; i >= 0; i--) {
		m_BufferData[m_WriteBufferIndex]=value.at(i);
		m_WriteBufferIndex++;
	}
}

// Accepts Index
// Reads the LE Int32 value on the latest Read Index
// Returns the Int32
uint32_t Buffer::ReadUInt32LE(size_t index) {

	uint32_t value = m_BufferData[index];
	value |= m_BufferData[++index] << 8;
	value |= m_BufferData[++index] << 16;
	value |= m_BufferData[++index] << 24;
	m_ReadBufferIndex = ++index;

	return value;
}

// Accepts nothing
// Reads the LE Int32 value on the latest Read Index
// Returns the Int32
uint32_t Buffer::ReadUInt32LE() {

	uint32_t value = m_BufferData[m_ReadBufferIndex];
	value |= m_BufferData[++m_ReadBufferIndex] << 8;
	value |= m_BufferData[++m_ReadBufferIndex] << 16;
	value |= m_BufferData[++m_ReadBufferIndex] << 24;
	m_ReadBufferIndex++;

	return value;
}

// Accepts nothing
// Reads the LE Short16 value on the latest Read Index
// Returns the Short16
int16_t Buffer::ReadShort16LE() {

	int16_t value = m_BufferData[m_ReadBufferIndex];
	value |= m_BufferData[++m_ReadBufferIndex] << 8;
	m_ReadBufferIndex++;

	return value;
}

// Accepts String size
// Reads the LE String value on the latest Read Index
// Returns the String
std::string Buffer::ReadStringLE(size_t lenght) {
	std::string value;
	int startingPoint = m_ReadBufferIndex + lenght -1;
	int i = startingPoint;
	int lowerLimit = m_ReadBufferIndex;
	for (int i = startingPoint; i >= lowerLimit; i--) {
		value.push_back(m_BufferData[i]);
	}
	m_ReadBufferIndex += lenght;
	return value;
}
