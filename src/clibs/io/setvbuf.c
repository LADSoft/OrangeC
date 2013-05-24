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
#include <stdlib.h>
int _RTL_FUNC setvbuf(FILE *restrict stream, char *restrict buf, int mode, size_t size)
{
    if (stream->token != FILTOK || !(stream->flags & _F_VBUF))
    {
        return 1;
    }
    switch (mode) {
        case _IOFBF:
        case _IOLBF:
        case _IONBF:
            break;
        default:
            return 1;
    }
    if (fflush(stream))
    {
        stream->flags |= _F_VBUF;
        return 1;
    }
    stream->flags |= _F_VBUF;
    if (stream->flags & _F_BUF)
        free(stream->buffer);
    stream->flags &= ~(_F_BUF | _F_LBUF);
    switch (mode) {
        case _IOLBF:
            stream->flags |= _F_LBUF;
        case _IOFBF:
            if (!buf) {
                buf = malloc(size);
                if (!buf)
                    return 1;
                stream->flags |= _F_BUF;
            }
            stream->buffer = buf;
            stream->bsize = size;
            break;
        case _IONBF:
            stream->buffer = 0;
            stream->bsize = 0;
            break;
        default:
            return 1;
    }
    stream->level = 0;
    return 0;
    
}