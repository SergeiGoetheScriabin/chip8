#include "sdl_window.hpp"
#include "chip8.hpp"

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include <iostream>

bool SDLWindow::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return false;

    window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        0
    );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 32
    );

    // IMGUI INIT
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    return true;
}

void SDLWindow::handleInput(Chip8& chip)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT)
            exit(0);

        bool pressed = (event.type == SDL_KEYDOWN);

        switch (event.key.keysym.sym)
        {
            case SDLK_x: chip.keypad[0x0] = pressed; break;
            case SDLK_1: chip.keypad[0x1] = pressed; break;
            case SDLK_2: chip.keypad[0x2] = pressed; break;
            case SDLK_3: chip.keypad[0x3] = pressed; break;
            case SDLK_q: chip.keypad[0x4] = pressed; break;
            case SDLK_w: chip.keypad[0x5] = pressed; break;
            case SDLK_e: chip.keypad[0x6] = pressed; break;
            case SDLK_a: chip.keypad[0x7] = pressed; break;
            case SDLK_s: chip.keypad[0x8] = pressed; break;
            case SDLK_d: chip.keypad[0x9] = pressed; break;
            case SDLK_z: chip.keypad[0xA] = pressed; break;
            case SDLK_c: chip.keypad[0xB] = pressed; break;
            case SDLK_4: chip.keypad[0xC] = pressed; break;
            case SDLK_r: chip.keypad[0xD] = pressed; break;
            case SDLK_f: chip.keypad[0xE] = pressed; break;
            case SDLK_v: chip.keypad[0xF] = pressed; break;
        }
    }
}

void SDLWindow::render(
    Chip8& chip,
    AppState& state,
    const std::vector<std::string>& roms,
    std::string& selectedRom
)
{
    // -----------------------------
    // IMGUI FRAME START
    // -----------------------------
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // -----------------------------
    // MENU
    // -----------------------------
    if (state == AppState::MENU)
    {
        ImGui::Begin("CHIP-8 Launcher");

        for (auto& rom : roms)
        {
            if (ImGui::Button(rom.c_str()))
            {
                selectedRom = rom;
            }
        }

        ImGui::Text("Select a ROM");

        ImGui::End();
    }
    // -----------------------------

    // -----------------------------
    // DRAW CHIP-8 SCREEN
    // -----------------------------
// -----------------------------
    // DRAW CHIP-8 SCREEN
    // -----------------------------
    if (chip.draw_flag)
    {
        uint32_t pixels[64 * 32];
        for (int i = 0; i < 64 * 32; i++)
            pixels[i] = chip.gfx[i] ? 0xFFFFFFFF : 0x000000FF;
        SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));
        chip.draw_flag = false;
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    // -----------------------------
    // IMGUI RENDER
    // -----------------------------
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(
        ImGui::GetDrawData(),
        renderer
    );

    SDL_RenderPresent(renderer);
}

void SDLWindow::shutdown()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}
