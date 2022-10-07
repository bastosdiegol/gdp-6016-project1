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
// Writes the value on the specified index
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
// Writes the value on the latest Write Index
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

// Accepts Index
// Reads the value on the specified index
// Returns the value
uint32_t Buffer::ReadUInt32LE(size_t index) {

	uint32_t value = m_BufferData[index];
	value |= m_BufferData[++index] << 8;
	value |= m_BufferData[++index] << 16;
	value |= m_BufferData[++index] << 24;
	m_ReadBufferIndex = ++index;

	return value;
}

// Accepts nothing
// Reads the value on the latest Read Index
// Returns the value
uint32_t Buffer::ReadUInt32LE() {

	uint32_t value = m_BufferData[m_ReadBufferIndex];
	m_BufferData[++m_ReadBufferIndex] = value >> 8;
	m_BufferData[++m_ReadBufferIndex] = value >> 16;
	m_BufferData[++m_ReadBufferIndex] = value >> 24;
	m_ReadBufferIndex++;

	return value;
}