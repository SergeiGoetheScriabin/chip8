CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SDL_FLAGS = $(shell sdl2-config --cflags --libs)

IMGUI_DIR = imgui

IMGUI_SRC = \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_demo.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp

SRC = \
	src/chip8.cpp \
	src/sdl_window.cpp \
	src/main.cpp \
	$(IMGUI_SRC)

INCLUDES = \
	-I$(IMGUI_DIR) \
	-I$(IMGUI_DIR)/backends

OUT = chip8

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDES) -o $(OUT) $(SDL_FLAGS)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
