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
#include <cstddef>
#include "_rtti.h"
#include <string.h>
#include <stdio.h>

typedef unsigned char BYTE;
                        
static RTTISTRUCT *getStructData(void *xt)
{
    RTTI *p = (RTTI *)xt;
    BYTE *b = (BYTE *)p->name + strlen(p->name) + 1;
    return (RTTISTRUCT *)b;
}

static int findoffs(RTTI *oldxt, RTTI *newxt, int *count)
{
    if (oldxt == newxt)
    {
        (*count)++;
        return 0;
    }
    else
    {
        RTTISTRUCT *oldrs = getStructData(oldxt);
        int offset =-1;
        while (oldrs->flags) {
          if (oldrs->flags & XD_CL_BASE) {
             if (oldrs->xt == newxt) {
                offset =  oldrs->offset ;
                (*count)++;
             } else {
                int offset1 = findoffs(oldrs->xt, newxt, count) ;
                if (offset1 != -1)
                    offset = oldrs->offset + offset1;
             }
          }
          oldrs++;
        }
        return offset ;
    }
}
void * _RTL_FUNC __dynamic_cast(void *__instance, void *__xceptptr, void *__oldxt, void *__newxt)
{
    BYTE  *vtab;
    RTTI *instancext;
    if (!__instance)
        return __instance;
    
    int offset = ((int *)__xceptptr)[-2];
    __instance = (void *)( ((BYTE *)__instance) - offset);
    // conversion to pointer to void gets outermost base
    if (!__newxt)
    {
        return __instance;
    }
    
    offset = ((int *)__xceptptr)[-1];
    vtab = (BYTE *)__xceptptr - offset - VTAB_XT_OFFS; // get vtab pointer
    instancext = *(RTTI **)vtab;
    if (!instancext)
        return NULL;

    int count = 0;
    // try to find an offset for the func
    offset = findoffs((RTTI *)instancext, (RTTI *)__newxt, &count) ;

    // if we found it
    if (count == 1)
    {
        return ((BYTE *)__instance) + offset;
    }
    // no unique match, signal failure
    return NULL ;
}