#include <windows.h>
#include <limits.h>

void __ll_cndFree(long long handle) { CloseHandle((HANDLE)handle); }
long long __ll_cndAlloc(void) { return (long long)CreateSemaphore(NULL, 0, INT_MAX, NULL); }
int __ll_cndWait(long long handle, unsigned ms)
{
    switch (WaitForSingleObject((HANDLE)handle, ms == (unsigned)-1 ? INFINITE : ms))
    {
        case WAIT_OBJECT_0:
            return 1;
        case WAIT_TIMEOUT:
            return 0;
        case WAIT_ABANDONED:  // undefined behavior if a thread exits without releasing a mutex...
            return -1;
        default:
            return -1;
    }
}
int __ll_cndSignal(long long handle, int cnt)
{
    if (ReleaseSemaphore((HANDLE)handle, cnt, NULL))
        return 0;
    return -1;
}
