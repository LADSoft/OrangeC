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
#include <string.h>
#include <io.h>
#include "libp.h"
#include <fcntl.h>

int __getmode(int __handle);

int __readbuf(FILE* stream)
{
    int sz, sz2;
    int oldmode;
    if (stream->flags & _F_XEOF)
    {
        stream->flags |= _F_EOF;
        return EOF;
    }
    if (stream->buffer)
    {
        /* if a buffer, attempt to read the whole thing... */
        stream->curp = stream->buffer;
        sz2 = stream->bsize;
    }
    else
    {
        /* if no buffer, we blit one byte into stream->hold */
        stream->curp = &stream->hold;
        sz2 = 1;
    }
    oldmode = __getmode(fileno(stream));
    setmode(fileno(stream), O_BINARY);
    stream->level = sz = read(fileno(stream), stream->curp, sz2);
    setmode(fileno(stream), oldmode);
    /* get out on error */
    if (sz < 0)
    {
        stream->flags |= _F_ERR;
        errno = EIO;
        return EOF;
    }
    /* get out on EOF */
    else if (sz == 0)
    {
        stream->flags |= _F_EOF;
        return EOF;
    }
    else
    {
        if (!(__getmode(fileno(stream)) & O_BINARY))
        {
            if (stream->buffer)
            {
                /* look for CTRL-Z */
                unsigned char* p = stream->buffer;
                int i;
                p = memchr(stream->buffer, 0x1a, stream->level);
                if (p)
                {
                    stream->level = p - stream->buffer;
                    stream->flags |= _F_XEOF;
                }
            }
            else if (stream->hold == 0x1a)
            {
                stream->flags |= _F_EOF;
                return EOF;
            }
        }
    }
    return 0;
}
int _RTL_FUNC fgetc(FILE* stream)
{
    flockfile(stream);
    int rv = fgetc_unlocked(stream);
    funlockfile(stream);
    return rv;
}
int _RTL_FUNC fgetc_unlocked(FILE* stream)
{
    int rv;
    int binary;
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
    if (stream->flags & _F_EOF)
        if (isatty(fileno(stream)))
            stream->flags &= ~(_F_XEOF | _F_EOF);
        else
            return EOF;
    stream->flags &= ~_F_VBUF;
    if (!(stream->flags & _F_READ))
    {
        stream->flags |= _F_ERR;
        errno = EFAULT;
        return EOF;
    }
    if (stream == stdin)
    {
        fflush(stdout);
    }
    if (!(stream->flags & _F_IN))
    {
        if (stream->flags & _F_OUT)
        {
            if (fflush(stream))
                return EOF;
        }
        if (stream->flags & _F_BUFFEREDSTRING)
        {
            if (stream->flags & _F_OUT)
                stream->level = -stream->level;
        }
        else
        {
            stream->level = 0;
        }
        stream->flags &= ~_F_OUT;
        stream->flags |= _F_IN;
    }
    if (stream->flags & _F_UNGETC)
    {
        rv = stream->hold;
        stream->hold = 0;
        stream->flags &= ~_F_UNGETC;
        return rv;
    }
    binary = (__getmode(fileno(stream)) & O_BINARY) || (stream->flags & _F_BUFFEREDSTRING);
    do
    {
        if (--stream->level <= 0)
        {
            if (stream->flags & _F_BUFFEREDSTRING)
            {
                stream->flags |= _F_EOF;
                return EOF;
            }
            rv = __readbuf(stream);
            if (rv)
                return rv;
        }
        rv = *stream->curp++;
    } while (rv == '\r' && !binary);
    return rv;
}
int _RTL_FUNC _fgetc(FILE* stream) { return fgetc(stream); }
int _RTL_FUNC(getc)(FILE* stream) { return fgetc(stream); }
int _RTL_FUNC(getchar)(void) { return fgetc(stdin); }
int _RTL_FUNC fgetchar(void) { return fgetc(stdin); }
int _RTL_FUNC _fgetchar(void) { return fgetc(stdin); }

int _RTL_FUNC _fgetc_unlocked(FILE* stream) { return fgetc_unlocked(stream); }
int _RTL_FUNC(getc_unlocked)(FILE* stream) { return fgetc_unlocked(stream); }
int _RTL_FUNC(getchar_unlocked)(void) { return fgetc_unlocked(stdin); }
int _RTL_FUNC fgetchar_unlocked(void) { return fgetc_unlocked(stdin); }
int _RTL_FUNC _fgetchar_unlocked(void) { return fgetc_unlocked(stdin); }
