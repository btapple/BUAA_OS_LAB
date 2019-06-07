// implement fork from user space

#include "lib.h"
#include <mmu.h>
#include <env.h>

/* ----------------- help functions ---------------- */

/* Overview:
 * 	Copy `len` bytes from `src` to `dst`.
 *
 * Pre-Condition:
 * 	`src` and `dst` can't be NULL. Also, the `src` area 
 * 	 shouldn't overlap the `dest`, otherwise the behavior of this 
 * 	 function is undefined.
 */
void user_bcopy(const void *src, void *dst, size_t len)
{
    void *max;

    //	writef("~~~~~~~~~~~~~~~~ src:%x dst:%x len:%x\n",(int)src,(int)dst,len);
    max = dst + len;

    // copy machine words while possible
    if (((int)src % 4 == 0) && ((int)dst % 4 == 0))
    {
        while (dst + 3 < max)
        {
            *(int *)dst = *(int *)src;
            dst += 4;
            src += 4;
        }
    }

    // finish remaining 0-3 bytes
    while (dst < max)
    {
        *(char *)dst = *(char *)src;
        dst += 1;
        src += 1;
    }

    //for(;;);
}

/* Overview:
 * 	Sets the first n bytes of the block of memory 
 * pointed by `v` to zero.
 * 
 * Pre-Condition:
 * 	`v` must be valid.
 *
 * Post-Condition:
 * 	the content of the space(from `v` to `v`+ n) 
 * will be set to zero.
 */
void user_bzero(void *v, u_int n)
{
    char *p;
    int m;

    p = v;
    m = n;

    while (--m >= 0)
        *p++ = 0;
}
/*--------------------------------------------------------------*/

/* Overview:
 * 	Custom page fault handler - if faulting page is copy-on-write,
 * map in our own private writable copy.
 * 
 * Pre-Condition:
 * 	`va` is the address which leads to a TLBS exception.
 *
 * Post-Condition:
 *  Launch a user_panic if `va` is not a copy-on-write page.
 * Otherwise, this handler should map a private writable copy of 
 * the faulting page at correct address.
 */
static void
pgfault(u_int va)
{
    u_int tmp;
    u_int perm;

    va = ROUNDDOWN(va, BY2PG);
    perm = (*vpt)[VPN(va)] & (BY2PG - 1);
    if (perm & PTE_COW)
    {
        // for safety
        tmp = UXSTACKTOP - 2 * BY2PG;
        perm = perm & (~PTE_COW);

        //map the new page at a temporary place
        syscall_mem_alloc(0, tmp, perm);

        //copy the content
        user_bcopy((void *)va, (void *)tmp, BY2PG);

        //map the page on the appropriate place
        syscall_mem_map(0, tmp, 0, va, perm);

        //unmap the temporary place
        syscall_mem_unmap(0, tmp);
    }
    else
        user_panic("panic at <pgfault_fork.c>: va is not cow");

    //	writef("fork.c:pgfault():\t va:%x\n",va);
}

/* Overview:
 * 	Map our virtual page `pn` (address pn*BY2PG) into the target `envid`
 * at the same virtual address. 
 *
 * Post-Condition:
 *  if the page is writable or copy-on-write, the new mapping must be 
 * created copy on write and then our mapping must be marked 
 * copy on write as well. In another word, both of the new mapping and
 * our mapping should be copy-on-write if the page is writable or 
 * copy-on-write.
 * 
 * Hint:
 * 	PTE_LIBRARY indicates that the page is shared between processes.
 * A page with PTE_LIBRARY may have PTE_R at the same time. You
 * should process it correctly.
 */
static void inline duppage(u_int envid, u_int pn)
{
    u_int perm = ((Pte *)(*vpt))[pn] & (BY2PG - 1);
    u_int addr = pn << PGSHIFT;

    // read only
    if ((perm & PTE_R) == 0)
        syscall_mem_map(0, addr, envid, addr, perm);
    // writable
    else if (perm & PTE_LIBRARY)
        syscall_mem_map(0, addr, envid, addr, perm);
    else if (perm & PTE_COW)
        syscall_mem_map(0, addr, envid, addr, perm);
    else
    {
        perm = perm | PTE_COW;
        syscall_mem_map(0, addr, envid, addr, perm);
        syscall_mem_map(0, addr, 0, addr, perm);
    }
    // writef("fin:%x\n", addr);
    //	user_panic("duppage not implemented");
}

/* Overview:
 * 	User-level fork. Create a child and then copy our address space
 * and page fault handler setup to the child.
 *
 * Hint: use vpd, vpt, and duppage.
 * Hint: remember to fix "env" in the child process!
 * Note: `set_pgfault_handler`(user/pgfault.c) is different from 
 *       `syscall_set_pgfault_handler`. 
 */
extern void __asm_pgfault_handler(void);
int fork(void)
{
    // Your code here.
    u_int newenvid;
    extern struct Env *envs;
    extern struct Env *env;
    u_int i;
    int r;

    //The parent installs pgfault using set_pgfault_handler
    set_pgfault_handler(pgfault);

    //alloc a new alloc
    if ((newenvid = syscall_env_alloc()) < 0)
        return -E_NO_FREE_ENV;
    else if (newenvid == 0)
    {
        env = &envs[ENVX(syscall_getenvid())];
        return newenvid;
    }
    // vpd : point at user pgdir
    // vpt : point at uesr page table
    // copy user space

    u_int pn;
    int j;
    for (i = 0; i < PTE2PT; i++)
    {
        if (!((*vpd)[i] & PTE_V))
            continue;

        for (j = 0; j < PTE2PT; j++)
        {
            pn = i * PTE2PT + j;
            if (pn == (VPN(UTOP - 2 * BY2PG)))
                break;
            if (((*vpt)[pn] & PTE_V) != 0)
                duppage(newenvid, pn);
        }
    }
    // for (i = 0; i < VPN(UTOP - 2 * BY2PG); i++)
    // {
    //     if (((*vpd)[i >> (PDSHIFT - PGSHIFT)] & PTE_V) != 0 &&
    //         ((*vpt)[i] & PTE_V) != 0)
    //     {
    //         writef("after:%x\n", i);
    //         duppage(newenvid, i);
    //     }
    // }

    if ((r = syscall_mem_alloc(newenvid, UXSTACKTOP - BY2PG, PTE_V | PTE_R)) < 0)
        return r;

    if ((r = syscall_set_pgfault_handler(newenvid, __asm_pgfault_handler, UXSTACKTOP)) < 0)
        return r;

    syscall_set_env_status(newenvid, ENV_RUNNABLE);
    //    writef("new:%x\n", newenvid);
    return newenvid;
}

// Challenge!
int sfork(void)
{
    user_panic("sfork not implemented");
    return -E_INVAL;
}
