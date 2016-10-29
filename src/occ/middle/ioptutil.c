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
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "compiler.h"

BITINT bittab[BITINTBITS];

void BitInit(void)
{
    int i;
    for (i=0; i <BITINTBITS; i++)
        bittab[i] = 1 << i;
}
BRIGGS_SET *briggsAlloc(int size)
{
    BRIGGS_SET *p = oAlloc(sizeof(BRIGGS_SET));
    p-> indexes = oAlloc(sizeof(*p->indexes) * size);
    p->data = oAlloc(sizeof(*p->data) * size);
    p->size = size ;
    return p;
}
BRIGGS_SET *briggsAlloct(int size)
{
    BRIGGS_SET *p = tAlloc(sizeof(BRIGGS_SET));
    p-> indexes = tAlloc(sizeof(*p->indexes) * size);
    p->data = tAlloc(sizeof(*p->data) * size);
    p->size = size ;
    return p;
}
BRIGGS_SET *briggsAllocc(int size)
{
    BRIGGS_SET *p = cAlloc(sizeof(BRIGGS_SET));
    p-> indexes = cAlloc(sizeof(*p->indexes) * size);
    p->data = cAlloc(sizeof(*p->data) * size);
    p->size = size ;
    return p;
}
BRIGGS_SET *briggsAllocs(int size)
{
    BRIGGS_SET *p = sAlloc(sizeof(BRIGGS_SET));
    p-> indexes = sAlloc(sizeof(*p->indexes) * size);
    p->data = sAlloc(sizeof(*p->data) * size);
    p->size = size ;
    return p;
}
BRIGGS_SET *briggsReAlloc(BRIGGS_SET *set, int size)
{
    if (!set || set->size < size)
    {
        BRIGGS_SET *set2 = briggsAlloc(size);
#ifdef XXXXX
        if (set)
        {
            memcpy(set2->data, set->data, set->top * sizeof(*set->data));
            memcpy(set2->indexes, set->indexes, set->size * sizeof(*set->indexes));
            set2->top = set->top;
        }
#endif
        set = set2;		
    }
    else
        briggsClear(set);
    return set;
}
int briggsSet(BRIGGS_SET *p, int index)
{
    if (index >= p->size || index < 0)
        diag("briggsSet: out of bounds");
    if (p->indexes[index] >= p->top ||
        index != p->data[p->indexes[index]])
    {
        p->indexes[index] = p->top ;
        p->data[p->top++] = index;	
    }
    return 0;
}
int briggsReset(BRIGGS_SET *p, int index)
{
    int i ;
    if (index >= p->size || index < 0)
        diag("briggsReset: out of bounds");
    i = p->indexes[index];
    if (i >= 0 && i < p->top)
    {
        int j = p->data[i] ;
        if (j == index)
        {
            p->top--;
            p->indexes[index] = -1;
            if (i != p->top)
            {
                j = p->data[i] = p->data[p->top] ;
                p->indexes[j] = i ;
            }
        }
    }
    return 0;
}
int briggsTest(BRIGGS_SET *p, int index)
{
    int i = p->indexes[index];
    if (index >= p->size || index < 0)
        diag("briggsTest: out of bounds");
    if (i >= p->top)
        return 0;
    return p->data[i] == index;	
}
int briggsUnion(BRIGGS_SET *s1, BRIGGS_SET *s2)
{
    int i;
    for (i=0; i < s2->top; i++)
    {
        briggsSet(s1, s2->data[i]);
    }
    return 0;
}
int briggsIntersect(BRIGGS_SET *s1, BRIGGS_SET *s2)
{
    int i;
    for (i=s1->top-1; i >= 0; i--)
    {
        if (!briggsTest(s2, s1->data[i]))
        {
            briggsReset(s1, s1->data[i]);
        }
    }
    return 0;
}
#ifdef TESTBITS
BITARRAY *allocbit(int size)
{
    BITARRAY *rv = oAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS-1))/BITINTBITS);
    rv->count = size;
    return rv;
}
BITARRAY *tallocbit(int size)
{
    BITARRAY *rv = tAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS-1))/BITINTBITS * sizeof(BITINT));
    rv->count = size;
    return rv;
}
BITARRAY *sallocbit(int size)
{
    BITARRAY *rv = sAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS-1))/BITINTBITS * sizeof(BITINT));
    rv->count = size;
    return rv;
}
BITARRAY *aallocbit(int size)
{
    BITARRAY *rv = aAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS-1))/BITINTBITS * sizeof(BITINT));
    rv->count = size;
    return rv;
}
BITARRAY *callocbit(int size)
{
    BITARRAY *rv = cAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS-1))/BITINTBITS * sizeof(BITINT));
    rv->count = size;
    return rv;
}
BOOLEAN isset(BITARRAY *arr, int bit)
{
    if (bit >= arr->count)
        diag("isset: overflow");
    return arr->data[bit/BITINTBITS] & bittab[bit & (BITINTBITS-1)];
}
void setbit(BITARRAY *arr, int bit)
{
    if (bit >= arr->count)
        diag("setbit: overflow");
    arr->data[bit/BITINTBITS] |= bittab[bit & (BITINTBITS-1)];
}
void clearbit(BITARRAY *arr, int bit)
{
    if (bit >= arr->count)
        diag("clearbit: overflow");
    arr->data[bit/BITINTBITS] &= ~bittab[bit & (BITINTBITS-1)];
}
void bitarrayClear(BITARRAY *arr, int count)
{
    if (count > arr->count)
        diag("bitarrayclear: overflow");
    memset(arr->data, 0, (arr->count + (BITINTBITS-1)) / BITINTBITS * sizeof(BITINT));
}
#endif