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

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include "libp.h"
#include <fcntl.h>

int __getmode(int __handle);

int _RTL_FUNC fgetpos(FILE* restrict stream, fpos_t* restrict pos)
{
    flockfile(stream);
    int rv = fgetpos_unlocked(stream, pos);
    funlockfile(stream);
    return rv;
}
int _RTL_FUNC fgetpos_unlocked(FILE* restrict stream, fpos_t* restrict pos)
{
    if (stream->token == FILTOK)
    {
        long curpos;
        stream->flags &= ~_F_VBUF;
        if (!(__getmode(stream->fd) & O_BINARY) && fflush(stream))
            return EOF;
        curpos = tell(fileno(stream));
        if (curpos < 0)
        {
            stream->flags |= _F_ERR;
            errno = EIO;
            return EOF;
        }
        if (stream->level > 0 && stream->buffer)
            curpos -= stream->level - 1;
        else if (stream->level < 0 && stream->buffer)
            curpos += stream->bsize + stream->level;
        if (stream->flags & _F_UNGETC)
            curpos--;
        *pos = curpos;
        return 0;
    }
    _dos_errno = errno = ENOENT;
    return EOF;
}