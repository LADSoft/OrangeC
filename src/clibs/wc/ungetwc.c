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
#include <string.h>
#include <wchar.h>
#undef ungetc
wint_t _RTL_FUNC ungetwc(wint_t c, FILE* stream)
{
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return WEOF;
    }
    if (stream->extended->orient == __or_narrow)
    {
        errno = EINVAL;
        return WEOF;
    }
    stream->extended->orient = __or_wide;
    stream->flags &= ~_F_VBUF;
    if (c == WEOF)
        return WEOF;
    if (stream->buffer)
    {
        if ((stream->flags & _F_IN) && stream->curp != stream->buffer)
        {
            if (stream->level == stream->bsize)
            {
                errno = _dos_errno = ENOSPC;
                return WEOF;
            }
            if (stream->curp == stream->buffer)
            {
                int len;
                memmove(stream->buffer + (len = stream->bsize - stream->level), stream->buffer, stream->level);
                stream->curp += len;
            }
        }
        else
        {
            if (fflush(stream))
                return EOF;
            stream->flags |= _F_IN;
            stream->level = 0;
            stream->curp = stream->buffer + stream->bsize;
        }
        stream->level++;
        *--stream->curp = (char)c;
    }
    else
    {
        if (stream->hold)
        {
            errno = _dos_errno = ENOSPC;
            return WEOF;
        }
        stream->hold = (char)c;
    }
    stream->flags &= ~_F_EOF;
    return c;
}
int _RTL_FUNC _ungetwc(int c, FILE* stream) { return ungetwc(c, stream); }