#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

// if you fucked up, try this version.
LEAF(msyscall)
    syscall
    jr ra
    nop
END(msyscall)

/*
LEAF(msyscall)
    // TODO: execute a `syscall` instruction and return from msyscall

    // sw a0, 0(sp)
    // sw a1, 4(sp)
    // sw a2, 8(sp)
    // sw a3, 12(sp)
    // the 5th and 6th arguments are saved by compiler
    // sw a4, 16(sp)        // No a4 or a5.
    // sw a5, 20(sp)

    // the syscall no should be saved in $v0
    // Actually, the ABI does not define the implementation of individual system calls.
    // move v0, a0
    syscall

    jr ra
    nop
END(msyscall)

*/
