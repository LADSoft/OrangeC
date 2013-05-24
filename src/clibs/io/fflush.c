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
#include <time.h>
#include <memory.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include <io.h>
#include "libp.h"

extern FILE *_pstreams[_NFILE_];
extern int __maxfiles;

/* so if they use a larger buf size in ascii mode, we are ignoring
 * it in terms of DPMI calls which may not be a good thing...
 */
#define LBSIZE 511

static char localbuf[LBSIZE];

static int __flushone(FILE *stream)
{
    int rv = 0, lvl;
    if (stream->token == FILTOK) {
        stream->flags &= ~_F_VBUF;
        if (stream->buffer)
            if (stream->flags & _F_OUT) {
                rv = write(fileno(stream),stream->buffer,stream->bsize + stream->level);
                if (rv < 0) {
                    stream->flags |= _F_ERR;
                    errno = ENOSPC;
                    rv = EOF;
                }
                else rv = 0;
                if (eof(fileno(stream)))
                    stream->flags |= _F_EOF;
                stream->level = 0;
            }
            else if (stream->flags & _F_READ) {
                if (stream->level > 0) {
                    rv = lseek(fileno(stream),-stream->level+1,SEEK_CUR);
                    if (rv < 0) {
                        stream->flags |= _F_ERR;
                        errno = ENOSPC;
                        rv = EOF;
                    }
                    else
                        rv = 0;
                }
                else rv = 0;
                memset(stream->buffer,0,stream->bsize);
                stream->level = 0;
            }
        stream->hold = 0;
        stream->curp = stream->buffer;
        stream->flags &= ~(_F_IN | _F_OUT | _F_UNGETC);
        return rv;			
    }
    else {
        errno = _dos_errno = ENOENT;
        return EOF;
   }
}
int _RTL_FUNC fflush(FILE *stream)
{
    int rv=0;
    if (stream) {
        return __flushone(stream);
    } else {
        int i;
        for (i=0; i < __maxfiles; i++) {
            rv |= __flushone(_pstreams[i]);
        }
    }
    return rv;
}
int _RTL_FUNC flushall(void)
{
   fflush(0) ;
   return __maxfiles -3 ;
}
int _RTL_FUNC _flushall(void)
{
   fflush(0) ;
   return __maxfiles -3 ;
}
