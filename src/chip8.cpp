#include "chip8.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

void Chip8::init()
{
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

    // load fontset into memory, starts at 0x50.
    for (size_t i = 0; i < fontset.size(); i++)
    {
        memory[0x50 + i] = fontset[i];
    }

    rom_loaded = false;
    draw_flag = false;
}

void Chip8::loadROM(const std::string &filename)
{
    // [ 0x000 - 0x1FF ] -> FONTS/INTERPRETER BYTES
    // [ 0x200 - 0xFFF ] -> ROM BYTES
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        std::cout << "Failed to open ROM: " << filename << std::endl;
        return;
    }

    // copy bytes from the ROM file directly into CHIP-8 RAM starting at 0x200, only if we are in bounds.
    file.read(reinterpret_cast<char *>(&memory[0x200]), MEMORY_SIZE - 0x200);

    if (file.gcount() == 0)
    {
        std::cout << "ROM is empty\n";
        return;
    }

    pc = 0x200; //  program counter starts at 0x200.
    rom_loaded = true;
}

void Chip8::exec_opcode(uint16_t opcode)
{
    switch (opcode & 0xF000)
    {

    // 00E0 / 00EE
    case 0x0000:
        handle0Group(opcode);
        break;

    // 1NNN - JP addr: unconditional jump to address NNN
    case 0x1000:
        handleJump(opcode);
        return; // skip pc += 2, jump sets pc directly

    // 2NNN - call subroutine
    case 0x2000:
        handleSubRoutine(opcode);
        return; // skip pc += 2, subroutine sets pc directly

    // 3XNN - Skip if Vx equals a number.
    case 0x3000:
        handleSkipIfEqualByte(opcode);
        break;

    // 4XNN - skip next instruction if Vx does NOT equal a number
    case 0x4000:
        handleSkipIfNotEqualByte(opcode);
        break;

    // 5XY0 - skip next instruction if Vx equals Vy
    case 0x5000:
        handleSkipIfRegisterEqual(opcode);
        break;

    // 6XNN - set Vx to a number
    case 0x6000:
        handleSetRegister(opcode);
        break;

    // 7XNN - add a number to Vx
    case 0x7000:
        handleAddToRegister(opcode);
        break;

    case 0x8000:
        handle8Group(opcode);
        break;

    // 9XY0 - skip next instruction if Vx does NOT equal Vy
    case 0x9000:
        handleSkipIfRegisterNotEqual(opcode);
        break;

    // ANNN - set I to an address
    case 0xA000:
        handleSetIndex(opcode);
        break;

    // BNNN - jump with offset
    case 0xB000:
        pc = (opcode & 0x0FFF) + V[0];
        return; // skip pc += 2

    // CXNN - random
    case 0xC000:
        V[(opcode & 0x0F00) >> 8] = (std::rand() % 256) & (opcode & 0xFF);
        break;

    // DXYN - draw sprite
    case 0xD000:
        DrawToScreen(opcode);
        break;

    // EX9E / EXA1 - keyboard input
    case 0xE000:
        handleKeyInput(opcode);
        break;

    case 0xF000:
        handleFGroup(opcode);
        break;

    default:
        break;
    }
    pc += 2;
}

// 00E0 / 00EE
inline void Chip8::handle0Group(uint16_t opcode)
{
    if (opcode == 0x00E0)
    {
        gfx.fill(0); // clear screen by setting all pixels to 0.
        draw_flag = true;
    }
    else if (opcode == 0x00EE)
    {
        // 00EE - RET: return from subroutine
        sp--;
        pc = stack[sp];
        // don't increment pc, return address already points to next instruction
    }
}

inline void Chip8::handle8Group(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0x000F)
    {
        case 0x0: V[x]  = V[y]; break;
        case 0x1: V[x] |= V[y]; break;
        case 0x2: V[x] &= V[y]; break;
        case 0x3: V[x] ^= V[y]; break;
        case 0x4: { uint16_t r = V[x] + V[y]; V[0xF] = r > 0xFF; V[x] = r & 0xFF; break; }
        case 0x5: { uint8_t vf = V[x] > V[y]; V[x] -= V[y]; V[0xF] = vf; break; }
        case 0x6: { uint8_t vf = V[x] & 0x1; V[x] >>= 1; V[0xF] = vf; break; }
        case 0x7: { uint8_t vf = V[y] > V[x]; V[x] = V[y] - V[x]; V[0xF] = vf; break; }
        case 0xE: { uint8_t vf = (V[x] >> 7) & 1; V[x] <<= 1; V[0xF] = vf; break; }
    }
}

inline void Chip8::handleFGroup(uint16_t opcode)
{
    switch (opcode & 0x00FF)
    {
    case 0x07:
        setVXToDelayTimer(opcode);
        break;
    case 0x0A:
        WaitForKeyPress(opcode);
        break;
    case 0x15:
        setDelayTimerFromVX(opcode);
        break;
    case 0x18:
        setSoundTimerFromVX(opcode);
        break;
    case 0x1E:
        addVXToIndex(opcode);
        break;
    case 0x29:
        setIndexToFontSpriteForVX(opcode);
        break;
    case 0x33:
        storeVXAsBCD(opcode);
        break;
    case 0x55:
        storeV0ToVXToMemory(opcode);
        break;
    case 0x65:
        loadV0ToVXFromMemory(opcode);
        break;
    default:
        break;
    }
}

inline void Chip8::handleJump(uint16_t opcode)
{
    pc = opcode & 0x0FFF; // 12 bit address.
}

inline void Chip8::handleSubRoutine(uint16_t opcode)
{
    stack[sp] = pc + 2;
    sp++;
    pc = opcode & 0x0FFF; // 12 bit address.
}

inline void Chip8::handleSetRegister(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t n = opcode & 0x00FF;

    V[x] = n;
}

inline void Chip8::handleAddToRegister(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t n = opcode & 0x00FF;

    V[x] += n;
}

inline void Chip8::handleSkipIfRegisterEqual(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if (V[x] == V[y])
        pc += 2;
}

inline void Chip8::handleSkipIfRegisterNotEqual(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if (V[x] != V[y])
        pc += 2;
}

inline void Chip8::handleSetIndex(uint16_t opcode)
{
    I = opcode & 0x0FFF; // store 12 bit address [ 0x000–0xFFF ]  in memory register.
}

inline void Chip8::handleSkipIfEqualByte(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    if (V[x] == nn)
    {
        pc += 2;
    }
}

inline void Chip8::handleSkipIfNotEqualByte(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    if (V[x] != nn)
    {
        pc += 2;
    }
}

inline void Chip8::handleKeyInput(uint16_t opcode)
{
    switch (opcode & 0x00FF)
    {
    case 0x9E:
        SkipKeyIfVXIsPressed(opcode);
        break;
    case 0xA1:
        SkipKeyIfVXIsNotPressed(opcode);
        break;
    }
}

inline void Chip8::SkipKeyIfVXIsPressed(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    if (keypad[V[x]])
    {
        pc += 2;
    }
}

inline void Chip8::SkipKeyIfVXIsNotPressed(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    if (!keypad[V[x]])
    {
        pc += 2;
    }
}

inline void Chip8::WaitForKeyPress(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    for (int i = 0; i < 16; i++)
    {
        if (keypad[i]) { V[x] = i; return; }
    }
    pc -= 2; // no key pressed, re-run this instruction next cycle
}

inline void Chip8::DrawToScreen(uint16_t opcode)
{
    // 0xD000
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);

    uint8_t xPos = V[x];
    uint8_t yPos = V[y];

    V[0xF] = 0;

    for (int row = 0; row < n; row++)
    {
        uint8_t sprite = memory[I + row];

        for (int col = 0; col < 8; col++)
        {
            if (sprite & (0x80 >> col))
            {
                int px = (xPos + col) % SCREEN_WIDTH;
                int py = (yPos + row) % SCREEN_HEIGHT;
                int idx = px + py * SCREEN_WIDTH;

                if (gfx[idx])
                    V[0xF] = 1; // visited something on screen.

                gfx[idx] ^= 1; // flip pixel on/off
            }
        }
    }

    draw_flag = true;
}

// fx07 delay timer
inline void Chip8::setVXToDelayTimer(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    V[x] = delay_timer;
}

// fx15 delay timer
inline void Chip8::setDelayTimerFromVX(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    delay_timer = V[x];
}

// fx18 sound timer
inline void Chip8::setSoundTimerFromVX(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    sound_timer = V[x];
}

// fx1e - i += vx
inline void Chip8::addVXToIndex(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    I += V[x];
}

// fx29 - font sprite address
inline void Chip8::setIndexToFontSpriteForVX(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    I = 0x50 + (V[x] * 5);
}

// fx33 - bcd store
inline void Chip8::storeVXAsBCD(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t value = V[x];

    memory[I]     = value / 100;
    memory[I + 1] = (value / 10) % 10;
    memory[I + 2] = value % 10;
}

// fx55 - store V0-VX in memory
inline void Chip8::storeV0ToVXToMemory(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++)
    {
        memory[I + i] = V[i];
    }
}

// fx65 - load v0-vx from memory
inline void Chip8::loadV0ToVXFromMemory(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++)
    {
        V[i] = memory[I + i];
    }
}

void Chip8::cycle()
{
    if (!rom_loaded)
        return;
    if (pc + 1 >= MEMORY_SIZE)
        return;

    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

    exec_opcode(opcode);
}

void Chip8::debug_render()
{
    std::cout << "\033[2J\033[H";

    for (size_t y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (size_t x = 0; x < SCREEN_WIDTH; x++)
        {
            std::cout << (gfx[x + y * SCREEN_WIDTH] ? "█" : " ");
        }
        std::cout << "\n";
    }
}

void Chip8::tickTimers()
{
    if (delay_timer > 0) --delay_timer;
    if (sound_timer > 0) --sound_timer;
}

Chip8::Chip8()
{
    std::srand(std::time(nullptr));
    init();
}
