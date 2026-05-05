#ifndef CHIP8_H
#define CHIP8_H
#include <thread>
#include <chrono>
#include <array>
#include <cstdint>
#include <string>

class Chip8 {
public:
    Chip8();

    void init();
    void loadROM(const std::string& filename);
    void cycle();
    void debug_render();
    // for opcodes
    void handleJump(uint16_t opcode);
    void handleSubRoutine(uint16_t opcode);
    void handleSkipIfEqualByte(uint16_t opcode);
    void handleSkipIfNotEqualByte(uint16_t opcode);
    void handleSkipIfRegisterEqual(uint16_t opcode);
    void handleSetRegister(uint16_t opcode);
    void handleAddToRegister(uint16_t opcode);
    void handle0Group(uint16_t opcode);
    void handleSkipIfRegisterNotEqual(uint16_t opcode);
    void handleSetIndex(uint16_t opcode);
    void DrawToScreen(uint16_t opcode);
    void handleKeyInput(uint16_t opcode);
    void SkipKeyIfVXIsPressed(uint16_t opcode);
    void SkipKeyIfVXIsNotPressed(uint16_t opcode);
    void WaitForKeyPress(uint16_t opcode);
    



    static constexpr std::size_t SCREEN_WIDTH = 64;
    static constexpr std::size_t SCREEN_HEIGHT = 32;
    static constexpr std::size_t SCREEN_SIZE = SCREEN_HEIGHT * SCREEN_WIDTH;
    static constexpr std::size_t NUM_KEYS = 16;
    static constexpr std::size_t MEMORY_SIZE = 4096;
    static constexpr std::size_t NUM_REGISTERS = 16;
    static constexpr std::size_t STACK_SIZE = 16;

    std::array<uint8_t, SCREEN_SIZE> gfx; // fix magic number

    std::array<uint8_t, NUM_KEYS> keypad; // keypad[0x0] = 0, keypad[0xA] = A etc.. 0 not pressed, 1 pressed

private:
    std::array<uint16_t, STACK_SIZE> stack;
    std::array<uint8_t, MEMORY_SIZE> memory; // 4096 bytes | 0x000-0xFFF |
    std::array<uint8_t, NUM_REGISTERS> V; // V[0x0-F], E Flag, F Special, 16 registers
	
    static constexpr std::array<uint8_t, 80> fontset = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
    bool rom_loaded = false;

    uint16_t sp{}; // index of stack, points to top, call/ret management
    uint16_t I{}; // index register, ptr to data in memory
    uint16_t pc{}; // starts at 0x200, ptr to current instruction address

    uint8_t delay_timer{}; // timer loop is 60 hz decrement until 0
    uint8_t sound_timer{};

    void exec_opcode(uint16_t opcode);
};

#endif
