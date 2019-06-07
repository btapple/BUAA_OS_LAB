#ifndef _BT_H_
#define _BT_H_
int get_pre_sp_ra(void **prev_sp, void **prev_ra, void *sp, void *ra);
void *get_sp();
void *get_ra();
int get_call_stack_no_fp(void **retaddrs, int max_size);
#endif

