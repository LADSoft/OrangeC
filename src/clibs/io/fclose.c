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
#include <errno.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "io.h"
#include "libp.h"

extern int _abterm;

FILE *_pstreams[_NFILE_];
int __maxfiles;

FILE _RTL_DATA *__stdin ;
FILE _RTL_DATA *__stdout ;
FILE _RTL_DATA *__stderr ;
FILE _RTL_DATA *__stdaux ;
FILE _RTL_DATA *__stdprn ;

#pragma startup __fileinit 226
#pragma rundown __closeall 10

void __fileinit(void)
{
    __ll_init();
   __stdin = __getStream(0) ;
   __stdout = __getStream(1) ;
   __stderr = __getStream(2) ;
}

void __closeall(void)
{
   if (!_abterm) {
         fcloseall() ;
   }
}
int _RTL_FUNC fcloseall(void)
{
    int i;
    int rv = __maxfiles - 3 ;
    for (i= __maxfiles-1; i >=3; i--)
        fclose(_pstreams[i]) ;
    __maxfiles = 3 ;
    return rv ;	
}
int _RTL_FUNC _fcloseall(void)
{
    return fcloseall();
}
int __basefclose(FILE *stream,int release)
{
    int rv,i;
    
    if (stream->token == FILTOK && __maxfiles) {
        char *fname = stream->name;
        stream->flags &= ~_F_VBUF;
        fflush(stream);
        stream->token = (short)-1;
        if (__maxfiles > 1) {
            __ll_enter_critical() ;
            for (i=0; i < __maxfiles; i++)
                if (_pstreams[i] == stream) {
                    _pstreams[i] = _pstreams[__maxfiles-1];
                    __maxfiles--;
                    break ;
                }
            __ll_exit_critical() ;
        }
        rv = close(fileno(stream));
        if (fname)
            free(fname);
        if (release) {
            if (stream->flags & _F_BUF)
                free(stream->buffer);
            free(stream);
        }
        if (!rv)
            return 0;
        else {
            return EOF;
        }
    }
    else {
        errno = _dos_errno = ENOENT;
        return EOF;
    }
}
int _RTL_FUNC fclose(FILE *stream)
{
    return __basefclose(stream,1);
}