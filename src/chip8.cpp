#include "chip8.hpp"
#include <fstream>
#include <iostream>



void Chip8::init() {
    pc = 0x200;
    I = 0;
    sp = 0;

    memory.fill(0);
    V.fill(0);
    stack.fill(0);
    gfx.fill(0);
    keypad.fill(0);

    delay_timer = 0;
    sound_timer = 0;

    // load fontset into memory
    for (size_t i = 0; i < fontset.size(); i++) {
        memory[0x50 + i] = fontset[i];
    }
}

void Chip8::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cout << "Failed to open ROM: " << filename << std::endl;
        return;
    }

    file.read(reinterpret_cast<char*>(&memory[0x200]), MEMORY_SIZE - 0x200);

    if (file.gcount() == 0) {
        std::cout << "ROM is empty\n";
        return;
    }

    pc = 0x200;
    rom_loaded = true;
}

void Chip8::exec_opcode(uint16_t opcode) {

    switch (opcode & 0xF000) {

        case 0x1000: // JMP
            pc = opcode & 0x0FFF;
            return;

        case 0xA000: // LD I
            I = opcode & 0x0FFF;
            break;

        case 0x6000: // LD VX
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;

        case 0x7000: // ADD VX
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;

        case 0xD000: { // DRAW
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            uint8_t n = opcode & 0x000F;

            uint8_t xPos = V[x];
            uint8_t yPos = V[y];

            V[0xF] = 0;

            for (int row = 0; row < n; row++) {
                uint8_t sprite = memory[I + row];

                for (int col = 0; col < 8; col++) {
                    if (sprite & (0x80 >> col)) {

                        int px = (xPos + col) % SCREEN_WIDTH;
                        int py = (yPos + row) % SCREEN_HEIGHT;
                        int idx = px + py * SCREEN_WIDTH;

                        if (gfx[idx])
                            V[0xF] = 1;

                        gfx[idx] ^= 1;
                    }
                }
            }
            break;
        }

        case 0x0000:
            if (opcode == 0x00E0)
                gfx.fill(0);
            break;

        default:
            break;
    }

    // default instruction step
    pc += 2;
}

void Chip8::cycle() {

    if (!rom_loaded) return;
    if (pc + 1 >= MEMORY_SIZE) return;

    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

    exec_opcode(opcode);

    if (delay_timer > 0) --delay_timer;
    if (sound_timer > 0) --sound_timer;
}

void Chip8::debug_render() {
    std::cout << "\033[2J\033[H";

    for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (size_t x = 0; x < SCREEN_WIDTH; x++) {
            std::cout << (gfx[x + y * SCREEN_WIDTH] ? "█" : " ");
        }
        std::cout << "\n";
    }
}

Chip8::Chip8() {
        init();
}
