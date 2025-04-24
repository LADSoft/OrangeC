/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include "ctypes.h"
#include "Utils.h"
#include <map>
#include "memory.h"
#ifdef TARGET_OS_WINDOWS
#    include <Windows.h>
#endif

namespace Parser
{
bool IsCompiler();
}
struct MEMORY
{
    MEMBLK* block;
    unsigned used;
};
static MEMORY globals;
static MEMORY locals;
/*static*/ MEMORY opts;
static MEMORY alias;
static MEMORY temps;
static MEMORY live;
static MEMORY templates;
static MEMORY conflicts;

static bool globalFlag = true;
static int globalPeak, localPeak, optPeak, tempsPeak, aliasPeak, livePeak, templatePeak, conflictPeak;

#define MINALLOC ((int)(256 * 1024 - sizeof(MEMBLK)))
#define MALIGN ((int)sizeof(void*))

// #define DEBUG
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
}
static MEMBLK* galloc(MEMORY* arena, int size)
{
    MEMBLK* selected;
    int allocsize = size <= MINALLOC ? MINALLOC : (size + (MINALLOC - 1)) & -MINALLOC;
#ifdef TARGET_OS_WINDOWS
    selected = (MEMBLK*)VirtualAlloc(nullptr, allocsize + sizeof(MEMBLK) - 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    selected = (MEMBLK*)malloc(allocsize + sizeof(MEMBLK) - 1);
#endif
    if (!selected)
    {
        Utils::Fatal("out of memory");
    }
    arena->used += allocsize + sizeof(MEMBLK) - 1;
    selected->size = allocsize;
    selected->left = selected->size;
    selected->next = arena->block;
    arena->block = selected;
    return selected;
}
static void* memAlloc(MEMORY* arena, int size, bool clear = true)
{
    size = size ? size : 1;
    MEMBLK* selected = arena->block;
    void* rv;
    if (!selected || selected->left < size)
    {
        selected = galloc(arena, size);
    }
    rv = (void*)(selected->m + selected->size - selected->left);
#ifndef TARGET_OS_WINDOWS
    if (clear)
    {
        memset(rv, 0, size);
    }
#endif
    selected->left = selected->left - ((size + MALIGN - 1) & -MALIGN);
    return rv;
}
static void memFree(MEMORY* arena, int* peak)
{
    MEMBLK* freefind = arena->block;
    if (!freefind)
        return;
    while (freefind)
    {
        MEMBLK* next = freefind->next;
#ifdef TARGET_OS_WINDOWS
        VirtualFree(freefind, 0, MEM_RELEASE);
#else
        free(freefind);
#endif
        freefind = next;
    }
    arena->block = 0;
    if (arena->used > *peak)
        *peak = arena->used;
    arena->used = 0;
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
    if (!globalFlag && Parser::IsCompiler())
        return memAlloc(&locals, size);
    return memAlloc(&globals, size);
}
void* nzAlloc(int size)
{
    if (!globalFlag && Parser::IsCompiler())
        return memAlloc(&locals, size, false);
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
void SetGlobalFlag(bool flag, bool& old) { old = globalFlag, globalFlag = flag; }
void ReleaseGlobalFlag(bool old) { globalFlag = old; }
bool GetGlobalFlag(void) { return globalFlag; }
char* litlate(const char* name)
{
    int l = strlen(name) + 1;
    char* rv = nzAllocate<char>(l);
    memcpy(rv, name, l);
    return rv;
}
LCHAR* wlitlate(const LCHAR* name)
{
    const LCHAR* p = name;
    int count = 0;
    LCHAR* rv;
    while (*p)
        p++, count++;
    rv = nzAllocate<LCHAR>(count + 1);
    p = name;
    count = 0;
    while (*p)
        rv[count++] = *p++;
    rv[count] = 0;
    return rv;
}