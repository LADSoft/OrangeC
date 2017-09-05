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
#include <string.h>
#include <io.h>
#include "libp.h"
#include <fcntl.h>

int __getmode(int __handle);

int __readbuf(FILE *stream)
{
    int sz,sz2;
    int oldmode;
    if (stream->flags & _F_XEOF) {
        stream->flags |= _F_EOF ;
        return EOF ;
    }
    if (stream->buffer) {
        /* if a buffer, attempt to read the whole thing... */
        stream->curp = stream->buffer;
        sz2 = stream->bsize;
    }
    else {
        /* if no buffer, we blit one byte into stream->hold */
        stream->curp = &stream->hold;
        sz2 = 1;
    }
    oldmode = __getmode(fileno(stream));
    setmode(fileno(stream), O_BINARY);
    stream->level = sz = read(fileno(stream),stream->curp,sz2);
    setmode(fileno(stream), oldmode);
    /* get out on error */
    if (sz < 0) {
        stream->flags |= _F_ERR;
        errno = EIO;
        return EOF;
    }
    /* get out on EOF */
    else if (sz == 0) {
        stream->flags |= _F_EOF;
        return EOF;
    }
    else {
        if (!(__getmode(fileno(stream)) & O_BINARY)) {
            if (stream->buffer) {
                /* look for CTRL-Z */
                unsigned char *p = stream->buffer ;
                int i ;
                p = memchr(stream->buffer, 0x1a, stream->level);
                if (p)
                {
                    stream->level = p-stream->buffer;
                    stream->flags |= _F_XEOF ;
                }
            } else
                if (stream->hold == 0x1a) {
                    stream->flags |= _F_EOF ;
                    return EOF ;
                }
        }
    }
    return 0;
}
int _RTL_FUNC fgetc(FILE *stream)
{
    int rv;
    int binary;
    if (stream->token != FILTOK) {
        errno = _dos_errno = ENOENT;
        return EOF;
    }
    if (stream->extended->orient == __or_wide) {
        errno = EINVAL;
        return EOF;
    }
    stream->extended->orient = __or_narrow;
    if (stream->flags & _F_EOF)
        return EOF;
    stream->flags &= ~_F_VBUF;
    if (!(stream->flags & _F_READ)) {
        stream->flags |= _F_ERR;
        errno = EFAULT;
        return EOF;
    }
    if (stream == stdin) {
        fflush(stdout);
    }
    if (!(stream->flags & _F_IN)) {
        if (stream->flags & _F_OUT) {
            if (fflush(stream))
                return EOF;
        }
        stream->flags &= ~_F_OUT;
        stream->flags |= _F_IN;
        stream->level = 0;
    }
    if (stream->flags & _F_EOF)
        return EOF;
    if (stream->flags & _F_UNGETC) {
        rv = stream->hold;
        stream->hold = 0;
        stream->flags &= ~ _F_UNGETC;
        return rv;
    }
    binary = (__getmode(fileno(stream)) & O_BINARY) || (stream->flags & _F_BUFFEREDSTRING);
    do {
        if (--stream->level <= 0) {
          if (stream->flags & _F_BUFFEREDSTRING) {
             stream->flags |= _F_EOF ;
             return EOF ;
          }
            rv = __readbuf(stream);
            if (rv)
                return rv;
        }
        rv = *stream->curp++ ;
    } while (rv == '\r' && !binary) ;
    return rv ;
}
int _RTL_FUNC _fgetc(FILE *stream)
{
    return fgetc(stream);
}
int _RTL_FUNC (getc)(FILE *stream)
{
    return fgetc(stream);
}
int _RTL_FUNC (getchar)(void)
{
    return fgetc(stdin);
}
int _RTL_FUNC fgetchar(void)
{
    return fgetc(stdin);
}
int _RTL_FUNC _fgetchar(void)
{
    return fgetc(stdin);
}
