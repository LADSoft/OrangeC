#include <string.h>
#include <sys/types.h>

size_t _RTL_FUNC strlcpy(char* dst, const char* src, size_t n)
{
    size_t lsrc = strlen(src), lcpy;
    if (n > 0)
    {
        lcpy = (lsrc >= n) ? n - 1 : lsrc;
        memcpy(dst, src, lcpy);
        dst[lcpy] = '\0';
    }
    return lsrc;
}
