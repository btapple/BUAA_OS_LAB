#include "lib.h"

void umain()
{
    // fork();
    // fork();
    // fork();
    // fork();
    // fork();
    int a = 0;
    int id = 0;
    /*
    id = fork();
    if(id==0)
    */
    if ((id = fork()) == 0)
    {
        if ((id = fork()) == 0)
        {
            a += 3;
            for (;;)
                writef("\t\tthis is child2 :a:%d\n", a);
        }

        a += 2;
        for (;;)
            writef("\tthis is child :a:%d\n", a);
    }
    /*
    writef("father get id:%x\n",id);
    for(;;);
    */
    a++;
    for (;;)
        writef("this is father: a:%d\n", a);
}
