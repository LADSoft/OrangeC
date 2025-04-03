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
#include <time.h>
#include <memory.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include "libp.h"

extern FILE* _pstreams[_NFILE_];
extern int __maxfiles;

/* so if they use a larger buf size in ascii mode, we are ignoring
 * it in terms of DPMI calls which may not be a good thing...
 */
#define LBSIZE 511

static char localbuf[LBSIZE];

int _RTL_FUNC __flush(int __handle);

static int __flushone(FILE* stream)
{
    int rv = 0, lvl;
    if (stream->token == FILTOK)
    {
        stream->flags &= ~_F_VBUF;
        if (stream->flags & _F_BUFFEREDSTRING)
        {
            if (!(stream->flags & _F_BIN))
            {
                if (stream->extended->flags2 & _F2_DYNAMICBUFFER)
                {
                    if (stream->curp - stream->buffer >= stream->bsize - 2)
                    {
                        char* p = realloc(stream->buffer, stream->bsize * 2);
                        if (p)
                        {
                            stream->level += stream->flags & _F_OUT ? -stream->bsize : stream->bsize;
                            stream->curp = p + (stream->curp - stream->buffer);
                            stream->buffer = p;
                            stream->bsize *= 2;
                        }
                    }
                }
                if (stream->flags & _F_OUT)
                {
                    if (stream->extended->flags2 & _F2_WCHAR)
                    {
                        *(wchar_t*)stream->curp = 0;
                    }
                    else
                    {
                        *stream->curp = 0;
                    }
                }
            }
            stream->hold = 0;
            stream->flags &= ~(_F_UNGETC);
            if (stream->extended->flags2 & _F2_DYNAMICBUFFER)
            {
                if (stream->extended->dynamicBuffer[0])
                    *(void**)stream->extended->dynamicBuffer[0] = stream->buffer;
                int size = 0;
                if (stream->extended->flags2 & _F2_WCHAR)
                {
                    for (size = 0; size < stream->bsize && ((wchar_t*)stream->buffer)[size] != 0; size++)
                        ;
                }
                else
                {
                    for (size = 0; size < stream->bsize && stream->buffer[size] != 0; size++)
                        ;
                }
                if (stream->extended->dynamicBuffer[1])
                    *(int*)stream->extended->dynamicBuffer[1] = size;
            }
            return rv;
        }
        if (stream->buffer)
            if (stream->flags & _F_OUT)
            {
                rv = write(fileno(stream), stream->buffer, stream->bsize + stream->level);
                if (rv < 0)
                {
                    stream->flags |= _F_ERR;
                    errno = ENOSPC;
                    rv = EOF;
                }
                else
                    rv = 0;
                if (eof(fileno(stream)))
                    stream->flags |= _F_EOF;
                stream->level = 0;
            }
            else if (stream->flags & _F_READ)
            {
                if (stream->level > 0)
                {
                    rv = lseek(fileno(stream), -stream->level + 1, SEEK_CUR);
                    if (rv < 0)
                    {
                        stream->flags |= _F_ERR;
                        errno = ENOSPC;
                        rv = EOF;
                    }
                    else
                        rv = 0;
                }
                else
                    rv = 0;
                memset(stream->buffer, 0, stream->bsize);
                stream->level = 0;
            }
        stream->hold = 0;
        stream->curp = stream->buffer;
        stream->flags &= ~(_F_IN | _F_OUT | _F_UNGETC);
        return rv;
    }
    else
    {
        errno = _dos_errno = ENOENT;
        return EOF;
    }
}
int _RTL_FUNC fflush(FILE* stream)
{
    int rv = 0;
    if (stream)
    {
        flockfile(stream);
        rv = __flushone(stream);
        funlockfile(stream);
    }
    else
    {
        int i;
        for (i = 0; i < __maxfiles; i++)
        {
            flockfile(_pstreams[i]);
            rv |= __flushone(_pstreams[i]);
            funlockfile(_pstreams[i]);
        }
    }
    return rv;
}
int _RTL_FUNC fflush_unlocked(FILE* stream)
{
    int rv = 0;
    if (stream)
    {
        return __flushone(stream);
    }
    else
    {
        int i;
        for (i = 0; i < __maxfiles; i++)
        {
            rv |= __flushone(_pstreams[i]);
        }
    }
    return rv;
}
int _RTL_FUNC flushall(void)
{
    fflush(0);
    return __maxfiles - 3;
}
int _RTL_FUNC _flushall(void)
{
    fflush(0);
    return __maxfiles - 3;
}
