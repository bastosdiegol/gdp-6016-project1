#include "Buffer.h";

int main(int argc, char** argv) {
	Buffer b1(16);


	b1.WriteInt32LE(0, 50);		// 00110010
	b1.WriteInt32LE(500);	// 00000001 11110100	

	int readValue50 = b1.ReadUInt32LE(0);
	int readValue500 = b1.ReadUInt32LE(4);

	b1.WriteInt32LE(10);		// 00110010
	b1.WriteInt32LE(13, 100);		// 00000001 11110100	

	int readValue10 = b1.ReadUInt32LE();
	int readValue100 = b1.ReadUInt32LE(13);

	return 0;
}