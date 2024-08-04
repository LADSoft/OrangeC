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
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <libp.h>
#include <fcntl.h>
#include <errno.h>
#include <io.h>
long _RTL_FUNC ftell(FILE* stream)
{
    fpos_t rv;
    if (fgetpos(stream, &rv))
        return -1;
    return rv;
}
off_t _RTL_FUNC ftello(FILE* stream)
{
    return (off_t)ftell(stream);
}

long long _RTL_FUNC _ftell64(FILE* restrict stream)
{
    flockfile(stream);
    if (stream->token == FILTOK)
    {
        long long curpos;
        stream->flags &= ~_F_VBUF;
        if (!(__getmode(stream->fd) & O_BINARY) && fflush(stream))
        {   
            funlockfile(stream);
            return EOF;
        }
        curpos = __ll_getpos64(__uiohandle(fileno(stream)));
        if (curpos < 0)
        {
            stream->flags |= _F_ERR;
            errno = EIO;
            funlockfile(stream);
            return EOF;
        }
        if (stream->level > 0 && stream->buffer)
            curpos -= stream->level - 1;
        else if (stream->level < 0 && stream->buffer)
            curpos += stream->bsize + stream->level;
        if (stream->flags & _F_UNGETC)
            curpos--;
        funlockfile(stream);
        return curpos;
    }
    _dos_errno = errno = ENOENT;
    funlockfile(stream);
    return EOF;
}