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
    CloseHandle((HANDLE)handle);    
}
long long __ll_mtxAlloc(void)
{
    return (long long)CreateMutex(NULL, 0, NULL);
}
int __ll_mtxTake(long long handle, unsigned ms)
{
    switch (WaitForSingleObject((HANDLE)handle, ms == (unsigned)-1 ? INFINITE : ms))
    {
        case WAIT_OBJECT_0:
            return 1;
        case WAIT_TIMEOUT:
            return 0;
        case WAIT_ABANDONED:  // undefined behavior if a thread exits without releasing a mutex...
            __ll_enter_critical();
            ReleaseMutex((HANDLE)handle);
            WaitForSingleObject((HANDLE)handle, 0);
            __ll_exit_critical();
            return 1;
        default:
            return -1;
    }
}
int __ll_mtxRelease(long long handle)
{
    int rv ;
    __ll_enter_critical();
    rv = ReleaseMutex((HANDLE)handle);
    __ll_exit_critical();
    if (rv)
        return 0;
    return -1;
}
