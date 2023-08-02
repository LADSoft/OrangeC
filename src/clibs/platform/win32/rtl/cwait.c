#include <errno.h>
#include <windows.h>
void __WaitForInputIdle(HANDLE hprocess);
int cwait(int* status, int pid, int action)
{
    DWORD rv;
    __WaitForInputIdle((HANDLE)pid);
    if (WaitForSingleObject((HANDLE)pid, INFINITE))
    {
        errno = ECHILD;
        return -1;
    }
    if (!GetExitCodeProcess((HANDLE)pid, &rv))
    {
        errno = EINTR;
        return -1;
    }
    if (status)
        *status = rv;
    return pid;
}
int _cwait(int* status, int pid, int action) { return cwait(status, pid, action); }
