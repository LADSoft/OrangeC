#include <string.h>
#include <ctype.h>
#include <windows.h>

// this is to get around a buggy command.com on freedos...
// it is implemented this way to prevent the IDE from popping up a console window
// when it is used...
int winsystem(char *cmd)
{
    STARTUPINFO stStartInfo;
    PROCESS_INFORMATION stProcessInfo;
    DWORD exitCode;
    BOOL bRet;

    memset(&stStartInfo, 0, sizeof(STARTUPINFO));
    memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

    stStartInfo.cb = sizeof(STARTUPINFO);
    stStartInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    stStartInfo.wShowWindow = SW_HIDE;
    stStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE) ;
    stStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE) ;
    stStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE) ;
    bRet = CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, 
            NULL,  &stStartInfo, &stProcessInfo);

    if (!bRet)
    {
        return -1;
    }
    WaitForSingleObject(stProcessInfo.hProcess, INFINITE);
    CloseHandle(stProcessInfo.hProcess);
    CloseHandle(stProcessInfo.hThread);
    return 0;
}
