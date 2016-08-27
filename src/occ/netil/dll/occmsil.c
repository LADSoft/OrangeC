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
	unsigned char thunk[16];
} *table[MAX_HASH];

#pragma pack(1)

static struct hash *dataPointer;
static int left;
static void *NextRecord()
{
    if (!left)
    {
        dataPointer = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (dataPointer)
        {
            left = 4096/sizeof(struct hash);
        }
    }
    if (left)
    {
        left--;
        return dataPointer++;
    }
    return NULL;
}
static struct hash *GetThunkRecord(void *proc)
{
    int hashval = (((int)proc)>>2) % MAX_HASH;
    struct hash *link = table[hashval];
    while (link)
    {
        if (link->funcptr == proc)
            return link;
        link = link->next;
    }
    link = NextRecord();
    if (link)
    {
        link->next = table[hashval];
        table[hashval] = link;
        return link;
    }
    return NULL;
}
 void * _export _OCCMSIL_GetProcThunkToManaged(void *proc)
 {
	static struct thunk {       /* must be <= 16 bytes, not sure if this is an MSIL limitation or not */
		unsigned char code[9];
		int offset;
	} thunk = {
		0x8B, 0x4C, 0x24, 0x04,     /* mov ecx,DWORD PTR [esp+4] */
		0x8B, 0x54, 0x24, 0x08,     /* mov edx,DWORD PTR [esp+8] */
		0xE9,                       /* jmp rel32 */
		0
	};
    struct hash * record = GetThunkRecord(proc);
    if (record)
    {
        if (!record->funcptr)
        {
            record->funcptr = proc;
            memcpy(&record->thunk, &thunk, sizeof(thunk));
            *(int *)(record->thunk + 9) = (char *)proc - ((char *)record->thunk + 9 + 4);
        }
        return record->thunk;
    }
    return NULL;
 }
 void * _export _OCCMSIL_GetProcThunkToUnmanaged(void *proc)
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
    struct hash * record = GetThunkRecord(proc);
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