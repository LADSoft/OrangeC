#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <stdio.h>
#include <errno.h>
#include <dir.h>

int __ll_system(char* string, int in, int out)
{
    FILE* f;
    char buf[4096], *a;
    if (*string)
    {
        while (isspace(*string))
            string++;
        if (!strnicmp(string, "cd ", 3))
        {
            return chdir(string + 3);
        }
    }
    a = getenv("COMSPEC");
    if (!a)
        a = searchpath("cmd.exe");
    if (!string)
    {
        if (!a)
            return 0;
        if (f = fopen(a, "r"))
        {
            fclose(f);
            return 1;
        }
        return 0;
    }
    if (!a)
    {
        errno = ENOENT;
        return -1;
    }
    sprintf(buf, "%s /C %s", a, string);
    STARTUPINFO stStartInfo;
    PROCESS_INFORMATION stProcessInfo;
    DWORD exitCode;
    BOOL bRet;

    memset(&stStartInfo, 0, sizeof(STARTUPINFO));
    memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

    stStartInfo.cb = sizeof(STARTUPINFO);
    stStartInfo.dwFlags = STARTF_USESTDHANDLES;
    stStartInfo.hStdInput = _get_osfhandle(in);
    stStartInfo.hStdOutput = _get_osfhandle(out);
    stStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    bRet = CreateProcess(NULL, buf, NULL, NULL, TRUE, 0, NULL, NULL, &stStartInfo, &stProcessInfo);

    if (in != fileno(stdin))
        close(in);
    if (out != fileno(stdout))
        close(out);
    if (!bRet)
    {
        return -1;
    }
    WaitForSingleObject(stProcessInfo.hProcess, INFINITE);
    DWORD rv;
    GetExitCodeProcess(stProcessInfo.hProcess, &rv);
    CloseHandle(stProcessInfo.hProcess);
    CloseHandle(stProcessInfo.hThread);
    return rv;
}
