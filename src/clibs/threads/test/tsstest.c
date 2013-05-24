#include <threads.h>
#include <windows.h>
#include <Stdio.h>

tss_t tss1, tss2;

void rundown(void *a)
{
    printf("$%d\n", a);
}
void rundown1(void *a)
{
    printf("$%d\n", a);
    tss_delete(tss2);
    printf("@%d\n", tss_set(tss2, (void *)5));
}
int thread1(void *aa)
{
    printf("%d\n", tss_create(&tss1, rundown));
    printf("%d\n", tss_set(tss1, (void *)4));
    printf(":%d\n", tss_get(tss1));
    tss_create(&tss2, rundown1);
    tss_set(tss2, (void *)5);
    printf(":%d\n", tss_get(tss2));
}
main()
{
    thrd_t thrd1;
    thrd_create(&thrd1, thread1, 0);
    thrd_join(thrd1, NULL);
}