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
#include "header.h"
//#include <dir.h>
#include <shlobj.h>
extern char szInstallPath[1024];

static char szProfileName[260];
static char szProfileClass[] = "Defaults";

int mkdir(const char* __path);  // can't include dir. because of the redef of eof
void GetUserDataPath(char* buf)
{
    HANDLE hndl;
    WIN32_FIND_DATA data;
    char ispth[MAX_PATH];
    sprintf(ispth, "%s\\appdata", szInstallPath);
    hndl = FindFirstFile(ispth, &data);
    if (hndl != INVALID_HANDLE_VALUE)
    {
        sprintf(buf, "%s\\appdata\\", szInstallPath);
        FindClose(hndl);
    }
    else if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buf) == S_OK)
    {
        if (buf[strlen(buf) - 1] != '\\')
            strcat(buf, "\\");
        strcat(buf, "Orange C\\");
        mkdir(buf);
    }
    else
    {
        strcpy(buf, szInstallPath);
        strcat(buf, "\\");
    }
}

char* getprofilestring(char* name, char* def, HKEY rootkey)
{
    static char buf[512];
    if (!szProfileName[0])
    {
        GetUserDataPath(szProfileName);
        strcat(szProfileName, "\\ocide.ini");
    }
    GetPrivateProfileString(szProfileClass, name, def, buf, 256, szProfileName);
    return buf;
}

//-------------------------------------------------------------------------

void StringToProfile(char* name, char* string)
{
    if (!szProfileName[0])
    {
        GetUserDataPath(szProfileName);
        strcat(szProfileName, "\\ocide.ini");
    }
    WritePrivateProfileString(szProfileClass, name, string, szProfileName);
}

//-------------------------------------------------------------------------

char* ProfileToString(char* name, char* def)
{
    static char buf[256];
    buf[0] = 0;
    return strcpy(buf, getprofilestring(name, def, HKEY_CURRENT_USER));
}

//-------------------------------------------------------------------------

void IntToProfile(char* key, int value)
{
    char buf[256];
    sprintf(buf, "%d", value);
    StringToProfile(key, buf);
}

//-------------------------------------------------------------------------

int ProfileToInt(char* key, int def)
{
    int val;
    char vvv[256];
    char* buf;
    sprintf(vvv, "%d", def);
    buf = ProfileToString(key, vvv);
    sscanf(buf, "%d", &val);
    return val;
}
