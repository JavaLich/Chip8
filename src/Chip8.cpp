#include "Chip8.h"

#include <algorithm>
#include <bits/stdint-uintn.h>
#include <cstring>
#include <fstream>
#include <iostream>

const uint16_t START_ADDRESS = 0x200; 
const uint16_t FONTSET_ADDRESS = 0x50; 
const uint8_t NUM_FONTS = 16;
const uint8_t FONT_SIZE = 5;

const uint8_t NUM_KEYS = 16;

const uint8_t NUM_REGISTERS = 16;

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
  dist = std::uniform_int_distribution<uint8_t>(0, 255);

  pc = START_ADDRESS;

  std::copy(&fontset[0][0], &fontset[0][0] + NUM_FONTS * FONT_SIZE, &memory[FONTSET_ADDRESS]);
}

uint8_t Chip8::generate_random_number() { return (uint8_t)dist(mt); }

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
    opcode = (memory[pc] << 8) | memory[pc + 1];

    pc += 2; 

    if (delayTimer > 0) 
        delayTimer--;
    if (soundTimer > 0)
        soundTimer--;

    if (opcode == 0x00E0) OP_00E0();
    else if (opcode == 0x00EE) OP_00EE();
    else if (opcode >> 12 == 0x1) OP_1nnn();
    else if (opcode >> 12 == 0x2) OP_2nnn();
    else if (opcode >> 12 == 0x3) OP_3xkk();
    else if (opcode >> 12 == 0x4) OP_4xkk();
    else if (opcode >> 12 == 0x5) OP_5xy0();
    else if (opcode >> 12 == 0x6) OP_6xkk();
    else if (opcode >> 12 == 0x7) OP_7xkk();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x0) OP_8xy0();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x1) OP_8xy1();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x2) OP_8xy2();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x3) OP_8xy3();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x4) OP_8xy4();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x5) OP_8xy5();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x6) OP_8xy6();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0x7) OP_8xy7();
    else if (opcode >> 12 == 0x8 && (opcode & 0x000F) == 0xE) OP_8xyE();
    else if (opcode >> 12 == 0x9) OP_9xy0();
    else if (opcode >> 12 == 0xA) OP_Annn();
    else if (opcode >> 12 == 0xB) OP_Bnnn();
    else if (opcode >> 12 == 0xC) OP_Cxkk();
    else if (opcode >> 12 == 0xD) OP_Dxyn();
    else if (opcode >> 12 == 0xE && (opcode & 0x00FF) == 0x9E) OP_Ex9E();
    else if (opcode >> 12 == 0xE && (opcode & 0x00FF) == 0xA1) OP_ExA1();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x07) OP_Fx07();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x0A) OP_Fx0A();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x15) OP_Fx15();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x18) OP_Fx18();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x1E) OP_Fx1E();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x29) OP_Fx29();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x33) OP_Fx33();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x55) OP_Fx55();
    else if (opcode >> 12 == 0xF && (opcode & 0x00FF) == 0x65) OP_Fx65();
    else {
        std::cout << std::hex << opcode << std::endl;
        std::cout << "Error: unknown opcode" << std::endl;
    } 
}

void Chip8::OP_00E0() { 
    std::memset(video, 0, sizeof(video)); 
}

void Chip8::OP_00EE() {
	pc = stack[--sp];
}

void Chip8::OP_1nnn() { 
    uint16_t address = opcode & 0x0FFF;

	pc = address;
}

void Chip8::OP_2nnn() {
  stack[sp++] = pc;
  pc = opcode & 0x0FFF;
}

void Chip8::OP_3xkk() {
  uint8_t kk = opcode & 0x00FF;
  uint8_t Vx = (opcode & 0x0F00) >> 8;

  if (registers[Vx] == kk) {
    pc += 2;
  }
}

void Chip8::OP_4xkk() {
  uint8_t kk = opcode & 0x00FF;
  uint8_t Vx = (opcode & 0x0F00) >> 8;

  if (registers[Vx] != kk) {
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
  uint8_t kk = opcode & 0x00FF;
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  registers[Vx] += kk;
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

  if ((registers[Vx] & 0x1) == 1) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }

  registers[Vx] /= 2;
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

  if ((registers[Vx] & 0x80) >> 7 == 1) {
    registers[0xF] = 1;
  } else {
    registers[0xF] = 0;
  }

  registers[Vx] *= 2;
}

void Chip8::OP_9xy0() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;

  if (registers[Vx] != registers[Vy])
    pc += 2;
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

void Chip8::OP_Dxyn() {
  uint8_t Vx = (opcode & 0x0F00) >> 8;
  uint8_t Vy = (opcode & 0x00F0) >> 4;
  uint8_t n = opcode & 0x000F;

  uint8_t *sprite = &memory[i];
  bool collided = false;
  for (int r = 0; r < n; r++) {
    uint8_t byte = sprite[r];
    for (int c = 8 - 1; c > -1; c--) {
      uint8_t bit = byte & 0x1;
      int x = registers[Vx] + c;
      int y = registers[Vy] + r;

      if (y >= VIDEO_HEIGHT) {
        y -= VIDEO_HEIGHT;
      }
      if (x >= VIDEO_WIDTH) {
        x -= VIDEO_WIDTH;
      }
      if (x < 0) {
          x += VIDEO_WIDTH;
      }
      if (y < 0) {
          y += VIDEO_HEIGHT;
      }

      uint8_t pixel = video[y * 64 + x] == 0xFFFFFFFF ? 1 : 0;
      if ((bit ^ pixel) == 1)
          video[y * 64 + x] = 0xFFFFFFFF;
      else 
          video[y * 64 + x] = 0;
      if (video[y * 64 + x] && bit == 1)
        collided = true;
      byte >>= 1;
    }
  }

  if (collided) 
      registers[0xF] = 1;
  else
      registers[0xF] = 0;
}

void Chip8::OP_Ex9E() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    if (keypad[Vx]) {
        pc += 2;
    }
}

void Chip8::OP_ExA1() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    if (!keypad[Vx]) {
        pc += 2;
    }
}

void Chip8::OP_Fx07() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    bool keyPressed = false;
    for (uint8_t key = 0; key < NUM_KEYS; key++) {
        if (keypad[key]) {
            keyPressed = true;
            registers[Vx] = key;
            break;
        }
    }

    if (!keyPressed) pc -= 2;
}

void Chip8::OP_Fx15() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    i += registers[Vx];
}

void Chip8::OP_Fx29() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    i = FONTSET_ADDRESS + registers[Vx];
}

void Chip8::OP_Fx33() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t value = registers[Vx];
    memory[i + 2] = value % 10;
    value /= 10;
    memory[i + 1] = value % 10;
    value /= 10;
    memory[i] = value % 10;
}

void Chip8::OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    std::copy(&registers[0], &registers[Vx + 1], &memory[i]);
}

void Chip8::OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    std::copy(&memory[i], &memory[i + Vx + 1], &registers[0]); 
}
