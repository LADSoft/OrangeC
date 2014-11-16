#include "huff.h"
#include <limits.h>

static HUFF empty = { -1, INT_MAX };
BHEAP *BHeapCreate(int numItems)
{
    BHEAP *rv = calloc(1, sizeof(BHEAP));
    int n = 1;
    while (n < numItems)
        n <<= 1;
    n++;        
    rv->numItems = n;
    rv->items = malloc(n *sizeof(HUFF *));
    for (int i=0; i < n; i++)
    {
        rv->items[i] = &empty;        
    }
    return rv;
}
void BHeapPushUp(BHEAP *heap, int m)
{
    int n = (m - 1)/2;
    while (m && heap->items[n]->freq > heap->items[m]->freq)
    {
        HUFF *temp = heap->items[m];
        heap->items[m] = heap->items[n];
        heap->items[n] = temp;
        m = n;
        n = (m - 1)/2;
    }
}
BOOL BHeapInsert(BHEAP *heap, HUFF *item)
{
    int pos = -1;
    for (int i=0; i < heap->numItems; i++)
        if (heap->items[i] == &empty)
        {
            pos = i;
            break;
        }
    if (pos != -1)
    {
        heap->items[pos] = item;
        if (pos)
        {
            BHeapPushUp(heap, pos);
        }
    }
    return FALSE;
}
HUFF *BHeapRemove(BHEAP *heap)
{
    HUFF *rv = NULL;
    if (heap->items[0] != &empty)
    {
        rv = heap->items[0];
        int n = 0;
        while (n < heap->numItems/2)
        {
            if (heap->items[n*2+1]->freq < heap->items[n*2+2]->freq)
            {
                heap->items[n] = heap->items[n*2+1];
                n = n * 2 + 1;
            }
            else
            {
                heap->items[n] = heap->items[n*2+2];
                n = n * 2 + 2;
            }
        }
        heap->items[n] = &empty;
    }
    return rv;
}
HUFF *BHeapPeek(BHEAP *heap)
{
    if (heap->numItems && heap->items[0] != &empty)
        return heap->items[0];
    return NULL;
}
