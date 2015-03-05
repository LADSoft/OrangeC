/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include "libp.h"

int _RTL_FUNC fputc(int c, FILE *stream)
{
    int rv;
    if (stream->token != FILTOK) {
        errno= _dos_errno = ENOENT;
        return EOF;
    }
    if (stream->extended->orient == __or_wide) {
        errno = EINVAL;
        return EOF;
    }
    stream->extended->orient = __or_narrow;
    stream->flags &= ~_F_VBUF;
    if (!(stream->flags & _F_WRIT)) {
        stream->flags |= _F_ERR;
        errno = EFAULT;
        return EOF;
    }
    if ((stream->flags & _F_IN) || 
            stream->buffer && ( stream->flags & _F_OUT) 
                && stream->level >= 0) {
        if (fflush(stream))
            return EOF;
        goto join;
    }
    else {
        if (!(stream->flags & _F_OUT)) {
join:
            stream->flags &= ~_F_IN;
            stream->flags |= _F_OUT;
            stream->level = -stream->bsize;
            stream->curp = stream->buffer;
        }
    }
    if (stream->buffer) {
        if (stream->level < 0) // if a buffered string don't write beyond the end
        {
            ++stream->level;
            *stream->curp++ = (unsigned char)c;
        }
        if (!(stream->flags & _F_BUFFEREDSTRING))
        {
            if (stream->level >= 0 || c == '\n' && (stream->flags & _F_LBUF)) {
                if (fflush(stream))
                    return EOF ;
                stream->flags &= ~_F_IN;
                stream->flags |= _F_OUT;
                stream->level = -stream->bsize;
                stream->curp = stream->buffer;
            }
        }
    }		
    else {
        if (write(fileno(stream),&c,1) < 0) {
            stream->flags |= _F_ERR;
            errno = EIO;
            return EOF;
        }
        if (eof(fileno(stream)))
            stream->flags |= _F_EOF;
    }
    return c;
}
int _RTL_FUNC _fputc(int c, FILE *stream)
{
    return fputc(c,stream);
}
#undef putc
#undef putchar
int _RTL_FUNC putc(const int c, FILE *stream)
{
    return fputc(c,stream);
}
int _RTL_FUNC putchar(const int c)
{
    return fputc(c,stdout);
}
int _RTL_FUNC fputchar(int c)
{
    return fputc(c,stdout);
}
int _RTL_FUNC _fputchar(int c)
{
    return fputc(c,stdout);
}
