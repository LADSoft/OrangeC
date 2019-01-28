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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <windows.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

extern char* _oscmd;
extern HINSTANCE* __hInstance;

int _RTL_DATA _argc, _RTL_DATA __argc;
char _RTL_DATA **_argv, _RTL_DATA **__argv;
char* _passed_name;
#pragma startup argset 225

static void argset(void)
{
    char buf[260];
    char *bufp[10000], *ocl;
    char* _cmdline = _oscmd;
    int inquote = 0;
    _argc = 0;
    while (*_cmdline)
    {
        while (isspace(*_cmdline))
            _cmdline++;
        if (*_cmdline)
        {
            int i = 0;
            while ((inquote || !isspace(*_cmdline)) && *_cmdline)
            {
                if (*_cmdline == '"')
                {
                    inquote = !inquote;
                    _cmdline++;
                    continue;
                }
                if (*_cmdline == '\\' && *(_cmdline + 1) == '"')
                    _cmdline++;
                buf[i++] = *_cmdline++;
            }
            buf[i++] = 0;
            bufp[_argc++] = strdup(buf);
        }
    }
    _argv = calloc((_argc + 1), sizeof(char*));
    memcpy(_argv, bufp, _argc * sizeof(char*));
    _passed_name = _argv[0];
    GetModuleFileName(__hInstance, buf, 200);
    _argv[0] = strdup(buf);
    __argv = _argv;
    __argc = _argc;
}
