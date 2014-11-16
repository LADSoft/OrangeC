#include <windows.h>
#include <stdio.h>

typedef struct _huff
{
    int ch;
    int freq;
    struct _huff * parent;
    struct _huff * children[2];
} HUFF;
typedef struct _bheap
{
    int numItems;
    HUFF **items;
} BHEAP;


BHEAP *BHeapCreate(int numItems);
BOOL BHeapInsert(BHEAP *heap, HUFF *item);
HUFF *BHeapRemove(BHEAP *heap);
HUFF *BHeapPeek(BHEAP *heap);
