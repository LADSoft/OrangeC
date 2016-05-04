/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "htmlhelp.h"
#include "header.h"
extern char szInstallPath[];
extern HWND hwndFrame;

char szHelpPath[1024]; // so editing will work...
HMODULE htmlLib;
static FARPROC HelpFunc;

//-------------------------------------------------------------------------

int InitHelp(void)
{
    strcpy(szHelpPath, (char*)ProfileToString("HelpPath", ""));
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
    StringToProfile("HelpPath", szHelpPath);
}
int findhvfiles(char *path, char *sel)
{
    char buf[MAX_PATH];
    sprintf(buf, "%s\\Microsoft Help Viewer\\%s\\Microsoft.VisualStudio.Help.Runtime.dll", path, sel);
    return !access(buf);
}
int CheckWebVersion()
{
    int netVer = 0;
     HKEY hKey;

    if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\NET Framework Setup\\NDP", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        int i=0;
        while (1)
        {
            int n;
            int retCode;
            char buf[256];
            FILETIME lastWriteTime;
            DWORD cbName = sizeof(buf);
            retCode = RegEnumKeyEx(hKey, i++,
                     buf, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &lastWriteTime); 
            if (retCode != ERROR_SUCCESS) 
            {
                break;
            }
            if (buf[0] == 'v')
            {
                n = (buf[1] - '0') * 10;
                if (buf[2] == '.')
                    n += buf[3] - '0';
                else
                    if (n == 40)
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
    if ((GetVersion() & 255) < 5)
    {
        ExtendedMessageBox("Windows 7 required", 0, "Windows 7 or better is required to use this feature.");
        return 0;
    }
    // windows 7
    if ((GetVersion() & 255) == 5)
    {
        char *p =getenv("ProgramFiles(x86)");
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
//http://msdn.microsoft.com/query/dev10.query?appId=Dev10IDEF1&l=EN-US&k=k(%22WINUSER%2fMESSAGEBOX%22);k(MESSAGEBOX)&rd=true
static void  WebHelpThread(void *x)
{
    static DWORD idProcess = NULL;
    char *string = (char *)x;
    DWORD idProcessLast;
    char search[256];
    char cmd[1024];
    STARTUPINFO stStartInfo;
    PROCESS_INFORMATION stProcessInfo;
    strncpy(search, string, sizeof(search));
    search[sizeof(search)-1] = 0;
    strupr(search);
    if ((GetVersion() & 255) < 6)
        sprintf(cmd, "%s\\bin_7\\hv2viewer winbase/%s%%5Cwinuser/%s%%5C%s", szInstallPath, search, search, search);
    else
        sprintf(cmd, "%s\\bin\\hv2viewer winbase/%s%%5Cwinuser/%s%%5C%s", szInstallPath, search, search, search);
//    sprintf(cmd, "explorer \"http://msdn.microsoft.com/query/dev10.query?appId=Dev10IDEF1&l=EN-US&k=k(%%22WINUSER%%2f%s%%22);k(%s)&rd=true\"", search, search);
    memset(&stStartInfo, 0, sizeof(STARTUPINFO));
    memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

    stStartInfo.cb = sizeof(STARTUPINFO);
    CreateProcess(NULL, cmd, NULL, NULL, TRUE, DETACHED_PROCESS, NULL, 
            NULL,  &stStartInfo, &stProcessInfo);
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
int WebHelp(char *string)
{
    if (CheckWebVersion())
        _beginthread((BEGINTHREAD_FUNC)WebHelpThread,0, (LPVOID)string);
}
int ConfigWebHelp(void)
{
    if (CheckWebVersion())
    {
        char cmd[1024];
        STARTUPINFO stStartInfo;
        PROCESS_INFORMATION stProcessInfo;
        sprintf(cmd, "%s\\bin\\helpdownloader", szInstallPath);
        memset(&stStartInfo, 0, sizeof(STARTUPINFO));
        memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));
    
        stStartInfo.cb = sizeof(STARTUPINFO);
        CreateProcess(NULL, cmd, NULL, NULL, TRUE, DETACHED_PROCESS, NULL, 
                NULL,  &stStartInfo, &stProcessInfo);
        CloseHandle(stProcessInfo.hProcess);
        CloseHandle(stProcessInfo.hThread);
    }
}
//-------------------------------------------------------------------------
int SpecifiedHelp(char *string)
{
    HH_AKLINK hl;
    char buf[256];
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
    HelpFunc(GetDesktopWindow(), buf, HH_DISPLAY_TOPIC, 0);
    return HelpFunc(GetDesktopWindow(), buf, HH_KEYWORD_LOOKUP, &hl);
//   	return WinHelp(hwndFrame, szHelpPath, HELP_KEY, (DWORD)string);
}
int RTLHelp(char *string)
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
int LanguageHelp(char *string)
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
        return ;
    HelpFunc(GetDesktopWindow(), buf, HH_DISPLAY_TOPIC, 0);
    HelpFunc(GetDesktopWindow(), buf, HH_HELP_CONTEXT, id);
//    WinHelp(0, buf, HELP_CONTEXT, id);
}
void GenericHelp(char *str, int id)
{
    if (!InitHelp())
        return ;
    if (id == -1)
        HelpFunc(GetDesktopWindow(), str, HH_DISPLAY_TOPIC, 0);
    else
    {
        HelpFunc(GetDesktopWindow(), str, HH_DISPLAY_TOPIC, 0);
        HelpFunc(GetDesktopWindow(), str, HH_HELP_CONTEXT, id);
    }
}