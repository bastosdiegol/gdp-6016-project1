#include "Buffer.h"

#include <iostream>

// Default Constructor
Buffer::Buffer(size_t size) {
	m_Buffer.resize(size);
	m_WriteBufferIndex	= 0;
	m_ReadBufferIndex	= 0;
}

// Accepts Index and Value
// Writes the value on the specified index
void Buffer::WriteInt32LE(std::size_t index, int32_t value) {
	m_Buffer[index]		= value;
	m_Buffer[++index]	= value >> 8;
	m_Buffer[++index]	= value >> 16;
	m_Buffer[++index]	= value >> 24;
	m_WriteBufferIndex	= ++index;
}

// Accepts Value
// Writes the value on the latest Write Index
void Buffer::WriteInt32LE(int32_t value) {
	m_Buffer[m_WriteBufferIndex] = value;
	m_Buffer[++m_WriteBufferIndex] = value >> 8;
	m_Buffer[++m_WriteBufferIndex] = value >> 16;
	m_Buffer[++m_WriteBufferIndex] = value >> 24;
	m_WriteBufferIndex++;
}

// Accepts Index
// Reads the value on the specified index
// Returns the value
uint32_t Buffer::ReadUInt32LE(std::size_t index) {

	uint32_t value = m_Buffer[index];
	value |= m_Buffer[++index] << 8;
	value |= m_Buffer[++index] << 16;
	value |= m_Buffer[++index] << 24;
	m_ReadBufferIndex = ++index;

	return value;
}

// Accepts nothing
// Reads the value on the latest Read Index
// Returns the value
uint32_t Buffer::ReadUInt32LE() {

	uint32_t value = m_Buffer[m_ReadBufferIndex];
	m_Buffer[++m_ReadBufferIndex] = value >> 8;
	m_Buffer[++m_ReadBufferIndex] = value >> 16;
	m_Buffer[++m_ReadBufferIndex] = value >> 24;
	m_ReadBufferIndex++;

	return value;
}