/*
Software License Agreement (BSD License)

Copyright (c) 2016, David Lindauer, (LADSoft).
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

contact information:
email: TouchStone222@runbox.com <David Lindauer>
*/
#include <windows.h>
int PASCAL DllMain(
 	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved
	)
 {
    return TRUE;
 }
#define MAX_HASH 64

struct hash {
	struct hash *next;
	void *funcptr;
	unsigned char thunk[1];
} *table[MAX_HASH];

#pragma pack(1)

static char *dataPointer;
static int left;
static void *NextRecord(int n)
{
    if (left < n)
    {
        dataPointer = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (dataPointer)
        {
            left = 4096;
        }
        else
        {
            left = 0;
        }
    }
    if (left)
    {
        left-= n;
        dataPointer += n;
        return dataPointer - n;
    }
    return NULL;
}
static struct hash *GetThunkRecord(void *proc, int n)
{
    int hashval = (((int)proc)>>2) % MAX_HASH;
    struct hash *link = table[hashval];
    while (link)
    {
        if (link->funcptr == proc)
            return link;
        link = link->next;
    }
    link = NextRecord(n);
    if (link)
    {
        link->next = table[hashval];
        table[hashval] = link;
        return link;
    }
    return NULL;
}
_declspec(dllexport) void * __OCCMSIL_GetProcThunkToManaged(void *proc, unsigned char *pdata)
 {
	static struct thunk1 {       /* must be <= 16 bytes, not sure if this is an MSIL limitation or not */
		unsigned char code[10];
		int saveAddr;
	} thunk1 = {
		0x8B, 0x4C, 0x24, 0x04,     /* mov ecx,DWORD PTR [esp+4] */
		0x8B, 0x54, 0x24, 0x08,     /* mov edx,DWORD PTR [esp+8] */
        0x8f, 0x05,                 /* pop dword ptr [xxxxx] */
        0
    };
    static struct thunk2 {
        unsigned char code[3];
        int offset;
    } thunk2 = 
    {
        0xff, 0xb4, 0x24,             /* push dword ptr [esp + xxxxxx] */
        0
    };
    static struct thunk3 {
        unsigned char code0[1];
        int offset;
        unsigned char code1[1];
        int saveAddr;
        unsigned char code2[1];
        short retsize;
    } thunk3 = 
    {
        0xe8,
        0,
        0x68,                       /* push dword const */
        0,
        0xc2, 
        0
    };
    struct hash * record;
    int n = ((pdata[0] & 0x7f) << 8) + pdata[1], n1 = n;
    BOOL isstdcall = pdata[2];
    int ts;
    if (n < 2)
        n = 0;
    else
        n -= 2;
    
    ts = sizeof(thunk1) + n * sizeof(thunk2) + sizeof(thunk3);
    record = GetThunkRecord(proc, ts);
    if (record)
    {
        if (!record->funcptr)
        {
            struct thunk1 *t1;
            struct thunk2 *t2;
            struct thunk3 *t3;
            int i;
            record->funcptr = proc;
            t1 = (struct thunk1 *)record->thunk;
            t2 = (struct thunk1 *)((char *)t1 + sizeof(thunk1));
            t3 = (struct thunk3 *)((char *)t2 + n * sizeof(thunk2));
            memcpy(t1, &thunk1, sizeof(thunk1));
            for (i=0; i < n; i++)
                memcpy((char *)t2 + i * sizeof(thunk2), &thunk2, sizeof(thunk2));
            memcpy(t3, &thunk3, sizeof(thunk3));
            t1->saveAddr = (int)& t3->saveAddr;
            pdata += 3;
            for (i=0; i < n;)
            {
                int val = *pdata++;
                int j;
                if (val & 0x80)
                {
                    val = ((val &0x80) << 8) + *pdata++;;
                }
                for (j = 0; j < val; j++)
                    t2[i + j].offset = 8 + (i) * 8 + (val - 1) *4;
                i += val;
            }
            t3->offset = (int)((char *)proc - (((char *)&t3->offset) + 4));
            t3->retsize =  isstdcall ? n1 * 4 : 0;
        }
        return record->thunk;
    }
    return NULL;
 }
__declspec(dllexport) void *__OCCMSIL_GetProcThunkToUnmanaged(void *proc)
 {
	static struct thunk {       /* must be <= 16 bytes, not sure if this is an MSIL limitation or not */
		unsigned char code[3];
		int offset;
		unsigned char retcode[4];
	} thunk = {
		0x52,                       /* push	edx */
		0x51,                       /* push ecx */
		0xE8,                       /* call rel32 */
		0,
		0x83, 0xC4, 0x08,           /* add esp,8 */
		0xC3                        /* ret */
	};
    struct hash * record = GetThunkRecord(proc, sizeof(thunk));
    if (record)
    {
        if (!record->funcptr)
        {
            record->funcptr = proc;
            memcpy(&record->thunk, &thunk, sizeof(thunk));
            *(int *)(record->thunk + 3) = (char *)proc - ((char *)record->thunk + 3 + 4);
        }
        return record->thunk;
    }
    return NULL;
 }
 /*
 __declspec(dllexport) void _stdcall testfunc(void *a)
 {
    void (*_stdcall b)(int a, int b, int c, int d, long long e, int f, int g,int h, int i) = a;
    (*b)(1,2,3,4,5,6,7,8,9);
    printf("try");
 }
 */