#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include <stackframe.h>
#include <unistd.h>

// if you fucked up, try this version.
NESTED(handle_sys,TF_SIZE, sp)
    SAVE_ALL                            // Macro used to save trapframe
    CLI                                 // Clean Interrupt Mask
    nop
    .set at                             // Resume use of $at

    // TODO: Fetch EPC from Trapframe, calculate a proper value and store it back to trapframe.

    // get epc
    lw      t0, TF_EPC(sp)

    // epc + 4, because return position should be the next instruction after syscall
    addiu   t0, 4

    sw      t0, TF_EPC(sp)
    
    lw      a0, TF_REG4(sp)

    addiu   a0, a0, -__SYSCALL_BASE     // a0 <- relative syscall number
    sll     t0, a0, 2                   // t0 <- relative syscall number times 4
    la      t1, sys_call_table          // t1 <- syscall table base
    addu    t1, t1, t0                  // t1 <- table entry of specific syscall
    lw      t2, 0(t1)                   // t2 <- function entry of specific syscall

    lw      t0, TF_REG29(sp)            // t0 <- user s stack pointer
    lw      t3, 16(t0)                  // t3 <- the 5th argument of msyscall
    lw      t4, 20(t0)                  // t4 <- the 6th argument of msyscall

    // TODO: Allocate a space of six arguments on current kernel stack and copy the six arguments to proper location

    lw      a0, TF_REG4(sp)
    lw      a1, TF_REG5(sp)
    lw      a2, TF_REG6(sp)
    lw      a3, TF_REG7(sp)

    addiu   sp, sp, -24

    // mips function call rule

    // sw      a0, 0(sp)
    // sw      a1, 4(sp)
    // sw      a2, 8(sp)
    // sw      a3, 12(sp)
    sw      t3, 16(sp)
    sw      t4, 20(sp)

    jalr    t2                          // Invoke sys_* function
    nop
    
    // TODO: Resume current kernel stack
    
    addiu   sp, sp, 24

    sw      v0, TF_REG2(sp)             // Store return value of function sys_* (in $v0) into trapframe

    j       ret_from_exception          // Return from exeception
    nop
END(handle_sys)

sys_call_table:                         // Syscall Table
.align 2
    .word sys_putchar
    .word sys_getenvid
    .word sys_yield
    .word sys_env_destroy
    .word sys_set_pgfault_handler
    .word sys_mem_alloc
    .word sys_mem_map
    .word sys_mem_unmap
    .word sys_env_alloc
    .word sys_set_env_status
    .word sys_set_trapframe
    .word sys_panic
    .word sys_ipc_can_send
    .word sys_ipc_recv
    .word sys_cgetc

/*
NESTED(handle_sys,TF_SIZE, sp)
    SAVE_ALL                            // Macro used to save trapframe
    CLI                                 // Clean Interrupt Mask
    nop
    .set at                             // Resume use of $at

    // TODO: Fetch EPC from Trapframe, calculate a proper value and store it back to trapframe.

    // get epc
    lw      t0, TF_EPC(sp)

    // get v0, which stores syscall number
    lw      v0, TF_REG2(sp)

    // epc + 4, because return position should be the next instruction after syscall
    addiu   t0, 4

    // TODO: Copy the syscall number into $a0.
    // copy the syscall number into a0
    move    a0, v0

    sw      t0, TF_EPC(sp)
    
    addiu   a0, a0, -__SYSCALL_BASE     // a0 <- relative syscall number
    sll     t0, a0, 2                   // t0 <- relative syscall number times 4
    la      t1, sys_call_table          // t1 <- syscall table base
    addu    t1, t1, t0                  // t1 <- table entry of specific syscall
    lw      t2, 0(t1)                   // t2 <- function entry of specific syscall

    lw      t0, TF_REG29(sp)            // t0 <- user s stack pointer
    lw      t3, 16(t0)                  // t3 <- the 5th argument of msyscall
    lw      t4, 20(t0)                  // t4 <- the 6th argument of msyscall

    // TODO: Allocate a space of six arguments on current kernel stack and copy the six arguments to proper location
    
    addiu   sp, sp, -24
    
    // maybe we can get $a0 just by lw a0, TF_REG4(sp) before allocate stack space
    // I will try it some day

    lw      a0, 0(t0)                   // a0 <- the 1st argument of msyscall
    lw      a1, 4(t0)                   // a1 <- the 2nd argument of msyscall
    lw      a2, 8(t0)                   // a2 <- the 3rd argument of msyscall
    lw      a3, 12(t0)                  // a3 <- the 4th argument of msyscall

    // mips function call rule

    sw      a0, 0(sp)
    sw      a1, 4(sp)
    sw      a2, 8(sp)
    sw      a3, 12(sp)
    sw      t3, 16(sp)
    sw      t4, 20(sp)

    jalr    t2                          // Invoke sys_* function
    nop
    
    // TODO: Resume current kernel stack
    
    addiu   sp, sp, 24

    sw      v0, TF_REG2(sp)             // Store return value of function sys_* (in $v0) into trapframe

    j       ret_from_exception          // Return from exeception
    nop
END(handle_sys)

sys_call_table:                         // Syscall Table
.align 2
    .word sys_putchar
    .word sys_getenvid
    .word sys_yield
    .word sys_env_destroy
    .word sys_set_pgfault_handler
    .word sys_mem_alloc
    .word sys_mem_map
    .word sys_mem_unmap
    .word sys_env_alloc
    .word sys_set_env_status
    .word sys_set_trapframe
    .word sys_panic
    .word sys_ipc_can_send
    .word sys_ipc_recv
    .word sys_cgetc
*/