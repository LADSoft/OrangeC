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
#include <stdio.h>

typedef void CALLBACK convert_func(const char* name, char* buf);

static const char* GetName(const char *name, char *buf)
{
    const char *ptr = name;
    HMODULE hmod = LoadLibrary("lsdbghelper");
    if (hmod)
    {
        convert_func* f = (convert_func*)GetProcAddress(hmod, "ConvertSymbolName");
        if (f)
        {
            buf[0] = 0;
            f(name, buf);
            ptr = buf;
        }
        FreeLibrary(hmod);
    }
    return ptr;
}
// depends on the prototype for ___canary_check
void __buffer_overflow_abort(const char *name, const char *varName)
{
    char buf[8000],buf2[1024];
    fprintf(stderr, "Runtime check: buffer overflow of variable '%s' in function '%s'", GetName(varName, buf2), GetName(name, buf));
    fflush(stderr);
    _abort();
}
// depends on the prototype for ___canary_check
void __uninitialized_variable_abort(const char *name, const char *fileName, int lineno)
{
    char buf[8000];
    fprintf(stderr, "Runtime check: Use of uninitialized variable '%s' at '%s(%d)'", GetName(name, buf), fileName, lineno);
    fflush(stderr);
    _abort();
}
