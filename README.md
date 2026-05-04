# chip8 emulator
---

## what it is

chip8 is a tiny vm.

basic flow:
- load a rom into memory
- execute instructions one at a time
- update cpu state (registers, stack, etc.)
- draw to a 64x32 screen


---

## current state

### memory
- 4kb ram (4096 bytes)

- rom loads into memory starting at 0x200

### cpu
- pc -> current instruction
- I -> index register (memory pointer, mainly for sprites)
- stack → handles calls / returns (16 levels)

### registers
- V0–VF (8-bit registers)
- VF used as a flag (collision, carry, etc.)

### display
- 64x32 framebuffer
- each pixel is 0 or 1
- drawing is basically XOR on pixels

### timers
- delay timer @ 60hz
- sound timer triggers beep when > 0

### fontset
- built-in hex sprites (0-F)
- loaded into memory at startup
- might let user swap/customize fonts later

---

## opcodes implemented so far

- 00E0 -> clear screen

- 00EE -> return from subroutine (pop program counter)

- 1NNN -> jump

- 6XNN -> set register

- 7XNN -> add to register

- DXYN -> draw sprite

- 3XNN -> skip if VX = NN

- 4XNN -> skip if VX != NN

- 5XY0 -> skip if VX == VY

- 9XY0 -> skip if VX != VY

- ANNN -> I = NNN

- 2NNN -> call subroutine (push pc, jump to NNN)

---
## op codes needed to implement


### input (keyboard)
-EX9E -> skip if key VX is pressed

-EXA1 -> skip if key VX is not pressed

### timers / system
-FX07 -> VX = delay timer

-FX15 -> delay timer = VX

-FX18 -> sound timer = VX

### memory / utils


-FX1E -> I += VX

-FX29 -> I = sprite for VX (font lookup)

-FX33 -> store BCD of VX in memory[I]

-FX55 -> store V0-VX in memory[I]


-FX65 -> load V0-VX in memory[I]

## roms

testing with basic roms like:
- ibm logo

just to make sure rendering + opcode flow is correct

---

## build

```bash
make
./chip8 roms/ibm_logo.ch8

