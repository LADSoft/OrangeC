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

#include <errno.h>
#include <windows.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <sys\stat.h>

extern int __uihandles[HANDLE_MAX];

void __ll_uioinit(void)
{
    DWORD mode;
    __uihandles[0] = GetStdHandle(STD_INPUT_HANDLE);
    __uihandles[1] = GetStdHandle(STD_OUTPUT_HANDLE);
    __uihandles[2] = GetStdHandle(STD_ERROR_HANDLE);

    GetConsoleMode((HANDLE)__uihandles[0], &mode);
    SetConsoleMode((HANDLE)__uihandles[0], mode | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
}
