#include <windows.h>
int _RTL_FUNC getppid(void)
{
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(PROCESSENTRY32);

    int pid = GetCurrentProcessId();
    int retval = -1;

    if (Process32First(h, &pe))
    {
        do
        {
            if (pe.th32ProcessID == pid)
            {
                retval = pe.th32ParentProcessID;
                break;
            }
        } while (Process32Next(h, &pe));
    }

    CloseHandle(h);

    return retval;
}
int _RTL_FUNC getpid() { return GetCurrentProcessId(); }
int _RTL_FUNC _getpid() { return GetCurrentProcessId(); }
