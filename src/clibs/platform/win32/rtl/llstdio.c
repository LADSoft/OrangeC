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
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int __maxfiles;
extern FILE *_pstreams[] ;

static unsigned char inbuf[512];

FILE _istreams[3] = { 
   { 0, _F_READ | _F_LBUF | _F_TERM,0,0,512,inbuf,inbuf,"CON:",FILTOK },
   { 0, _F_WRIT | _F_LBUF | _F_TERM,1,0,0,0,0,"CON:",FILTOK },
   { 0, _F_WRIT | _F_LBUF | _F_TERM,2,0,0,0,0,"CON:",FILTOK }
} ;

#undef stdin
#undef stdout
#undef stderr

#define stdin (&_istreams[0])
#define stdout (&_istreams[1])
#define stderr (&_istreams[2])

extern int __maxfiles;

FILE *_RTL_FUNC __getStream(int stream)
{
   return _pstreams[stream] ;
}

void __ll_init(void)
{
   static int done ;
   if (!done) {
      _pstreams[0] = &_istreams[0] ;
      _pstreams[1] = &_istreams[1] ;
      _pstreams[2] = &_istreams[2] ;
      __maxfiles = 3;
      done = 1 ;
   }
}
