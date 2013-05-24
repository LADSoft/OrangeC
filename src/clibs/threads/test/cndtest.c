#include "Threads.h"
#include <windows.h>
#include <stdio.h>

cnd_t cnd1;
mtx_t mtx1;
mtx_t mtx2;

int thread1(void *aa)
{
    Sleep(1000);
    cnd_signal(&cnd1);
    Sleep(1000);
    cnd_signal(&cnd1);
    Sleep(1000);
    cnd_signal(&cnd1);
}
int thread2(void *bb)
{
    mtx_lock(&mtx2);
    printf("-%d\n",cnd_wait(&cnd1, &mtx2));
    printf("bye\n");
    mtx_unlock(&mtx2);
}
int thread3(void *bb)
{
    Sleep(100);
    mtx_lock(&mtx2);
    printf("try");
    mtx_unlock(&mtx2);
}
int thread4(void *bb)
{
    mtx_lock(&mtx1);
    printf("$%d\n",cnd_wait(&cnd1, &mtx1));
    printf("omhy\n");
    mtx_unlock(&mtx1);
}
main()
{
    thrd_t thrd1;
    mtx_init(&mtx1, mtx_plain);
    mtx_init(&mtx2, mtx_plain);
    mtx_lock(&mtx1);
    printf("&%d\n", cnd_init(&cnd1));
    thrd_create(&thrd1, thread1, 0);
    thrd_detach(thrd1);
    thrd_create(&thrd1, thread2, 0);
    thrd_detach(thrd1);
    thrd_create(&thrd1, thread3, 0);
    thrd_detach(thrd1);
    Sleep(200);
    printf("#%d\n",cnd_wait(&cnd1, &mtx1));
    printf("hi\n");

    thrd_create(&thrd1, thread1, 0);
    thrd_detach(thrd1);
    thrd_create(&thrd1, thread2, 0);
    thrd_detach(thrd1);
    thrd_create(&thrd1, thread3, 0);
    thrd_detach(thrd1);
    Sleep(200);
    xtime xt;
    xtime_get(&xt, TIME_UTC);
    xt.nsec += 200000000;
    printf("+%d\n",cnd_timedwait(&cnd1, &mtx1, &xt));
    xt.sec += 4;
    printf(":%d\n",cnd_timedwait(&cnd1, &mtx1, &xt));
    mtx_unlock(&mtx1);
    
    thrd_create(&thrd1, thread2, 0);
    thrd_detach(thrd1);
    thrd_create(&thrd1, thread4, 0);
    thrd_detach(thrd1);
    Sleep(1000);
    cnd_broadcast(&cnd1);
    Sleep(1000);
    
    mtx_destroy(&mtx1);
    mtx_destroy(&mtx2);
    cnd_destroy(&cnd1);
}