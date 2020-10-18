#include "Chip8.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

const uint16_t START_ADDRESS = 0x200;
const uint16_t FONTSET_ADDERSS = 0x50;

const uint8_t NUM_FONTS = 16;
const uint8_t FONT_SIZE = 5;

const uint8_t VIDEO_WIDTH = 64;
const uint8_t VIDEO_HEIGHT = 32;

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
  std::random_device rd;
  mt = std::mt19937(rd());
  dist = std::uniform_int_distribution<uint32_t>(0, 255);

  pc = START_ADDRESS;

  std::copy(&fontset[0][0], &fontset[0][0] + NUM_FONTS * FONT_SIZE, &memory[FONTSET_ADDERSS]);
}

uint8_t Chip8::generate_random_number() {
  return (uint8_t) dist(mt);
}

void Chip8::load_rom(const char *filename) {
  std::ifstream file(filename, std::ios::binary);

  file.seekg(0, std::ios::end);
  std::streampos size = file.tellg();

  file.seekg(0, std::ios::beg);

  char *buffer = new char[size];
  file.read(buffer, size);

  std::copy(buffer, buffer + size, &memory[START_ADDRESS]);

  delete[] buffer;
}

void Chip8::step() {
  pc += 2;
}

void Chip8::OP_00E0() {
  std::memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE() {
  sp--;
  pc = sp;
}

void Chip8::OP_1nnn() {
  pc = opcode & 0x0FFF;
}

void Chip8::OP_2nnn() {
  stack[sp] = pc;
  pc = opcode & 0x0FFF;
  sp++;
}

void Chip8::OP_3xkk() {
  uint8_t kk = opcode & 0x00FF;

  if (registers[(opcode & 0x0F00) >> 8] == kk) {
    pc += 2;
  }
}

void Chip8::OP_4xkk() {
  uint8_t kk = opcode & 0x00FF;

  if (registers[(opcode & 0x0F00) >> 8] != kk) {
    pc += 2;
  }
}

void Chip8::OP_5xy0() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  if (registers[Vx] == registers[Vy]) {
    pc += 2;
  }
}

void Chip8::OP_6xkk() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  registers[Vx] = opcode & 0x00FF;
}

void Chip8::OP_7xkk() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  registers[Vx] += opcode & 0x00FF;
}

void Chip8::OP_8xy0() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;
  registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  registers[Vx] = registers[Vx] | registers[Vy];
}

void Chip8::OP_8xy2() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  registers[Vx] = registers[Vx] & registers[Vy];
}

void Chip8::OP_8xy3() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  registers[Vx] = registers[Vx] ^ registers[Vy];
}

void Chip8::OP_8xy4() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  uint16_t sum = registers[Vx] + registers[Vy];

  if (sum > 0x00FF) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }

  registers[Vx] = sum & 0x00FF;
}

void Chip8::OP_8xy5() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  uint8_t difference = registers[Vx] - registers[Vy];

  if (registers[Vy] < registers[Vx]) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }

  registers[Vx] = difference;
}

void Chip8::OP_8xy6() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;

  if ((Vx & 0x1) == 1) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }

  Vx /= 2;
}

void Chip8::OP_8xy7() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  uint8_t difference = registers[Vy] - registers[Vx];

  if (registers[Vy] > registers[Vx]) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }

  registers[Vx] = difference;
}

void Chip8::OP_8xyE() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;

  if (registers[Vx] >> 7 == 0x1) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }

  registers[Vx] *= 2;
}

void Chip8::OP_9xy0() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  if (registers[Vx] != registers[Vy]) pc += 2;
}

void Chip8::OP_Annn() {
  uint16_t nnn = opcode & 0x0FFF;
  i = nnn;
}

void Chip8::OP_Bnnn() {
  uint16_t nnn = opcode & 0x0FFF;
  pc = registers[0x0] + nnn;
}

void Chip8::OP_Cxkk() {
  uint8_t kk = 0x00FF & opcode;
  uint8_t Vx = (0x0F00 & opcode) >> 8;
  uint8_t rand = generate_random_number() & kk;

  registers[Vx] = rand;
}
