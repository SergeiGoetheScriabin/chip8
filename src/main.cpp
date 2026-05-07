#include "chip8.hpp"
#include "sdl_window.hpp"
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;

std::vector<std::string> loadRoms(const std::string& folder)
{
    std::vector<std::string> roms;
    for (const auto& entry : fs::directory_iterator(folder))
    {
        if (entry.is_regular_file() &&
            entry.path().extension() == ".ch8")
        {
            roms.push_back(entry.path().string());
        }
    }
    return roms;
}

int main()
{
    Chip8 chip;
    SDLWindow window;
    if (!window.init())
        return -1;

    AppState state = AppState::MENU;
    std::vector<std::string> roms = loadRoms("roms");
    std::string selectedRom;

    using clock = std::chrono::steady_clock;
    auto lastTimerTick = clock::now();

    while (true)
    {
        window.handleInput(chip);

        if (state == AppState::MENU)
        {
            window.render(chip, state, roms, selectedRom);

            if (!selectedRom.empty())
            {
                chip.init();               // ✅ clear dirty state
                chip.loadROM(selectedRom);
                std::cout << "ROM loaded: " << selectedRom << std::endl;
                selectedRom = "";          // ✅ clear so it doesn't reload
                state = AppState::RUNNING;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        for (int i = 0; i < 25; i++)
            chip.cycle();

        auto now = clock::now();
        if (now - lastTimerTick >= std::chrono::milliseconds(16))
        {
            chip.tickTimers();
            lastTimerTick = now;
        }

        window.render(chip, state, roms, selectedRom);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    window.shutdown();
}
