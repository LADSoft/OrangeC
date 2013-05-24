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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

char _RTL_DATA **_environ;

#pragma startup envset 225

static void envset(void)
{
   int count = __ll_getenv(0,0),i,j;
   _environ  = calloc(sizeof(char *)*(count+1),1);
   for (i=1,j=0; i<=count; i++)
   {
       int n = __ll_getenvsize(i-1);
       char *p = (char *)malloc(n+1);
       __ll_getenv(p,i-1);
       _environ[j++] = p;
   }
}
