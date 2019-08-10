/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "htmlhelp.h"
#include "header.h"
#define _MT
#include <errno.h>
extern char szInstallPath[];
extern HWND hwndFrame;

static HMODULE htmlLib;

static FARPROC HelpFunc;

//-------------------------------------------------------------------------

int InitHelp(void)
{
    if (htmlLib)
        return TRUE;
    htmlLib = LoadLibrary("hhctrl.ocx");
    if (htmlLib)
    {
        HelpFunc = GetProcAddress(htmlLib, ATOM_HTMLHELP_API_ANSI);
        HelpFunc(GetDesktopWindow(), 0, HH_INITIALIZE, 0);
        return TRUE;
    }

    return FALSE;
    ;
}

//-------------------------------------------------------------------------

void RundownHelp(void)
{
    if (htmlLib)
    {
        HelpFunc(GetDesktopWindow(), 0, HH_UNINITIALIZE, 0);
        FreeLibrary(htmlLib);
        htmlLib = 0;
    }
}
int findhvfiles(char* path, char* sel)
{
    char buf[MAX_PATH];
    sprintf(buf, "%s\\Microsoft Help Viewer\\%s\\Microsoft.VisualStudio.Help.Runtime.dll", path, sel);
    return !access(buf, 0);
}
int CheckWebVersion()
{
    int winVer = ((GetVersion() & 0xff) << 8) + ((GetVersion() >> 8) & 0xff);
    int netVer = 0;
    HKEY hKey;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\NET Framework Setup\\NDP", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        int i = 0;
        while (1)
        {
            int n = 0;
            int retCode;
            char buf[256];
            FILETIME lastWriteTime;
            DWORD cbName = sizeof(buf);
            retCode = RegEnumKeyEx(hKey, i++, buf, &cbName, NULL, NULL, NULL, &lastWriteTime);
            if (retCode != ERROR_SUCCESS)
            {
                break;
            }
            if (buf[0] == 'v')
            {
                n = (buf[1] - '0') * 10;
                if (buf[2] == '.')
                    n += buf[3] - '0';
                else if (n == 40)
                    n = 45;
                if (n > netVer)
                    netVer = n;
            }
        }
        RegCloseKey(hKey);
    }
    if (netVer < 45)
    {
        ExtendedMessageBox(".NET 4.5 required", 0, ".NET Framework 4.5 or better is required to use this feature.");
        return 0;
    }
    // windows 8 or above
    if (winVer < 0x601)
    {
        ExtendedMessageBox("Windows 7 required", 0, "Windows 7 or better is required to use this feature.");
        return 0;
    }
    // windows 7
    if (winVer == 0x601)
    {
        char* p = getenv("ProgramFiles(x86)");
        if (!p || !*p)
            p = getenv("ProgramFiles");
        if (!p || !*p)
            return 0;
        if (!findhvfiles(p, "v2.0"))
            if (!findhvfiles(p, "v2.1"))
                if (!findhvfiles(p, "v2.2"))
                {
                    ExtendedMessageBox("VS required", 0, "Install VS 12 or higher to use this feature.");
                    return 0;
                }
    }
    return 1;
}
// http://msdn.microsoft.com/query/dev10.query?appId=Dev10IDEF1&l=EN-US&k=k(%22WINUSER%2fMESSAGEBOX%22);k(MESSAGEBOX)&rd=true
static void MSDNHelpThread(void* x)
{
    static DWORD idProcess = NULL;
    char* string = (char*)x;
    DWORD idProcessLast;
    char search[256];
    char cmd[1024];
    STARTUPINFO stStartInfo;
    PROCESS_INFORMATION stProcessInfo;
    strncpy(search, string, sizeof(search));
    search[sizeof(search) - 1] = 0;
    strupr(search);
    if ((GetVersion() & 0xffff) == 0x0106)  // Windows 7
        sprintf(cmd, "\"%s\\bin_7\\hv2viewer\" winbase/%s%%5Cwinuser/%s%%5C%s", szInstallPath, search, search, search);
    else
        sprintf(cmd, "\"%s\\bin_8\\hv2viewer\" winbase/%s%%5Cwinuser/%s%%5C%s", szInstallPath, search, search, search);
    //    sprintf(cmd, "explorer
    //    \"http://msdn.microsoft.com/query/dev10.query?appId=Dev10IDEF1&l=EN-US&k=k(%%22WINUSER%%2f%s%%22);k(%s)&rd=true\"",
    //    search, search);
    memset(&stStartInfo, 0, sizeof(STARTUPINFO));
    memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

    stStartInfo.cb = sizeof(STARTUPINFO);
    CreateProcess(NULL, cmd, NULL, NULL, TRUE, DETACHED_PROCESS, NULL, NULL, &stStartInfo, &stProcessInfo);
    idProcessLast = idProcess;
    idProcess = stProcessInfo.dwProcessId;
    // if this is the first time we get stuck waiting, and idProcess has a handle
    // to the instance
    // if the instance is already running the new process exits immediately
    // and the existing handle can be used to bring it to top
    WaitForSingleObject(stProcessInfo.hProcess, INFINITE);
    CloseHandle(stProcessInfo.hProcess);
    CloseHandle(stProcessInfo.hThread);
    idProcess = idProcessLast;
    if (idProcess)
        ProcessToTop(idProcess);
}
// http://msdn.microsoft.com/query/dev10.query?appId=Dev10IDEF1&l=EN-US&k=k(%22WINUSER%2fMESSAGEBOX%22);k(MESSAGEBOX)&rd=true
static void MSDNHelp2(void* x)
{
    char* string = (char*)x;
    char search[256];
    char cmd[1024];
    STARTUPINFO stStartInfo;
    PROCESS_INFORMATION stProcessInfo;
    strncpy(search, string, sizeof(search));
    search[sizeof(search) - 1] = 0;
    strupr(search);
    sprintf(cmd,
            "explorer "
            "\"http://msdn.microsoft.com/query/"
            "dev10.query?appId=Dev10IDEF1&l=EN-US&k=k(%%22WINBASE%%2f%s%%22);k(%%22WINUSER%%2f%s%%22);k(%s)&rd=true\"",
            search, search, search);
    memset(&stStartInfo, 0, sizeof(STARTUPINFO));
    memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

    stStartInfo.cb = sizeof(STARTUPINFO);
    CreateProcess(NULL, cmd, NULL, NULL, TRUE, DETACHED_PROCESS, NULL, NULL, &stStartInfo, &stProcessInfo);
    CloseHandle(stProcessInfo.hProcess);
    CloseHandle(stProcessInfo.hThread);
}
int SpecifiedHelp(char* string)
{
    HH_AKLINK hl;
    char szHelpPath[1024];  // so editing will work...
    PropGetString(NULL, "CHM_HELP_PATH", szHelpPath, sizeof(szHelpPath));
    if (!szHelpPath[0] || !InitHelp())
        return FALSE;
    if (!string)
        string = "";
    hl.cbStruct = sizeof(HH_AKLINK);
    hl.fReserved = FALSE;
    hl.pszKeywords = string;
    hl.pszUrl = NULL;
    hl.pszMsgText = NULL;
    hl.pszMsgTitle = NULL;
    hl.pszWindow = NULL;
    hl.fIndexOnFail = TRUE;
    HelpFunc(GetDesktopWindow(), szHelpPath, HH_DISPLAY_TOPIC, 0);
    return HelpFunc(GetDesktopWindow(), szHelpPath, HH_KEYWORD_LOOKUP, &hl);
    //   	return WinHelp(hwndFrame, szHelpPath, HELP_KEY, (DWORD)string);
}
int MSDNHelp(char* string)
{
    switch (PropGetBool(NULL, "MSDN_HELP_MODE"))
    {
        case 0:
            MSDNHelp2(string);
            break;
        case 1:
            if (CheckWebVersion())
                _beginthread((BEGINTHREAD_FUNC)MSDNHelpThread, 0, (LPVOID)string);
            break;
        case 2:
            SpecifiedHelp(string);
            break;
    }
    return 1;
}
int ConfigMSDNHelp(void)
{
    if (CheckWebVersion())
    {
        char cmd[1024];
        STARTUPINFO stStartInfo;
        PROCESS_INFORMATION stProcessInfo;
        sprintf(cmd, "\"%s\\bin_8\\helpdownloader\"", szInstallPath);
        memset(&stStartInfo, 0, sizeof(STARTUPINFO));
        memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

        stStartInfo.cb = sizeof(STARTUPINFO);
        CreateProcess(NULL, cmd, NULL, NULL, TRUE, DETACHED_PROCESS, NULL, NULL, &stStartInfo, &stProcessInfo);
        CloseHandle(stProcessInfo.hProcess);
        CloseHandle(stProcessInfo.hThread);
    }
}
int RTLHelp(char* string)
{
    HH_AKLINK hl;
    char buf[256];
    strcpy(buf, szInstallPath);
    strcat(buf, "\\help\\crtl.chm");
    if (!InitHelp())
        return FALSE;
    if (!string)
        string = "";
    hl.cbStruct = sizeof(HH_AKLINK);
    hl.fReserved = FALSE;
    hl.pszKeywords = string;
    hl.pszUrl = NULL;
    hl.pszMsgText = NULL;
    hl.pszMsgTitle = NULL;
    hl.pszWindow = NULL;
    hl.fIndexOnFail = TRUE;
    HelpFunc(GetDesktopWindow(), buf, HH_DISPLAY_TOPIC, 0);
    HelpFunc(GetDesktopWindow(), buf, HH_KEYWORD_LOOKUP, &hl);
    //    WinHelp(hwndFrame, buf, HELP_KEY, (DWORD)string);

    return TRUE;
}
int LanguageHelp(char* string)
{
    HH_AKLINK hl;
    char buf[256];
    strcpy(buf, szInstallPath);
    strcat(buf, "\\help\\chelp.chm");
    if (!InitHelp())
        return FALSE;
    if (!string)
        string = "";
    hl.cbStruct = sizeof(HH_AKLINK);
    hl.fReserved = FALSE;
    hl.pszKeywords = string;
    hl.pszUrl = NULL;
    hl.pszMsgText = NULL;
    hl.pszMsgTitle = NULL;
    hl.pszWindow = NULL;
    hl.fIndexOnFail = TRUE;
    HelpFunc(GetDesktopWindow(), buf, HH_DISPLAY_TOPIC, 0);
    HelpFunc(GetDesktopWindow(), buf, HH_KEYWORD_LOOKUP, &hl);
    //    WinHelp(hwndFrame, buf, HELP_KEY, (DWORD)string);

    return TRUE;
}
//-------------------------------------------------------------------------

void ContextHelp(int id)
{
    char buf[256];
    strcpy(buf, szInstallPath);
    strcat(buf, "\\help\\ocide.chm");
    if (!InitHelp())
        return;
    HelpFunc(GetDesktopWindow(), buf, HH_DISPLAY_TOPIC, 0);
    HelpFunc(GetDesktopWindow(), buf, HH_HELP_CONTEXT, id);
    //    WinHelp(0, buf, HELP_CONTEXT, id);
}
void GenericHelp(char* str, int id)
{
    if (!InitHelp())
        return;
    if (id == -1)
        HelpFunc(GetDesktopWindow(), str, HH_DISPLAY_TOPIC, 0);
    else
    {
        HelpFunc(GetDesktopWindow(), str, HH_DISPLAY_TOPIC, 0);
        HelpFunc(GetDesktopWindow(), str, HH_HELP_CONTEXT, id);
    }
}