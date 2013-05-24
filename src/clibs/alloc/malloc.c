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
*/#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "libp.h"

FREELIST *__mallocchains[MEMCHAINS];

BLKHEAD *_allocbloc;
BLKHEAD *_newfree;

#pragma rundown memdelete 2

static void memdelete(void)
{
   BLKHEAD *p ;
   __ll_enter_critical() ;
   p = _allocbloc;
    while (p) {
        BLKHEAD *n = p->next;
        __ll_free(p);
        p = n;
    }
   __ll_exit_critical() ;
}

static int init_block(size_t size)
{
    size_t ns = size+sizeof(BLKHEAD);
    BLKHEAD *nb;
    FREELIST *list ;
    if (ns < ALLOCSIZE)
        ns = ALLOCSIZE;
    nb = __ll_malloc(ns);
    if (!nb)
        return NULL;
    nb->next = _allocbloc;
    _allocbloc = nb;
    list = nb->freemem = nb + 1 ;
    list->size = ns - sizeof(BLKHEAD) ;
    list->next = 0;
    _newfree = nb;
    return 1;
}
static FREELIST *split(FREELIST *p, int size, FREELIST **slist)
{
    FREELIST *rv;
    int ch;
    rv = (FREELIST *)(((char *)p)+size);
    rv->size = p->size - size ;
    rv->next = 0;
    *slist = rv;
    p->size = size ;
     return ++p;
}
void * _RTL_FUNC malloc(size_t size)
{
    BLKHEAD *head ;
    FREELIST **slist;
    register FREELIST *p;
    register int siz1 ;
    if (!size)
        return NULL;
    size +=3;	/* must be the same as in realloc for comparison purposes */
    size &= 0xfffffffc;

    siz1 = size + sizeof(FREELIST);

   __ll_enter_critical() ;
    /* search the chain for reuse */
    slist = &__mallocchains[(siz1 >> 2) % MEMCHAINS];
    while (*slist) {
        p = *slist;
        if (p->size == siz1) {
             *slist = p->next;
         __ll_exit_critical() ;
             return ++p;
        }
        slist = &p->next;
    }
    /* search the free blocks for free mem */
    head = _newfree ;
    while (head) {
        p = head->freemem ;
        if (p) {
            if (p->size == siz1) {
                 head->freemem = 0;
            __ll_exit_critical() ;
                 return ++p;
            }
         else if (p->size > siz1+sizeof(FREELIST)) {
            p = split(p,siz1,&head->freemem);
            __ll_exit_critical() ;
            return p ;
         }
        }
        head = head->next ;
    }
    /* now get a new block */
    if (!init_block(siz1)) {
      __ll_exit_critical() ;
        return NULL;
    }
    /* and split it up to get our block */
    p = _newfree->freemem;
    if (p->size == siz1) {
             _newfree->freemem = p->next ;
         __ll_exit_critical() ;
             return ++p;
    }

   p = split(p,siz1,&_newfree->freemem);
   __ll_exit_critical() ;
   return p ;
}