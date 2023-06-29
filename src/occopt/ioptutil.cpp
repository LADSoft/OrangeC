/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "ioptutil.h"
#include "memory.h"
#include "ilocal.h"

namespace Optimizer
{

BITINT bittab[BITINTBITS];

void BitInit(void)
{
    int i;
    for (i = 0; i < BITINTBITS; i++)
        bittab[i] = 1 << i;
}
BRIGGS_SET* briggsAlloc(int size)
{
    BRIGGS_SET* p = oAllocate<BRIGGS_SET>();
    p->indexes = oAllocate<unsigned short>(size);
    p->data = oAllocate<unsigned short>(size);
    p->size = size;
    return p;
}
BRIGGS_SET* briggsAlloct(int size)
{
    BRIGGS_SET* p = tAllocate<BRIGGS_SET>();
    p->indexes = tAllocate<unsigned short>(size);
    p->data = tAllocate<unsigned short>(size);
    p->size = size;
    return p;
}
BRIGGS_SET* briggsAllocc(int size)
{
    BRIGGS_SET* p = cAllocate<BRIGGS_SET>();
    p->indexes = cAllocate<unsigned short>(size);
    p->data = cAllocate<unsigned short>(size);
    p->size = size;
    return p;
}
BRIGGS_SET* briggsAllocs(int size)
{
    BRIGGS_SET* p = sAllocate<BRIGGS_SET>();
    p->indexes = sAllocate<unsigned short>(size);
    p->data = sAllocate<unsigned short>(size);
    p->size = size;
    return p;
}
BRIGGS_SET* briggsReAlloc(BRIGGS_SET* set, int size)
{
    if (!set || set->size < size)
    {
        BRIGGS_SET* set2 = briggsAlloc(size);
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
int briggsSet(BRIGGS_SET* p, int index)
{
    if (index >= p->size || index < 0)
        diag("briggsSet: out of bounds");
    if (p->indexes[index] >= p->top || index != p->data[p->indexes[index]])
    {
        p->indexes[index] = p->top;
        p->data[p->top++] = index;
    }
    return 0;
}
int briggsReset(BRIGGS_SET* p, int index)
{
    int i;
    if (index >= p->size || index < 0)
        diag("briggsReset: out of bounds");
    i = p->indexes[index];
    if (i >= 0 && i < p->top)
    {
        int j = p->data[i];
        if (j == index)
        {
            p->top--;
            p->indexes[index] = -1;
            if (i != p->top)
            {
                j = p->data[i] = p->data[p->top];
                p->indexes[j] = i;
            }
        }
    }
    return 0;
}
int briggsTest(BRIGGS_SET* p, int index)
{
    int i = p->indexes[index];
    if (index >= p->size || index < 0)
        diag("briggsTest: out of bounds");
    if (i >= p->top)
        return 0;
    return p->data[i] == index;
}
int briggsUnion(BRIGGS_SET* s1, BRIGGS_SET* s2)
{
    int i;
    for (i = 0; i < s2->top; i++)
    {
        briggsSet(s1, s2->data[i]);
    }
    return 0;
}
int briggsIntersect(BRIGGS_SET* s1, BRIGGS_SET* s2)
{
    int i;
    for (i = s1->top - 1; i >= 0; i--)
    {
        if (!briggsTest(s2, s1->data[i]))
        {
            briggsReset(s1, s1->data[i]);
        }
    }
    return 0;
}
#ifdef TESTBITS
BITARRAY* allocbit(int size)
{
    BITARRAY* rv = oAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS - 1)) / BITINTBITS);
    rv->count = size;
    return rv;
}
BITARRAY* tallocbit(int size)
{
    BITARRAY* rv = tAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS - 1)) / BITINTBITS * sizeof(BITINT));
    rv->count = size;
    return rv;
}
BITARRAY* sallocbit(int size)
{
    BITARRAY* rv = sAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS - 1)) / BITINTBITS * sizeof(BITINT));
    rv->count = size;
    return rv;
}
BITARRAY* aallocbit(int size)
{
    BITARRAY* rv = aAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS - 1)) / BITINTBITS * sizeof(BITINT));
    rv->count = size;
    return rv;
}
BITARRAY* callocbit(int size)
{
    BITARRAY* rv = cAlloc(sizeof(BITARRAY) - 1 + (size + (BITINTBITS - 1)) / BITINTBITS * sizeof(BITINT));
    rv->count = size;
    return rv;
}
bool isset(BITARRAY* arr, int bit)
{
    if (bit >= arr->count)
        diag("isset: overflow");
    return arr->data[bit / BITINTBITS] & bittab[bit & (BITINTBITS - 1)];
}
void setbit(BITARRAY* arr, int bit)
{
    if (bit >= arr->count)
        diag("setbit: overflow");
    arr->data[bit / BITINTBITS] |= bittab[bit & (BITINTBITS - 1)];
}
void clearbit(BITARRAY* arr, int bit)
{
    if (bit >= arr->count)
        diag("clearbit: overflow");
    arr->data[bit / BITINTBITS] &= ~bittab[bit & (BITINTBITS - 1)];
}
void bitarrayClear(BITARRAY* arr, int count)
{
    if (count > arr->count)
        diag("bitarrayclear: overflow");
    memset(arr->data, 0, (arr->count + (BITINTBITS - 1)) / BITINTBITS * sizeof(BITINT));
}
#endif
}  // namespace Optimizer