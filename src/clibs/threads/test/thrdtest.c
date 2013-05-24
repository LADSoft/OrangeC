#include "Threads.h"
#include "time.h"
#include "Windows.h"
#include "stdio.h"

int thread1(void *a)
{
    for (int i=0 ; i < 5; i++)
    {    
        printf(a);
        Sleep(1000);
    }
    return 45;
}
int thread2(void *a)
{
    for (int i=0 ; i < 5; i++)
    {    
        printf(a);
        Sleep(1000);
    }
    return 44;
}
int thread6(void *a)
{
    switch ((int)a)
    {
        case 0:
            Sleep(1000);
            break;
        case 1:
            break;
    }
    return 0;
}
int thread4(void *a)
{
    thrd_t thrd;
    thrd_create(&thrd, thread6, 0);
    thrd_detach(thrd);
    thrd_create(&thrd, thread6, (void *)1);
    Sleep(1000);
    thrd_detach(thrd);
    for (int i=0 ; i < 1000; i++)
    {    
        printf(a);
        Sleep(1000);
        if (i == 10)
            thrd_exit(49);
    }
    return 48;
}
int thread5(void *a)
{
    printf("%x\n",thrd_current());
    thrd_t thrd;
    thrd_create(&thrd, thread4, a);
    int res;
    thrd_join(thrd, &res);
    return res;
}
void thread3(void)
{
    printf("of");
}
int thread3a(void *a)
{
    printf("geez");
    return 0;
}
main()
{
    once_flag of = ONCE_FLAG_INIT;
    thrd_t thrd1, thrd, thrd2, thrd3, thrd4;
    xtime xt;
    call_once(&of, thread3);
    call_once(&of, thread3);
    thrd_create(&thrd3, thread3a, 0);
    thrd_create(&thrd4, thread3a, 0);
    printf("%d",thrd_create(&thrd1,thread1,"hi"));
    
    thrd_create(&thrd,thread2,"bye");
    printf(" %d\n", thrd_detach(thrd));
    
    thrd_create(&thrd, thread5, "ohmy");
    printf("%d", thrd_equal(thrd1, thrd));
    
    thrd2 = thrd;
    printf(" %d", thrd_equal(thrd2, thrd));
    printf(" %x\n",thrd);
    for (int i=0; i < 8; i++)
    {
        xtime_get(&xt, TIME_UTC);
        xt.sec ++;
        thrd_sleep(&xt);
        printf("nn");
        thrd_yield();
        printf("mm");
    }
    int res;
    printf("%d ", thrd_join(thrd, &res));
    printf("%d\n", res);
    
    thrd_join(thrd1, &res);
    printf("%d\n", res);
    
    printf("%d ", thrd_join(thrd4, NULL));
    printf("%d\n", thrd_join(thrd4, &res));
    printf("%d\n", thrd_detach(thrd3));
    printf("%d\n", thrd_detach(thrd3));
    printf("%d\n", thrd_join(thrd3, &res));
}
