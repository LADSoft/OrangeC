/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "compiler.h"

static MEMBLK *freemem;
static MEMBLK *freestdmem;
static MEMBLK *globals;
static MEMBLK *locals;
static MEMBLK *opts;
static MEMBLK *alias;
static MEMBLK *temps;
static MEMBLK *live;

static int globalFlag;
static int globalPeak, localPeak, optPeak, tempsPeak, aliasPeak, livePeak;

#define MINALLOC (4 * 1024)
#define MALIGN (4)

//#define DEBUG

void mem_summary(void)
{
    printf("Memory used:\n");
    printf("\tGlobal Peak %dK\n", (globalPeak + 1023)/1024);
    printf("\tLocal peak %dK\n", (localPeak+1023)/1024);
    printf("\tOptimizer peak %dK\n", (optPeak+ 1023)/1024);
    printf("\tTemporary peak %dK\n", (tempsPeak+ 1023)/1024);
    printf("\tAlias peak %dK\n", (aliasPeak + 1023)/1024);
    printf("\tLive peak %dK\n", (livePeak + 1023)/1024);
    globalPeak = localPeak = optPeak = tempsPeak = aliasPeak = livePeak = 0;
}
static MEMBLK *galloc(MEMBLK **arena, int size)
{
    MEMBLK *selected;
    int allocsize = size <= MINALLOC ? MINALLOC : (size + (MINALLOC-1)) & -MINALLOC;
    selected = NULL;
    if (allocsize == MINALLOC)
    {
        if (freestdmem)
        {
            selected = freestdmem;
            freestdmem = freestdmem->next;
            selected->left = selected->size;
        }
    }
    else
    {
        MEMBLK **free = &freemem;
        while (*free)
        {
            if (((*free)->size) >=  allocsize)
            {
                selected = *free;
                *free = (*free)->next;
                selected->left = selected->size;
                break;
            }
            free = &(*free)->next;
        }
    }
    if (!selected)
    {
        selected = (MEMBLK *)malloc(allocsize + sizeof(MEMBLK) - 1);
        if (!selected)
            fatal("out of memory");
        selected->size = selected->left = allocsize;
    }
#ifdef DEBUG
    memset(selected->m, 0xc4, selected->left);
#else
    memset(selected->m, 0, selected->left);
#endif
    selected->next = *arena;
    *arena = selected;
    return selected;
}
void *memAlloc(MEMBLK **arena, int size)
{
    MEMBLK *selected = *arena;
    void *rv;
    if (!selected || selected->left < size)
    {
        selected = galloc(arena, size);
    }
    rv = (void *)(selected->m + selected->size -selected->left);
#ifdef DEBUG
    memset(rv, 0, size);
#endif
    selected->left = selected->left - ((size + MALIGN - 1) & -MALIGN);
    return rv;
}
void memFree(MEMBLK **arena, int *peak)
{
    MEMBLK *freefind = *arena;
    long size = 0;
    if (!freefind)
        return;
    while(freefind)
    {
        MEMBLK *next = freefind->next;
#ifdef DEBUG
        memset(freefind->m, 0xc4, freefind->size);
#endif
        size += freefind->size;
        if (freefind->size == MINALLOC)
        {
            freefind->next = freestdmem;
            freestdmem = freefind;
        }
        else
        {
            freefind->next = freemem;
            freemem = freefind;
        }		
        freefind = next;
    }
    *arena = 0;
    if (size > *peak)
        *peak = size;
}
void *globalAlloc(int size)
{
    return memAlloc(&globals, size);
}
void globalFree(void)
{
    memFree(&globals, &globalPeak);
    globalFlag = 1;
}
void *localAlloc(int size)
{
    return memAlloc(&locals, size);
}
void localFree(void)
{
    memFree(&locals,  &localPeak);
}
void *Alloc( int size)
{
    if (!globalFlag)
        return memAlloc(&locals, size);
    else
        return memAlloc(&globals, size);
}
void *oAlloc(int size)
{
    return memAlloc(&opts, size);
}
void oFree(void)
{
    memFree(&opts, &optPeak);
}
void *aAlloc(int size)
{
    return memAlloc(&alias, size);
}
void aFree(void)
{
    memFree(&alias, &aliasPeak);
}
void *tAlloc(int size)
{
    return memAlloc(&temps, size);
}
void tFree(void)
{
    memFree(&temps, &tempsPeak);
}
void *sAlloc(int size)
{
    return memAlloc(&live, size);
}
void sFree(void)
{
    memFree(&live, &livePeak);
}
void IncGlobalFlag(void)
{
    globalFlag ++;
}
void DecGlobalFlag(void)
{
    globalFlag --;
}
void SetGlobalFlag(int flag)
{
    globalFlag = flag;
}
int GetGlobalFlag(void)
{
    return globalFlag;
}
char *litlate(char *name)
{
    int l;
    char *rv = Alloc((l=strlen(name)) + 1);
    memcpy(rv, name, l);
    return rv;
}
LCHAR *wlitlate(LCHAR *name)
{
    LCHAR *p = name;
    int count=0;
    LCHAR *rv;
    while (*p)
        p++, count++;
    IncGlobalFlag();
    rv = (LCHAR *)Alloc((count + 1) * sizeof(LCHAR));
    DecGlobalFlag();
    p = name;
    count = 0;
    while (*p)
        rv[count++] = *p++;
    rv[count] = 0;
    return rv;
}