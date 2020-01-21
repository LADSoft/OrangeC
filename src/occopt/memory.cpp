/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
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

#include "ctypes.h"
#include "Utils.h"
#include <map>
    
static MEMBLK* globals;
static MEMBLK* locals;
/*static*/ MEMBLK* opts;
static MEMBLK* alias;
static MEMBLK* temps;
static MEMBLK* live;
static MEMBLK* templates;
static MEMBLK* conflicts;

static std::map<int, unsigned> vv;
static std::map<std::string, unsigned> xx;
static bool globalFlag=true;
static int globalPeak, localPeak, optPeak, tempsPeak, aliasPeak, livePeak, templatePeak, conflictPeak;

#define MINALLOC (128 * 1024)
#define MALIGN (4)

//#define DEBUG
void memfunc(const char *a)
{
//    xx[a]++;
}
void mem_summary(void)
{
    printf("Memory used:\n");
    printf("\tGlobal Peak %dK\n", (globalPeak + 1023) / 1024);
    printf("\tLocal peak %dK\n", (localPeak + 1023) / 1024);
    printf("\tTemplate peak %dK\n", (templatePeak + 1023) / 1024);
    printf("\tOptimizer peak %dK\n", (optPeak + 1023) / 1024);
    printf("\tTemporary peak %dK\n", (tempsPeak + 1023) / 1024);
    printf("\tAlias peak %dK\n", (aliasPeak + 1023) / 1024);
    printf("\tLive peak %dK\n", (livePeak + 1023) / 1024);
    printf("\tConflict peak %dK\n", (conflictPeak + 1023) / 1024);
    globalPeak = localPeak = optPeak = tempsPeak = aliasPeak = livePeak = conflictPeak = 0;
for (auto v : vv)
	printf("%d, %d\n", v.first, v.second);
for (auto v : xx)
    printf("%s, %d\n", v.first.c_str(), v.second);
}
static MEMBLK* galloc(MEMBLK** arena, int size)
{
    MEMBLK* selected;
    int allocsize = size <= MINALLOC ? MINALLOC : (size + (MINALLOC - 1)) & -MINALLOC;
    selected = (MEMBLK*)malloc(allocsize + sizeof(MEMBLK) - 1);
    if (!selected)
        Utils::fatal("out of memory");
    selected->size = allocsize;
    selected->left = selected->size;
    selected->next = *arena;
    *arena = selected;
    return selected;
}
void* memAlloc(MEMBLK** arena, int size, bool clear = true)
{
//vv[size] += size;
    MEMBLK* selected = *arena;
    void* rv;
    if (!selected || selected->left < size)
    {
        selected = galloc(arena, size);
    }
    rv = (void*)(selected->m + selected->size - selected->left);
    if (clear)
	memset(rv, 0, size);
    selected->left = selected->left - ((size + MALIGN - 1) & -MALIGN);
    return rv;
}
void memFree(MEMBLK** arena, int* peak)
{
    MEMBLK* freefind = *arena;
    long size = 0;
    if (!freefind)
        return;
    while (freefind)
    {
        MEMBLK* next = freefind->next;
        size += freefind->size;
        free(freefind);
        freefind = next;
    }
    *arena = 0;
    if (size > *peak)
        *peak = size;
}
void* globalAlloc(int size) { return memAlloc(&globals, size); }
void globalFree(void)
{
    memFree(&globals, &globalPeak);
    globalFlag = 1;
}
void* localAlloc(int size) { return memAlloc(&locals, size); }
void localFree(void) { memFree(&locals, &localPeak); }
void* Alloc(int size)
{
#ifndef PARSER_ONLY
    if (!globalFlag)
        return memAlloc(&locals, size);
#endif
    return memAlloc(&globals, size);
}
void* nzAlloc(int size)
{
#ifndef PARSER_ONLY
    if (!globalFlag)
        return memAlloc(&locals, size, false);
#endif
    return memAlloc(&globals, size, false);
}
void* oAlloc(int size) { return memAlloc(&opts, size); }
void oFree(void) { memFree(&opts, &optPeak); }
void* aAlloc(int size) { return memAlloc(&alias, size); }
void aFree(void) { memFree(&alias, &aliasPeak); }
void* tAlloc(int size) { return memAlloc(&temps, size); }
void tFree(void) { memFree(&temps, &tempsPeak); }
void* cAlloc(int size) { return memAlloc(&conflicts, size); }
void cFree(void) { memFree(&conflicts, &conflictPeak); }
void* sAlloc(int size) { return memAlloc(&live, size); }
void sFree(void) { memFree(&live, &livePeak); }
void SetGlobalFlag(bool flag, bool &old) { old = globalFlag, globalFlag = flag; }
void ReleaseGlobalFlag(bool old) { globalFlag = old;  }
bool GetGlobalFlag(void) { return globalFlag; }
char* litlate(const char* name)
{
    int l;
    char* rv = (char*)nzAlloc((l = strlen(name)) + 1);
    strcpy(rv, name);
    return rv;
}
LCHAR* wlitlate(const LCHAR* name)
{
    const LCHAR* p = name;
    int count = 0;
    LCHAR* rv;
    while (*p)
        p++, count++;
    rv = (LCHAR*)nzAlloc((count + 1) * sizeof(LCHAR));
    p = name;
    count = 0;
    while (*p)
        rv[count++] = *p++;
    rv[count] = 0;
    return rv;
}