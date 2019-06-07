#include <env.h>
#include <pmap.h>
#include <printf.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *  Search through 'envs' for a runnable environment ,
 *  in circular fashion statrting after the previously running env,
 *  and switch to the first such environment found.
 *
 * Hints:
 *  The variable which is for counting should be defined as 'static'.
 */

int countsize(struct Env_list *sched)
{
    struct Env *e;
    int count = 0;
    LIST_FOREACH(e, sched, env_sched_link)
    {
        count += 1;
    }
    return count;
}

void sched_yield(void)
{
    static int count = 0;
    static struct Env_list *sched = env_sched_list;
    static int sub = 1;
    struct Env *e;
    for (;;)
    {
        if (LIST_EMPTY(sched))
        {
            sub = (sub == 0) ? 1 : 0;
            sched = (sub == 1) ? env_sched_list : (env_sched_list + 1);
            //            printf("%d\n",countsize(sched));
            continue;
        }
        e = LIST_FIRST(sched);
        // if (e->env_id == 0x1001)
        //     printf("id: %d status: %d pri: %d pc: %x ret: %x retp: %x\n", e->env_id, e->env_status, e->env_pri, e->env_tf.pc, e->env_tf.regs[2], e->env_tf.regs[31]);
        if (count >= e->env_pri || e->env_status != ENV_RUNNABLE)
        {
            count = 0;
            LIST_REMOVE(e, env_sched_link);
            LIST_INSERT_HEAD(&(env_sched_list[sub]), e, env_sched_link);
            // LIST_INSERT_TAIL(&(env_sched_list[sub]), e, env_sched_link);
        }
        else
        {
            ++count;
            env_run(e);
        }
    }
}
