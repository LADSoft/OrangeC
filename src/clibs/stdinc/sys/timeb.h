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
/*  timeb.h

    Struct and function declarations for ftime().

*/

#if !defined(__TIMEB_H)
#define __TIMEB_H

#pragma pack(1)

#ifndef __DEFS_H__
#include <_defs.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

struct timeb {
      long  time;
      short millitm;
      short timezone;
      short dstflag;
      };

struct _timeb {
      long  time;
      short millitm;
      short timezone;
      short dstflag;
      };

void _RTL_FUNC _IMPORT _ftime(struct _timeb *);
void _RTL_FUNC _IMPORT ftime(struct timeb *);
#ifdef __cplusplus
} ;
} ;
#endif

#pragma pack()
#endif  /* __TIMEB_H */
#if defined(__cplusplus) && !defined(__USING_CNAMES__) && !defined(__TIMEB_H_USING_LIST)
using __STD_NS_QUALIFIER timeb; 
using __STD_NS_QUALIFIER ftime;
using __STD_NS_QUALIFIER _timeb; 
using __STD_NS_QUALIFIER _ftime;
#endif
