/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <stdbool.h>
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

static bool init_block(size_t size)
{
    size_t ns = size+sizeof(BLKHEAD);
    BLKHEAD *nb;
    FREELIST *list ;
    if (ns < ALLOCSIZE)
        ns = ALLOCSIZE;
    nb = __ll_malloc(ns);
    if (!nb)
        return false;
    nb->next = _allocbloc;
    _allocbloc = nb;
    list = nb->freemem = nb + 1 ;
    list->size = ns - sizeof(BLKHEAD) ;
    list->next = 0;
    _newfree = nb;
    return true;
}
static FREELIST *split(FREELIST *p, int size, FREELIST **slist)
{
    FREELIST *rv;
    rv = (FREELIST *)(((char *)p)+size);
    rv->size = p->size - size ;
    rv->next = 0;
    *slist = rv;
    p->next = 0;
    p->size = size ;
     return ++p;
}
void * _RTL_FUNC malloc(size_t size)
{
    FREELIST **slist;
    register FREELIST *p;
    register int siz1 ;
    if (!size)
        return NULL;
    size += 7;	/* must be the same as in realloc for comparison purposes */
    size &= MALLOC_MASK;

    siz1 = size + sizeof(FREELIST);

   __ll_enter_critical() ;
    if (!__mallocchains[0])
    {
        for (int i=0; i < MEMCHAINS; i++)
        {
            __mallocchains[i] = 1; // nonzero to help the 'double free' situation
        }
    }
    /* search the chain for reuse */
    slist = &__mallocchains[(siz1 >> 2) % MEMCHAINS];
    while (*slist != 1) {
        p = *slist;
        if (p->size == siz1) {
             *slist = p->next;
             p->next = NULL;
         __ll_exit_critical() ;
             return ++p;
        }
        slist = &p->next;
    }
    /* search the free blocks for free mem */
    for (BLKHEAD* head = _newfree; head != NULL; head = head->next ) {
        p = head->freemem ;
        if (p) {
            if (p->size == siz1) {
                 head->freemem = 0;
                 p->next = NULL;
            __ll_exit_critical() ;
                 return ++p;
            }
         else if (p->size > siz1+sizeof(FREELIST)) {
            p = split(p,siz1,&head->freemem);
            __ll_exit_critical() ;
            return p ;
         }
        }
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
             p->next = NULL;
         __ll_exit_critical() ;
             return ++p;
    }

   p = split(p,siz1,&_newfree->freemem);
   __ll_exit_critical() ;
   return p ;
}