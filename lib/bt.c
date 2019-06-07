#include <bt.h>

int get_pre_sp_ra(void **prev_sp, void **prev_ra, void *sp, void *ra)
{
    unsigned *wra = (unsigned *)ra;
    int       sp_offset;
    while ((*wra >> 16) != 0x27bd)
    {
        // the first instruction of start.S
        if (*wra == 0x40806000)
            return 0;

        wra--;
    }
    sp_offset = ((int)*wra << 16) >> 16;
    *prev_sp  = (char *)sp - sp_offset;
    while (wra < (unsigned *)ra)
    {
        if ((*wra >> 16) == 0xafbf)
        {
            int ra_offset = ((int)*wra << 16) >> 16;
            *prev_ra      = *(void **)((char *)sp + ra_offset);
            return 1;
        }
        wra++;
    }
    return 0;
}

void *get_sp()
{
    void *sp;
    __asm__ __volatile__(
        "move %0,$sp\n\t"
        : "=r"(sp));
    return sp;
}

void *get_ra()
{
    void *ra;
    __asm__ __volatile__(
        "move %0,$ra\n\t"
        : "=r"(ra));
    return ra;
}

int get_call_stack_no_fp(void **retaddrs, int max_size)
{
    void *sp        = get_sp();
    void *ra        = get_ra();
    int * funcbase  = (int *)(int)&get_call_stack_no_fp;
    int   sp_offset = (*funcbase << 16) >> 16;
    int   i         = 0;
    sp              = (char *)sp - sp_offset;
    do
    {
        if (i < max_size)
        {
            retaddrs[i++] = ra;
        }
    } while (get_pre_sp_ra(&sp, &ra, sp, ra) && i < max_size);
    return i;
}

// example
//     void *retaddrs[10];
//     int temp = get_call_stack_no_fp(retaddrs, 10);
//     printf("----------------------------------\n");
//     printf("            BACKTRACE             \n");
//     printf("----------------------------------\n");
//     printf("  backtrace contains %d addresses  \n", temp);
//     for (int i = 0; i < temp; i++)
//     {
//         printf("| %d: [%x]\n", temp - i, (unsigned int)(retaddrs[i]) - 8);
//     }
//     printf("----------------------------------\n");
