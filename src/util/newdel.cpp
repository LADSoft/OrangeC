#include <stdlib.h>
#include <stdio.h>
void *_RTLENTRY _EXPFUNC operator new(size_t size)
{
    return malloc(size);
}
void _RTLENTRY _EXPFUNC operator delete(void *buf)
{
    return free(buf);
}
void *_RTLENTRY _EXPFUNC operator new [](size_t size)
{
    return malloc(size);
}
void _RTLENTRY _EXPFUNC operator delete [](void *buf)
{
    return free(buf);
}