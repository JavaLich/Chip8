#pragma once

#include <random>
#include <cstdint>

class Chip8 {
private:
  std::mt19937 mt;
  std::uniform_int_distribution<uint32_t> dist;

  uint8_t generate_random_number();

public:
  uint8_t registers[16]{};
  uint8_t memory[4096]{};
  uint16_t i{};
  uint16_t pc{};
  uint16_t stack[16]{};
  uint8_t sp{};
  uint8_t delayTimer{};
  uint8_t soundTimer{};
  bool keypad[16]{};
  uint32_t video[64 * 32]{};
  uint16_t opcode;

  void load_rom(const char *filename);
  void step();

  Chip8();

  void OP_00E0();
  void OP_00EE();
  void OP_1nnn();
  void OP_2nnn();
  void OP_3xkk();
  void OP_4xkk();
  void OP_5xy0();
  void OP_6xkk();
  void OP_7xkk();
  void OP_8xy0();
  void OP_8xy1();
  void OP_8xy2();
  void OP_8xy3();
  void OP_8xy4();
  void OP_8xy5();
  void OP_8xy6();
  void OP_8xy7();
  void OP_8xyE();
  void OP_9xy0();
  void OP_Annn();
  void OP_Bnnn();
  void OP_Cxkk();
  void OP_Dxyn();
  void OP_Ex9E();
  void OP_ExA1();
  void OP_Fx07();
  void OP_Fx0A();
  void OP_Fx15();
  void OP_Fx18();
  void OP_Fx1E();
  void OP_Fx29();
  void OP_Fx33();
  void OP_Fx55();
  void OP_Fx65();
};
