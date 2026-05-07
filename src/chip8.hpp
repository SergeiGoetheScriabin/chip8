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
    void tickTimers();
    void loadROM(const std::string& filename);
    void cycle();

    void debug_render();  // was debugRender
    void exec_opcode(uint16_t opcode);

    void handle0Group(uint16_t opcode);
    void handleJump(uint16_t opcode);
    void handleSubRoutine(uint16_t opcode);

    void handleSkipIfEqualByte(uint16_t opcode);
    void handleSkipIfNotEqualByte(uint16_t opcode);
    void handleSkipIfRegisterEqual(uint16_t opcode);
    void handleSkipIfRegisterNotEqual(uint16_t opcode);

    void handleSetRegister(uint16_t opcode);
    void handleAddToRegister(uint16_t opcode);

    void handleSetIndex(uint16_t opcode);

    void DrawToScreen(uint16_t opcode);

    void handleKeyInput(uint16_t opcode);
    void SkipKeyIfVXIsPressed(uint16_t opcode);
    void SkipKeyIfVXIsNotPressed(uint16_t opcode);
    void WaitForKeyPress(uint16_t opcode);

    void handleFGroup(uint16_t opcode);
    void handle8Group(uint16_t opcode);
    void setVXToDelayTimer(uint16_t opcode);
    void setDelayTimerFromVX(uint16_t opcode);
    void setSoundTimerFromVX(uint16_t opcode);

    void addVXToIndex(uint16_t opcode);
    void setIndexToFontSpriteForVX(uint16_t opcode);
    void storeVXAsBCD(uint16_t opcode);
    void storeV0ToVXToMemory(uint16_t opcode);
    void loadV0ToVXFromMemory(uint16_t opcode);

    static constexpr std::size_t SCREEN_WIDTH = 64;
    static constexpr std::size_t SCREEN_HEIGHT = 32;
    static constexpr std::size_t SCREEN_SIZE = SCREEN_HEIGHT * SCREEN_WIDTH;
    static constexpr std::size_t NUM_KEYS = 16;
    static constexpr std::size_t MEMORY_SIZE = 4096;
    static constexpr std::size_t NUM_REGISTERS = 16;
    static constexpr std::size_t STACK_SIZE = 16;

    bool draw_flag = false;

    std::array<uint8_t, SCREEN_SIZE> gfx;
    std::array<uint8_t, NUM_KEYS> keypad;

private:
    std::array<uint16_t, STACK_SIZE> stack;
    std::array<uint8_t, MEMORY_SIZE> memory;
    std::array<uint8_t, NUM_REGISTERS> V;

    static constexpr std::array<uint8_t, 80> fontset = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    bool rom_loaded = false;

    uint16_t sp{};
    uint16_t I{};
    uint16_t pc{};

    uint8_t delay_timer{};
    uint8_t sound_timer{};
};

#endif
