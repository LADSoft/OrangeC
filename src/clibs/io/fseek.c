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
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
#include "libp.h"

int _RTL_FUNC fseeko(FILE* stream, off_t offset, int origin)
{
    return fseek(stream, (long)offset, origin);
}
int _RTL_FUNC fseek(FILE* stream, long offset, int origin)
{
    flockfile(stream);
    int rv = fseek_unlocked(stream, offset, origin);
    funlockfile(stream);
    return rv;
}
static int _wfseek_unlocked(FILE* stream, long long offset, int origin)
{
    if (stream->token != FILTOK)
    {
        errno = ENOENT;
        return EOF;
    }
    stream->flags &= ~_F_VBUF;
    if (stream->flags & _F_BUFFEREDSTRING)
    {
        int end;
        for (end = 0; end < stream->bsize; end++)
            if (stream->buffer[end] == 0)
                break;
        int n;
        switch (origin)
        {
            case SEEK_CUR:
                n = stream->curp - stream->buffer;
                break;
            case SEEK_SET:
                n = 0;
                break;
            case SEEK_END:
                if (stream->flags & _F_BIN)
                {
                    n = stream->bsize;
                }
                else
                {
                    n = end;
                }
                break;
            default:
                return EOF;
        }
        int n1 = n + offset;
        if (n1 < 0)
            return EOF;
        if (n1 >= stream->bsize)
        {
            if (!(stream->extended->flags2 & _F2_DYNAMICBUFFER))
            {
                return EOF;
            }
            unsigned newSize = stream->bsize;
            while (newSize < n1)
                newSize *= 2;
            void* p = realloc(stream->buffer, newSize);
            if (p)
            {
                stream->curp = p + (stream->curp - stream->buffer);
                stream->buffer = p;
                stream->bsize = newSize;
            }
        }
        for (int i = end; i < n1; i++)
            stream->buffer[i] = 0;
        stream->level = stream->flags & _F_OUT ? -stream->bsize + n1 : stream->bsize - n1;
        if (stream->flags & _F_OUT)
            stream->level -= stream->bsize;
        stream->curp = stream->buffer + n1;
        stream->flags &= ~_F_UNGETC;
        stream->hold = 0;
        memset(stream->extended->mbstate, 0, sizeof(stream->extended->mbstate));
        stream->flags &= ~(_F_EOF | _F_XEOF);
        if (origin == SEEK_END && offset >= 0 && !(stream->extended->flags2 & _F2_DYNAMICBUFFER))
            stream->flags |= _F_EOF | _F_XEOF;
        return 0;
    }
    switch (origin)
    {
        case SEEK_CUR:
        case SEEK_SET:
        case SEEK_END:
            stream->flags &= ~_F_UNGETC;
            stream->hold = 0;
            if (fflush(stream))
            {
                return EOF;
            }
            if (lseek(fileno(stream), offset, origin) < 0)
            {
                stream->flags |= _F_ERR;
                errno = EIO;
                return EOF;
            }
            memset(stream->extended->mbstate, 0, sizeof(stream->extended->mbstate));
            stream->level = 0;
            stream->flags &= ~(_F_EOF | _F_XEOF | _F_IN | _F_OUT);
            if (origin == SEEK_END && offset >= 0)
                stream->flags |= _F_EOF | _F_XEOF;
            __uio_clearerr(fileno(stream));
            break;
        default:
            return EOF;
    }
    return 0;
}
int _RTL_FUNC fseek_unlocked(FILE* stream, long offset, int origin)
{
    return _wfseek_unlocked(stream, offset, origin);
}
int _RTL_FUNC _fseek64(FILE* stream, long long offset, int origin)
{
    flockfile(stream);
    int rv = _wfseek_unlocked(stream, offset, origin);
    funlockfile(stream);
    return rv;
}