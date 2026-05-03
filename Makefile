 CXX = g++
 CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC = src/chip8.cpp src/main.cpp
OUT = chip8

all:
		$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: all
		./$(OUT)

clean:
		rm -f $(OUT)
