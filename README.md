## so far we need to implement.


-Memory: CHIP-8 has 4096 bytes (4 KB) of ram.
-Display 64x32
-Program Counter -> points to current instruction in memory
-Index Register -> point to pointers in memory
-Stack for 16 bit addresses for calling subroutines/functions.
-Delay Timer - 60 hz
-Sound Timer - if not 0, beep.
-16 8 bit (one byte) general purpose variable registers 0-F, V0-VF...
-Fontset        TODO: https://github.com/mattmikolay/chip-8/issues/3 implement these fonts by choice of user

