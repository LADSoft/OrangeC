#include <string.h>

size_t _RTL_INTRINS _IMPORT strnlen(const char* ZSTR __s, size_t numberOfElements)
{
    void* next_pos = memchr(__s, '\0', numberOfElements);
    return next_pos != NULL ? (size_t)((intptr_t)next_pos - (intptr_t)__s) : numberOfElements;
}