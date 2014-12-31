#include <errno.h>
#include <windows.h>
int cwait(int *status, int pid, int action)
{
  DWORD rv;
  WaitForInputIdle((HANDLE)pid, INFINITE);
  if (WaitForSingleObject((HANDLE)pid, INFINITE)) {
    errno = ECHILD;
    return -1;
  }
  if (!GetExitCodeProcess((HANDLE)pid, &rv)) {
    errno = EINTR;
    return -1;
  }
  if (status)
    *status = rv;
  return pid;
}
int _cwait(int *status, int pid, int action)
{
  return cwait(status, pid, action);
}
