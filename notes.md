Needing to build Chip-8

-Memory
    - memory needs to be 4096 bytes, 4 KB of ram.
    - what is memory going to be represented as.
    - EACH REGISETER CAN ONLY PROCESS 12 BITS
    - INDEX, REG, PC, AND STACK ALL 16 BITS LONG.
    -memory is all writeable and ram.   
    -starts at 512 decimal, or address 200
-Registers
    uint18t -> registers
    - how many registers do we need for the Chip8 Arch?
    - one 16 bit index register that points at location in memory


-Stack
-uint16t stack[16] {}
-needs to call sub routines and functions and retunr fromt hem


-Program Counter
- will need to point to current instruction in resume
-program counter starts at 0x200


-Timers
    uint8t - delay, we have to decrement it 60 Hz (60 times per second) until is 0.
    uint8t - sound (functions like delay timer)

-Input


-general varaibles registed 0-F
ie 0-15 decimal
V0 - VF



-Display
need gfx rows of 32 by 64
gfx size of gfxRow by gfxCol

-Instruction Set
-00E0 CLEAR SCREEN
-1NNN JUMP
-6XNN REGISTER
-7XNN ADD VALUE TO REGISTER
-ANNN SET INDEX REGISTER I
-DXYN DISPLAY DRAW
-0NNN EXECUTE MACHINE LANGUAGE
-00EE SUBROUTINES
-2NNN SUBROUTINES
-3XNN, 4XNN, 5XY0, AND 9XY0: SKIP CONDITIONALLY
-6XNN : SET REGISTER VX TO VALUE NN
-7XNN : ADD VALUE NN TO VX

-8XYO: SET VX is set to value VY

-8XY1: Binary OR: VX is set to OR of VX and VY |

-8XY2: Binary AND: VX is set to & of VX and VY & 

-8XY3: Logical XOR: VX is set to the XOR of VX and VY ^

-8XY4: Add: VX is set to the value of VX + VY

-8XY5: Subtract: 8XY5 sets VX to result of VX - VY
-8XY7: Subtract: 8XY7 sets VX to result of VY - VX

-8XY6: Shift: set VX to the value of VY, shift the value of VX one bit to the right of 8XY6 or left of 8XY3
