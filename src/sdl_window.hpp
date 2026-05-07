#ifndef SDL_WINDOW_HPP
#define SDL_WINDOW_HPP

#include <SDL.h>
#include <string>
#include <vector>

class Chip8;

enum class AppState {
    MENU,
    RUNNING
};

class SDLWindow {
public:
    bool init();
    void handleInput(Chip8& chip);

    void render(
        Chip8& chip,
        AppState& state,
        const std::vector<std::string>& roms,
        std::string& selectedRom
    );

    void shutdown();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
};

#endif
