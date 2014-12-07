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
#include "header.h"
//#include <dir.h>
#include <shlobj.h>
extern char szInstallPath[1024];
 
    static char szProfileName[260] ;
    static char szProfileClass[] = "Defaults";

int mkdir( const char *__path ); // can't include dir. because of the redef of eof
void GetUserDataPath(char *buf)
{
    HANDLE hndl;
    WIN32_FIND_DATA data;
    char ispth[MAX_PATH];
    sprintf(ispth,"%s\\appdata",szInstallPath);
    hndl = FindFirstFile(ispth, &data);
    if (hndl != INVALID_HANDLE_VALUE)
    {
    sprintf(buf, "%s\\appdata\\", szInstallPath);
        FindClose(hndl);
    }
    else if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buf) == S_OK)
    {
          if (buf[strlen(buf)-1] != '\\')
             strcat(buf,"\\");
        strcat(buf,"Orange C\\");
        mkdir(buf);
    }
    else
    {
        strcpy(buf,szInstallPath);
        strcat(buf,"\\");
        
    }
}

char *getprofilestring(char *name, char *def, HKEY rootkey)
{
    static char buf[512];
    if (!szProfileName[0])
    {
        GetUserDataPath(szProfileName);
        strcat(szProfileName,"\\ocide.ini");
    }
        GetPrivateProfileString(szProfileClass, name, def, buf, 256,
            szProfileName);
    return buf;
}

//-------------------------------------------------------------------------

void StringToProfile(char *name, char *string)
{
    if (!szProfileName[0])
    {
        GetUserDataPath(szProfileName);
        strcat(szProfileName,"\\ocide.ini");
    }
        WritePrivateProfileString(szProfileClass, name, string, szProfileName);
}

//-------------------------------------------------------------------------

char *ProfileToString(char *name, char *def)
{
    static char buf[256];
    buf[0] = 0;
    return strcpy(buf, getprofilestring(name, def, HKEY_CURRENT_USER));
}

//-------------------------------------------------------------------------

void IntToProfile(char *key, int value)
{
    char buf[256];
    sprintf(buf, "%d", value);
    StringToProfile(key, buf);
}

//-------------------------------------------------------------------------

int ProfileToInt(char *key, int def)
{
    int val;
    char vvv[256];
    char *buf;
    sprintf(vvv, "%d", def);
    buf = ProfileToString(key, vvv);
    sscanf(buf, "%d", &val);
    return val;
}
