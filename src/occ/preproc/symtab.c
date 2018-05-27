/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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

void syminit(void)
{
}
HASHTABLE *CreateHashTable(int size)
{
    HASHTABLE *rv = Alloc(sizeof(HASHTABLE));
    rv->table = Alloc(sizeof(HASHREC *) * size);
    rv->size = size;
    return rv;
}
/* SYMBOL tab hash function */
HASHREC **GetHashLink(HASHTABLE *t, char *string)
{
    unsigned i;
    if (t->size == 1)
        return &t->table[0];
    for (i = 0; *string; string++)
        i = ((i << 7) + (i << 1) + i) ^ *string;
    return &t->table[i % t->size];
}
/* Add a hash item to the table */
HASHREC *AddName(SYMBOL *item, HASHTABLE *table)
{
    HASHREC **p = GetHashLink(table, item->name);
    HASHREC *newRec;

    if (*p)
    {
        HASHREC *q = *p, *r = *p;
        while (q)
        {
            r = q;
            if (!strcmp(r->p->name, item->name))
                return (r);
            q = q->next;
        }
        newRec = Alloc(sizeof(HASHREC));
        r->next = newRec;
        newRec->p = (struct _hrintern_ *)item;
    }
    else
    {
        newRec = Alloc(sizeof(HASHREC));
        *p = newRec;
        newRec->p = (struct _hrintern_ *)item;
    }
    return (0);
}

/*
 * Find something in the hash table
 */
HASHREC **LookupName(char *name, HASHTABLE *table)
{
    HASHREC **p = GetHashLink(table, name);

    while (*p)
    {
        if (!strcmp((*p)->p->name, name))
            return p;
        p = (HASHREC **)*p;
    }
    return (0);
}
SYMBOL *search(char *name, HASHTABLE *table)
{
    while (table)
    {
        HASHREC **p = LookupName(name, table);
        if (p)
            return (SYMBOL *)(*p)->p;
        table = table->next;
    }
    return 0;
}
void insert(SYMBOL *in, HASHTABLE *table)
{
    if (AddName(in, table))
    {
        pperrorstr(ERR_DUPLICATE_IDENTIFIER, in->name);
    }
}
