#include "Chip8.h"

#include <fstream>
#include <algorithm>

const unsigned int START_ADDRESS = 0x200;

void Chip8::LoadROM(const char* filename) {
	std::ifstream file(filename, std::ios::binary);	

	file.seekg(0, std::ios::end);
	std::streampos size = file.tellg();

	file.seekg(0, std::ios::beg);

	char* buffer = new char[size];
	file.read(buffer, size);
	
	std::copy(buffer, buffer+size, &(memory[START_ADDRESS]));

	delete[] buffer;
}
