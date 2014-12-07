#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
void *operator new(size_t size) throw(std::bad_alloc)
{
    return malloc(size);
}
void operator delete(void *buf) throw()
{
    return free(buf);
}
void * operator new [](size_t size)  throw(std::bad_alloc)
{
    return malloc(size);
}
void operator delete [](void *buf) throw()
{
    return free(buf);
}