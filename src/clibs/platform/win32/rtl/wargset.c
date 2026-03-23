/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <windows.h>
#include <stdlib.h>
#include <ctype.h>
#include <wchar.h>

extern HINSTANCE* __hInstance;

static char* empty_array[1] = { NULL };

int _RTL_DATA _wargc, _RTL_DATA __wargc;
WCHAR _RTL_DATA **_wargv = &empty_array[0], _RTL_DATA **__wargv = &empty_array[0];
WCHAR* _wpassed_name;

extern int __argc, _argc;

void __wmain_argset(void)
{
    WCHAR* _cmdline = GetCommandLineW();
    WCHAR *buf = calloc(sizeof(WCHAR), wcslen(_cmdline) + 1);
    WCHAR *bufp[10000], *ocl;
    int inquote = 0;
    _wargc = 0;
    while (*_cmdline)
    {
        while (iswspace(*_cmdline))
            _cmdline++;
        if (*_cmdline)
        {
            int i = 0;
            while ((inquote || !iswspace(*_cmdline)) && *_cmdline)
            {
                if (*_cmdline == '"')
                {
                    inquote = !inquote;
                    _cmdline++;
                    continue;
                }
                if (*_cmdline == '\\' && (*(_cmdline + 1) == '"' || *(_cmdline + 1) == '\\'))
                    _cmdline++;
                buf[i++] = *_cmdline++;
            }
            buf[i++] = 0;
            bufp[_wargc++] = wcsdup(buf);
        }
    }
    _wargv = calloc((_wargc + 1), sizeof(WCHAR*));
    memcpy(_wargv, bufp, _wargc * sizeof(WCHAR*));
    _wpassed_name = _wargv[0];
    WCHAR modname[260];
    GetModuleFileNameW(__hInstance, modname, 200);
    _wargv[0] = wcsdup(modname);
    __wargv = _wargv;
    __wargc = _argc = __argc = _wargc;
    free(buf);
}