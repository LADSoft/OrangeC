#include <threads.h>
#include <sys/timeb.h>
_Thread_local int aa = 7;

void addtime(timespec *aa, int ms)
{
    aa->nsec += ms * 1000 * 1000;
    if (aa->nsec >= 1000 * 1000 * 1000)
    {
        aa->nsec -= 1000 * 1000 * 1000;
        aa->sec++;
    }
}
int vv(void *t)
{
    mtx_t mutex = *(mtx_t *)t;
    for (int i=0; i < 10; i++)
    {	
        mtx_lock(&mutex);
	printf("%d", aa++);
        mtx_unlock(&mutex);
        timespec aa ;
        timespec_get(&aa, TIME_UTC);
	addtime(&aa,100);
        thrd_sleep(&aa, 0);
        
    }
}
int main()
{
        mtx_t mutex;
	thrd_t thrd1, thrd2;
        mtx_init(&mutex, mtx_plain);
        thrd_create(&thrd1, vv, &mutex);
        timespec aa;
        timespec_get(&aa, TIME_UTC);
        addtime(&aa,500);

        thrd_sleep(&aa, 0);
        thrd_create(&thrd2, vv, &mutex);
	int res = 0;
        thrd_join(thrd2, &res);
	return res;
        mtx_destroy(&mutex);
}