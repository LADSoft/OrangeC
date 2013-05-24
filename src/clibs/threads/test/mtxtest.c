#include <threads.h>
#include <windows.h>
#include <stdio.h>

mtx_t mtx1;

long long aa;
int thread1(void *aa)
{
    printf("%d\n", mtx_lock(&mtx1));
    printf("%d\n", mtx_lock(&mtx1));    
    printf("%d\n", mtx_unlock(&mtx1));    
    printf("%d\n", mtx_unlock(&mtx1));    
    return 0;
}
int thread2(void *nn1)
{
    int nn =(int)nn1;
    int n = GetTickCount();    
    while (GetTickCount() < n + 3000)
    {
        if (nn & 2)
            mtx_lock(&mtx1);
        if (aa != 0x123456789abcdef0 && aa != 0)
            printf("1");
        if (nn & 1)
        {
            aa = 0x123456789abcdef0;
        }
        else
        {
            aa = 0;
        }
        if (nn & 2)
            mtx_unlock(&mtx1);
        
    }
    return 0;
}
int thread3(void *nn1)
{
    mtx_lock(&mtx1);
    Sleep(1000);
    mtx_unlock(&mtx1);
}
int thread4(void *nn2)
{
    mtx_lock(&mtx1);
    Sleep(1000);
}
int thread5(void *nn2)
{
    mtx_lock(&mtx1);
    printf("thread5\n");
    mtx_unlock(&mtx1);
}
main()
{
    thrd_t thrd1;
    printf("%d\n", mtx_init(&mtx1, mtx_plain));
    thrd_create(&thrd1, thread1, 0);
    thrd_join(thrd1, NULL);
    mtx_destroy(&mtx1);

    printf("\n%d\n", mtx_init(&mtx1, mtx_plain | mtx_recursive));
    thrd_create(&thrd1, thread1, 0);
    thrd_join(thrd1, NULL);
    mtx_destroy(&mtx1);

    mtx_init(&mtx1, mtx_try);
    thrd_create(&thrd1, thread3, 0); 
    Sleep(500);
    printf("%d\n", mtx_trylock(&mtx1));
    xtime xt;
    xtime_get(&xt, TIME_UTC);
    xt.nsec += 200000000;
    printf("%d\n", mtx_timedlock(&mtx1, &xt));
    xt.nsec += 500000000;
    printf("%d\n", mtx_timedlock(&mtx1, &xt));
    mtx_unlock(&mtx1);
    printf("%d\n", mtx_trylock(&mtx1));
    mtx_unlock(&mtx1);
    
    thrd_create(&thrd1, thread4, 0);
    thrd_detach(thrd1);
    thrd_create(&thrd1, thread5, 0);
    thrd_join(thrd1, NULL);
        
    thrd_create(&thrd1, thread2, 0);
    thrd_detach(thrd1);
    thrd_create(&thrd1, thread2, (void *)1);
    thrd_detach(thrd1);
    Sleep(4000);

    thrd_create(&thrd1, thread2, (void *)2);
    thrd_detach(thrd1);
    thrd_create(&thrd1, thread2, (void *)3);
    thrd_detach(thrd1);
    Sleep(4000);
    
    mtx_destroy(&mtx1);

}