#include "Chip8.h"

#include <algorithm>
#include <fstream>

const uint16_t START_ADDRESS = 0x200;
const uint16_t FONTSET_ADDERSS = 0x50;

const uint8_t NUM_FONTS = 16;
const uint8_t FONT_SIZE = 5;

uint8_t fontset[NUM_FONTS][FONT_SIZE] = {
    {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
    {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
    {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
    {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
    {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
    {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
    {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
    {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
    {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
    {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
    {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
    {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
    {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
    {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
    {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
    {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
};

Chip8::Chip8() {
  pc = START_ADDRESS;

  std::copy(&fontset[0][0], &fontset[0][0] + NUM_FONTS * FONT_SIZE,
            &memory[FONTSET_ADDERSS]);
}

void Chip8::LoadROM(const char *filename) {
  std::ifstream file(filename, std::ios::binary);

  file.seekg(0, std::ios::end);
  std::streampos size = file.tellg();

  file.seekg(0, std::ios::beg);

  char *buffer = new char[size];
  file.read(buffer, size);

  std::copy(buffer, buffer + size, &memory[START_ADDRESS]);

  delete[] buffer;
}
