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