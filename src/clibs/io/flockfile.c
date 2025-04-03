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

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys\stat.h>
#include <stdatomic.h>
#include <threads.h>

#include "libp.h"

void flockfile(FILE* stream)
{
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return;
    }
    int expected = 0;
    while (atomic_compare_exchange_strong(&stream->extended->lock, &expected, 1))
    {
        thrd_yield();
        expected = 0;
    }
}
int ftrylockfile(FILE* stream)
{
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return 0;
    }
    int expected = 0;
    return !atomic_compare_exchange_strong(&stream->extended->lock, &expected, 1);
}
void funlockfile(FILE* stream)
{
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return;
    }
    atomic_store(&stream->extended->lock, 0);
}
