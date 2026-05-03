#include "chip8.hpp"
#include <iostream>

int main() {
    Chip8 chip;

    chip.init();
    chip.loadROM("/Users/frank/chip8/roms/ibm_logo.ch8");

    std::cout << "CHIP-8 starting...\n";

    while (true) {
        chip.cycle();
        chip.debug_render();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    return 0;
}
