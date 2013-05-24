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

#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <uchar.h>
#include "libp.h"

size_t c32rtomb (char *restrict s, char32_t c32, mbstate_t *restrict ps)
{
  char xx[10];

  if (s == NULL) {
        return c32rtomb(xx,U'\0',ps);
  }
  if (ps == NULL)
    ps = &__getRtlData()->wcrtomb_st;

  if (c32 < 0x80)
    {
      if (s != NULL)
        *s = (char) c32;
      return 1;
    }

  if ((unsigned) c32 <= 0x7ff) {
    if (s != NULL) {
        *s++ = 0xc0 + (c32 >> 6) ;
        *s++ = (c32 & 0x3f) | 0x80 ;
    }
    return 2;
  }
  if ((unsigned) c32 <= 0xffff) {
    if (s != NULL) {
        *s++ = 0xe0 + (c32 >> 12) ;
        *s++ = ((c32 >> 6) & 0x3f) | 0x80 ;
        *s++ = (c32 & 0x3f) | 0x80 ;
      
    }
    return 3;
  }
    if (s != NULL) {
        *s++ = 0xf0 + ((c32 >> 18) & 7) ;
        *s++ = ((c32 >> 12) & 0x3f) | 0x80 ;
        *s++ = ((c32 >> 6) & 0x3f) | 0x80 ;
        *s++ = (c32 & 0x3f) | 0x80 ;
      
    }
    return 4;
}
