#include "sdl_window.hpp"
#include "chip8.hpp"

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
        if (entry.is_regular_file() && entry.path().extension() == ".ch8")
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

    // ---------------- FIXED TIMING ----------------
    const double CPU_HZ = 700.0;
    const double CPU_STEP_NS = 1e9 / CPU_HZ;

    const double TIMER_HZ = 60.0;
    const double TIMER_STEP_NS = 1e9 / TIMER_HZ;

    double cpuAcc = 0.0;
    double timerAcc = 0.0;

    auto last = clock::now();

    while (true)
    {
        auto now = clock::now();
        double delta = std::chrono::duration<double, std::nano>(now - last).count();
        last = now;

        cpuAcc += delta;
        timerAcc += delta;

        window.handleInput(chip);

        // ---------------- MENU ----------------
        if (state == AppState::MENU)
        {
            window.render(chip, state, roms, selectedRom);

            if (!selectedRom.empty())
            {
                chip.init();
                chip.loadROM(selectedRom);

                std::cout << "ROM loaded: " << selectedRom << std::endl;

                selectedRom.clear();
                state = AppState::RUNNING;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        // ---------------- CPU (FIXED RATE) ----------------
        while (cpuAcc >= CPU_STEP_NS)
        {
            chip.cycle();
            cpuAcc -= CPU_STEP_NS;
        }

        // ---------------- TIMERS (FIXED 60Hz) ----------------
        while (timerAcc >= TIMER_STEP_NS)
        {
            chip.tickTimers();
            timerAcc -= TIMER_STEP_NS;
        }

        // ---------------- RENDER ----------------
        window.render(chip, state, roms, selectedRom);

        // prevent 100% CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    window.shutdown();
    return 0;
}
