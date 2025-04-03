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
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include "libp.h"

#undef putc
size_t _RTL_FUNC fwrite(void* restrict buf, size_t size, size_t count, FILE* restrict stream)
{
    flockfile(stream);
    size_t rv = fwrite_unlocked(buf, size, count, stream);
    funlockfile(stream);
    return rv;
}
size_t _RTL_FUNC fwrite_unlocked(void* restrict buf, size_t size, size_t count, FILE* restrict stream)
{
    int rv, l = count * size, i = 0;
    char* out = (char*)buf;
    if (l == 0)
    {
        return 0;
    }
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return EOF;
    }
    if (!(stream->flags & _F_WRIT))
    {
        stream->flags |= _F_ERR;
        errno = EFAULT;
        return 0;
    }
    stream->flags &= ~_F_VBUF;
    if ((stream->flags & _F_IN) || stream->buffer && (stream->flags & _F_OUT) && stream->level >= 0)
    {
        if (fflush(stream))
            return 0;
        goto join;
    }
    else
    {
        if (!(stream->flags & _F_OUT))
        {
        join:
            if (stream->flags & _F_BUFFEREDSTRING)
            {
                if (stream->flags & _F_IN)
                    stream->level = -stream->level;
            }
            else
            {
                stream->level = -stream->bsize;
                stream->curp = stream->buffer;
            }
            stream->flags &= ~_F_IN;
            stream->flags |= _F_OUT;
        }
    }
    if (stream->buffer && stream->bsize)
    {
        char* pos = stream->curp;
        while (l)
        {
            int v = l;
            if (v > -stream->level)
                v = -stream->level;
            memcpy(stream->curp, out, v);
            l -= v;
            out += v;
            stream->level += v;
            stream->curp += v;
            if (stream->level >= 0)
            {
                if (fflush(stream))
                    return EOF;
                stream->flags &= ~_F_IN;
                stream->flags |= _F_OUT;
                stream->level = -stream->bsize;
                stream->curp = stream->buffer;
                pos = stream->curp;
            }
            i += v;
        }
        if (stream->flags & _F_LBUF)
        {
            while (pos != stream->curp)
            {
                if (*pos++ == '\n')
                {
                    if (fflush(stream))
                        return EOF;
                    stream->flags &= ~_F_IN;
                    stream->flags |= _F_OUT;
                    stream->level = -stream->bsize;
                    stream->curp = stream->buffer;
                    break;
                }
            }
        }
    }
    else
    {
        if (write(fileno(stream), out, l) < 0)
        {
            stream->flags |= _F_ERR;
            errno = EIO;
            return i / size;
        }
        if (eof(fileno(stream)))
            stream->flags |= _F_EOF;
        i += l;
    }
    if (size == 1)
        return i;
    return i / size;
}