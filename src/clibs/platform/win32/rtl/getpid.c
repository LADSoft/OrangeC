

#include <windows.h>
#define TH32CS_SNAPHEAPLIST (1)
#define TH32CS_SNAPPROCESS (2)
#define TH32CS_SNAPTHREAD (4)
#define TH32CS_SNAPMODULE (8)
#define TH32CS_SNAPALL (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
#define TH32CS_INHERIT (0x80000000U)
typedef struct tagPROCESSENTRY32
{
    DWORD dwSize;
    DWORD cntUsage;
    DWORD th32ProcessID;  // this process
    ULONG_PTR th32DefaultHeapID;
    DWORD th32ModuleID;  // associated exe
    DWORD cntThreads;
    DWORD th32ParentProcessID;  // this process's parent process
    LONG pcPriClassBase;        // Base priority of process's threads
    DWORD dwFlags;
    CHAR szExeFile[MAX_PATH];  // Path
} PROCESSENTRY32;
typedef PROCESSENTRY32* PPROCESSENTRY32;
typedef PROCESSENTRY32* LPPROCESSENTRY32;

HANDLE PASCAL CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);
BOOL PASCAL WINBASEAPI Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);

BOOL PASCAL WINBASEAPI Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);


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
