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

#include "compiler.h"

extern COMPILER_PARAMS cparams;

void syminit(void) {}
HASHTABLE* CreateHashTable(int size)
{
    HASHTABLE* rv = (HASHTABLE*)Alloc(sizeof(HASHTABLE));
    rv->table = (SYMLIST**)Alloc(sizeof(SYMLIST*) * size);
    rv->size = size;
    return rv;
}
/* SYMBOL tab hash function */
SYMLIST** GetHashLink(HASHTABLE* t, const char* string)
{
    unsigned i;
    if (t->size == 1)
        return &t->table[0];
    for (i = 0; *string; string++)
        i = ((i << 7) + (i << 1) + i) ^ *string;
    return &t->table[i % t->size];
}
/* Add a hash item to the table */
SYMLIST* AddName(SYMBOL* item, HASHTABLE* table)
{
    SYMLIST** p = GetHashLink(table, item->name);
    SYMLIST* newRec;

    if (*p)
    {
        SYMLIST *q = *p, *r = *p;
        while (q)
        {
            r = q;
            if (!strcmp(r->p->name, item->name))
                return (r);
            q = q->next;
        }
        newRec = (SYMLIST*)Alloc(sizeof(SYMLIST));
        r->next = newRec;
        newRec->p = (sym*)item;
    }
    else
    {
        newRec = (SYMLIST*)Alloc(sizeof(SYMLIST));
        *p = newRec;
        newRec->p = (sym*)item;
    }
    return (0);
}

/*
 * Find something in the hash table
 */
SYMLIST** LookupName(const char* name, HASHTABLE* table)
{
    SYMLIST** p = GetHashLink(table, name);

    while (*p)
    {
        if (!strcmp((*p)->p->name, name))
            return p;
        p = (SYMLIST**)*p;
    }
    return (0);
}
SYMBOL* search(const char* name, HASHTABLE* table)
{
    while (table)
    {
        SYMLIST** p = LookupName(name, table);
        if (p)
            return (SYMBOL*)(*p)->p;
        table = table->next;
    }
    return 0;
}
void insert(SYMBOL* in, HASHTABLE* table)
{
    if (AddName(in, table))
    {
        pperrorstr(ERR_DUPLICATE_IDENTIFIER, in->name);
    }
}
