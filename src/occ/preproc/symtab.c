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
    if ( t->size == 1)
        return &t->table[0];
    for (i = 0;  *string; string++)
        i = ((i << 7) + (i << 1) +i )  ^ *string;
    return &t->table[i % t->size];
}
/* Add a hash item to the table */
HASHREC *AddName(SYMBOL *item, HASHTABLE *table)
{
    HASHREC **p = GetHashLink(table, item->name);
    HASHREC *newRec;

    if (*p)
    {
        HASHREC *q =  *p,  *r =  *p;
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
        p =  (HASHREC **)*p;
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

