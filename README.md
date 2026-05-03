## so far we need to implement

- **Memory**  
  CHIP-8 has 4096 bytes (4 KB) of RAM

- **Display**  
  64 x 32 monochrome screen

- **Program Counter (pc)**  
  points to the current instruction in memory

- **Index Register (I)**  
  points to locations in memory (mainly for sprites / data)

- **Stack**  
  stores 16-bit return addresses for subroutines (call/return)

- **Delay Timer**  
  counts down at 60Hz

- **Sound Timer**  
  same as delay timer, but when > 0 it triggers a beep

- **Registers (V0 - VF)**  
  16 general purpose 8-bit registers  
  VF is usually used as a flag (collision, carry, etc.)

- **Fontset**  
  built-in sprite data for hex characters (0-F)  
  TODO: https://github.com/mattmikolay/chip-8/issues/3  
  (might let user swap/customize fonts later)
