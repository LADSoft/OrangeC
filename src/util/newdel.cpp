#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
void *operator new(size_t size)
{
    return malloc(size);
}
void operator delete(void *buf)
{
    return free(buf);
}
void * operator new [](size_t size)
{
    return malloc(size);
}
void operator delete [](void *buf)
{
    return free(buf);
}