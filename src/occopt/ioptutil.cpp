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

static std::list<BriggsSet*> tab, tabc, tabt, tabs, taba, tabz;
std::unordered_set<BITINT*> btab, cbtab, tbtab, sbtab, abtab, zbtab;

inline static void freex(std::list<BriggsSet*>& set, std::unordered_set<BITINT*>* bits)
{
    for (auto s : set)
    {
        delete[] s->indexes;
        delete[] s->data;
        delete s;
    }
    set.clear();
    if (bits)
    {
        for (auto b : *bits)
        {
#ifdef TESTBITS
            delete ((BITINT*)b - 1);
#else
            delete b;
#endif
        }
        bits->clear();
    }
}
inline static BriggsSet* allocx(unsigned size)
{
    BriggsSet* p = new BriggsSet;
    p->indexes = size ? new unsigned[size] :nullptr;
    p->data = size ? new unsigned[size] : nullptr;
    p->size = size;
    p->top = 0;
    std::fill(p->indexes, p->indexes + size, -1);
    return p;
}
void BitInit(void)
{
    int i;
    for (i = 0; i < BITINTBITS; i++)
        bittab[i] = 1 << i;
    freex(tab, &btab);
    freex(tabc, &cbtab);
    freex(tabt, &tbtab);
    freex(tabs, &sbtab);   
    freex(taba, &abtab);
    freex(tabz, &zbtab);
}
void briggsClear(BriggsSet* data)
{
    std::fill(data->indexes, data->indexes + data->size, -1);
    data->top = 0;
}
BriggsSet* briggsAlloc(unsigned size)
{
    BriggsSet* p = allocx(size);
    tab.push_back(p);
    return p;
}
BriggsSet* briggsAlloct(unsigned size)
{
    BriggsSet* p = allocx(size);
    tabt.push_back(p);
    return p;
}
BriggsSet* briggsAllocc(unsigned size)
{
    BriggsSet* p = allocx(size);
    tabc.push_back(p);
    return p;
}
BriggsSet* briggsAllocs(unsigned size)
{
    BriggsSet* p = allocx(size);
    tabs.push_back(p);
    return p;
}
void briggsFree()
{
	freex(tab, &btab);
}
void briggsFreet()
{
	freex(tabt, &tbtab);
}
void briggsFreec()
{
	freex(tabc, &cbtab);
}
void briggsFrees()
{
	freex(tabs, &sbtab);
}
void briggsFreea() 
{ 
    freex(taba, &abtab); 
}
void briggsFreez() 
{ 
    freex(tabz, &zbtab); 
}
int briggsSet(BriggsSet* p, unsigned index)
{
    if (index < p->size)
    {
        if (p->indexes[index] == -1)
        {
            p->indexes[index] = p->top;
            p->data[p->top++] = index;
        }
    }
    return 0;
}
int briggsReset(BriggsSet* p, unsigned index)
{
    if (index < p->size)
    {
        int slot = p->indexes[index];
        if (slot != -1)
        {
            if (slot != p->top -1)
            {
                int q = p->data[p->top - 1];
                p->indexes[q] = slot;
                p->data[slot] = q;
            }
            p->indexes[index] = -1;
            p->top--;
        }
    }
    return 0;
}
int briggsTest(BriggsSet* p, unsigned index)
{

    return index < p->size && p->indexes[index] != -1;
}
int briggsUnion(BriggsSet* s1, BriggsSet* s2)
{
    int i;
    for (i = 0; i < s2->top; i++)
    {
        briggsSet(s1, s2->data[i]);
    }
    return 0;
}
BITINT* AllocBit(void* Allocator(int), std::unordered_set<BITINT*>* tab, unsigned size)
{
    if (size == 0)
        size++;
#ifdef TESTBITS
    int sz = ((size + BITINTBITS) + (BITINTBITS - 1)) / BITINTBITS;
    auto rv = new BITINT[sz];
    std::fill(rv, rv + sz, 0);
    *rv++ = (sz-1) * BITINTBITS;
#else
    int sz = (size + (BITINTBITS - 1)) / BITINTBITS;
    auto rv = new BITINT[sz];
    std::fill(rv, rv + sz, 0);
#endif
    if (tab)
        tab->insert(rv);
    return rv;
}
int isset(BITINT* array, unsigned bit)
{
#ifdef TESTBITS
    if (bit >= array[-1])
        printf("diag::isset:%d,%d\n", bit, array[-1]);
#endif
    return array[bit / BITINTBITS] & bittab[bit & (BITINTBITS - 1)];
}
 void setbit(BITINT* array, unsigned bit)
{
#ifdef TESTBITS
    if (bit >= array[-1])
        printf("diag::setbit:%d,%d\n", bit, array[-1]);
#endif
    array[bit / BITINTBITS] |= bittab[bit & (BITINTBITS - 1)];
}
 void clearbit(BITINT* array, unsigned bit)
{
#ifdef TESTBITS
    if (bit >= array[-1])
        printf("diag::clearbit:%d,%d\n", bit, array[-1]);
#endif
    array[bit / BITINTBITS] &= ~bittab[bit & (BITINTBITS - 1)];
}
 void bitarrayClear(BITINT* array, unsigned size)
{
#ifdef TESTBITS
    if (size > array[-1])
        printf("diag::arrayclear:%d,%d\n", size, array[-1]);
#endif
    memset(array, 0, (size + (BITINTBITS - 1)) / BITINTBITS * sizeof(BITINT));
}
 void zfreebit(BITINT* array)
{
    if (array == nullptr)
        return;
    zbtab.erase(array);
#ifdef TESTBITS
    array--;
#endif
    delete[] array;
}

}  // namespace Optimizer