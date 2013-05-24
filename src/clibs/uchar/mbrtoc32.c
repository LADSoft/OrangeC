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


size_t mbrtoc32 (char32_t *restrict pc32, const char *restrict s, size_t n, mbstate_t *restrict p)
{
  size_t used = 0;
  unsigned char b;

  if (s == NULL) {
    return mbrtoc32(NULL,"",1,p);
  }
  if (p == NULL)
    p = &__getRtlData()->mbrtowc_st;

  if (n <= 0)
    return (size_t)-2; 

  if (p->left == 0) {
    b = (unsigned char)*s++;
    used++;
    if (b < 0x80) {
        if (pc32)
            *pc32 = (wchar_t)b;
        return b ? 1 : 0;
    }

    if ((b & 0xc0) == 0x80 || b == 0xfe || b == 0xff) {
        errno = EILSEQ ;
        return (size_t) -1;
    }
    b <<= 1;
    while (b & 0x80) {
        p->left++ ;
        b <<= 1;
    }
    p->value = b >> (p->left + 1);
  }
  while (used < n) {
      b = (unsigned char) *s++;
      used++;
      if ((b & 0xc0) != 0x80) {
         errno = EILSEQ ;
         return (size_t) -1;
      }

      p->value <<= 6;
      p->value += b & 0x3f;

      if (--p->left == 0) {
          if (pc32 != NULL)
            *pc32 = (char32_t) p->value;
          return p->value ? used : 0;
      }
  }

  return (size_t) -2;
}
