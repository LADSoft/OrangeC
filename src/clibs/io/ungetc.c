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
int _RTL_FUNC(ungetc_unlocked)(int c, FILE* stream);

int _RTL_FUNC(ungetc)(int c, FILE* stream)
{
    flockfile(stream);
    int rv = (ungetc_unlocked)(c, stream);
    funlockfile(stream);
    return rv;
}
int _RTL_FUNC(ungetc_unlocked)(int c, FILE* stream)
{
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return EOF;
    }
    if (stream->extended->orient == __or_wide)
    {
        errno = EINVAL;
        return EOF;
    }
    stream->extended->orient = __or_narrow;
    stream->flags &= ~_F_VBUF;
    if (c == EOF)
        return EOF;
    if (stream->flags & _F_UNGETC)
    {
        if (!stream->buffer || stream->level == stream->bsize)
        {
            errno = _dos_errno = ENOSPC;
            return EOF;
        }
        stream->level++;
        *--stream->curp = stream->hold;
    }
    stream->hold = (char)c;
    stream->flags &= ~_F_EOF;
    stream->flags |= _F_UNGETC;
    return c;
}
int _RTL_FUNC _ungetc(int c, FILE* stream) { return ungetc(c, stream); }
