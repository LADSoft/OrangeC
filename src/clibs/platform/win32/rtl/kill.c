/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <signal.h>
#include "libp.h"

int _RTL_FUNC kill(pid_t pid, int signo)
{
    if (signo == 0)
        return 0;
    if (signo != SIGTERM)
    {
        errno = EINVAL;
        return -1;
    }
    if (pid > 0)
    {
        HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        if (!h)
        {
            errno = ESRCH;
            return -1;
        }
        int rv = !!TerminateProcess(h, 1) - 1;
        CloseHandle(h);
        return rv;
    }
    else
    {
        // not supporting group kills
        errno = ESRCH;
    }
    return -1;
}
