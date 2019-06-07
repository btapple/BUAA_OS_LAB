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
            continue;
        }
        e = LIST_FIRST(sched);
        if (count < e->env_pri)
        {
            ++count;
            env_run(e);
        }
        else
        {
            count = 0;
            LIST_REMOVE(e, env_sched_link);
            LIST_INSERT_HEAD(&(env_sched_list[sub]), e, env_sched_link);
            // LIST_INSERT_TAIL(&(env_sched_list[sub]), e, env_sched_link);
        }
    }
}
