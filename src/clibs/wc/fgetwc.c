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
#include <wchar.h>
#include <locale.h>
#include "libp.h"

static int __wreadbuf(FILE* stream)
{
    int sz, sz2;
    int handle;
    if (stream->flags & _F_XEOF)
    {
        stream->flags |= _F_EOF;
        return EOF;
    }
    handle = __uiohandle(fileno(stream));
    if (handle == -1)
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
    stream->level = sz = __ll_read(handle, stream->curp, sz2);
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
        if (!(stream->flags & _F_BIN))
        {
            if (stream->buffer)
            {
                /* look for CTRL-Z */
                unsigned char* p = stream->buffer;
                int i;
                for (i = 0; i < stream->level && *p != 0x1a; i++)
                    p++;
                stream->level = i;
                if (*p == 0x1a)
                    stream->flags |= _F_XEOF;
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

int _RTL_FUNC __wgetch(FILE* stream)
{
    int rv;
    if (--stream->level <= 0)
    {
        if (stream->flags & _F_BUFFEREDSTRING)
        {
            stream->flags |= _F_EOF;
            return WEOF;
        }
        rv = __wreadbuf(stream);
        if (rv)
            return WEOF;
    }
    rv = *stream->curp++;
    return rv;
}
int _RTL_FUNC __wgetc(FILE* stream)
{
    int one, two;
    one = __wgetch(stream);
    if (one == WEOF)
        return one;
    two = __wgetch(stream);
    if (two == WEOF)
    {
        errno = EILSEQ;
        return WEOF;
    }
#ifndef __BIG_ENDIAN
    if (stream->flags & _F_BUFFEREDSTRING)
    {
        return (two << 8) + one;
    }
#endif
    return (one << 8) + two;
}
wint_t _RTL_FUNC fgetwc(FILE* stream)
{
    flockfile(stream);
    wint_t rv = fgetwc_unlocked(stream);
    funlockfile(stream);
    return rv;
}
wint_t _RTL_FUNC fgetwc_unlocked(FILE* stream)
{
    int rv;
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
    if (stream->flags & _F_EOF)
        return WEOF;
    if (stream == stdin)
    {
        fflush(stdout);
    }
    if (stream->hold)
    {
        rv = stream->hold;
        stream->hold = 0;
        return rv;
    }
    if (!(stream->flags & _F_IN))
    {
        if (stream->flags & _F_OUT)
        {
            if (fflush(stream))
                return 0;
        }
        if (stream->flags & _F_BUFFEREDSTRING)
        {
            if (stream->flags & _F_OUT)
                stream->level = -stream->level;
        }
        else
        {
            stream->level = stream->bsize;
            stream->curp = stream->buffer;
        }
        stream->flags &= ~_F_OUT;
        stream->flags |= _F_IN;
    }
    do
    {
        rv = __wgetc(stream);
    } while (rv != WEOF && (rv == '\r' && !(stream->flags & _F_BIN)));
    return rv;
}
wint_t _RTL_FUNC _fgetwc(FILE* stream) { return fgetwc(stream); }
wint_t _RTL_FUNC(getwc)(FILE* stream) { return fgetwc(stream); }
wint_t _RTL_FUNC(getwchar)(void) { return fgetwc(stdin); }

wint_t _RTL_FUNC _fgetwc_unlocked(FILE* stream) { return fgetwc_unlocked(stream); }
wint_t _RTL_FUNC(getwc_unlocked)(FILE* stream) { return fgetwc_unlocked(stream); }
wint_t _RTL_FUNC(getwchar_unlocked)(void) { return fgetwc_unlocked(stdin); }
