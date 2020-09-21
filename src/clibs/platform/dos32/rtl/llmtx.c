#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <threads.h>
#include <sys\timeb.h>
#include <threads.h>
#include "libp.h"

void __ll_mtxFree(long long handle)
{
}
long long __ll_mtxAlloc(void)
{
    return 0;
}
int __ll_mtxTake(long long handle, unsigned ms)
{
    return -1;
}
int __ll_mtxRelease(long long handle)
{
    return -1;
}
