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
#include <io.h>
#include <fcntl.h>

int __getmode(int __handle);
int __readbuf(FILE *stream);

char *_RTL_FUNC fgets(char *restrict buf, int num, FILE *restrict stream)
{
    int i = 0,rv = 0;
    if (stream->token != FILTOK) {
        errno = _dos_errno = ENOENT;
        return 0;
    }
    if (stream->extended->orient == __or_wide) {
        errno = EINVAL;
        return 0;
    }
    stream->extended->orient = __or_narrow;
    stream->flags &= ~_F_VBUF;
    if (!(stream->flags & _F_READ)) {
        stream->flags |= _F_ERR;
        errno = EFAULT;
        return 0;
    }
    if (stream == stdin)
        fflush(stdout);
    if (!(stream->flags & _F_IN)) {
        if (stream->flags & _F_OUT) {
            if (fflush(stream))
                return 0;
        }
        stream->flags &= ~_F_OUT;
        stream->flags |= _F_IN;
        stream->level = 0;
    }
    if (stream->flags & _F_EOF)
        return 0;
    if (num == 0)
        return 0 ;
    if (stream->flags & _F_UNGETC) {
        rv = buf[i++] = stream->hold ;
        stream->hold = 0;
        stream->flags &= ~_F_UNGETC;
    }
    if (rv != '\n')
    {
        for (;i < num-1;) {
            if (--stream->level <= 0) {
              if (stream->flags & _F_BUFFEREDSTRING) {
                 stream->flags |= _F_EOF ;
                 rv = EOF ;
                 break ;
              }
                rv = __readbuf(stream);
                if (rv)
                    break ;
            }
            rv = *stream->curp++ ;
            if (rv != '\r' || (__getmode(fileno(stream)) & O_BINARY))
                buf[i++] = rv ;
            if (rv == '\n') {
                break ;
            }
        }
    }
    if (rv == EOF && i == 0)
        return 0;
    buf[i] = 0;
    return buf;
}
