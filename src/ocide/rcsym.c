/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
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
/* Handles symbol tables 
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <windows.h>
#include "rc.h"
#define ROTR(x,bits) (((x << (16 - bits)) | (x >> bits)) & 0xffff)
#define ROTL(x,bits) (((x << bits) | (x >> (16 - bits))) & 0xffff)
#define HASHTABLESIZE 1023

HASHREC **defhash = 0;

TABLE defsyms;

void symini(void)
{
    defsyms.head = defsyms.tail = 0;
    if (!defhash)
    {
        defhash = (HASHREC **)malloc(HASHTABLESIZE *sizeof(HASHREC*));
    }
    memset(defhash, 0, HASHTABLESIZE *sizeof(HASHREC*));
}

/* Sym tab hash function */
static unsigned int ComputeHash(char *string, int size)
{
    unsigned int len = strlen(string), rv;
    char *pe = len + string;
    unsigned char blank = ' ';

    rv = len | blank;
    while (len--)
    {
        unsigned char cback = (unsigned char)(*--pe) | blank;
        rv = ROTR(rv, 2) ^ cback;
    }
    return (rv % size);
}

/* Add a hash item to the table */
HASHREC *AddHash(HASHREC *item, HASHREC **table, int size)
{
    int index = ComputeHash(item->key, size);
    HASHREC **p;

    item->link = 0;

    if (*(p = &table[index]))
    {
        HASHREC *q =  *p,  *r =  *p;
        while (q)
        {
            r = q;
            if (!strcmp(r->key, item->key))
                return (r);
            q = q->link;
        }
        r->link = item;
    }
    else
        *p = item;
    return (0);
}

/*
 * Find something in the hash table
 */
HASHREC **LookupHash(char *key, HASHREC **table, int size)
{
    int index = ComputeHash(key, size);
    HASHREC **p;

    if (*(p = &table[index]))
    {
        HASHREC *q =  *p;
        while (q)
        {
            if (!strcmp(q->key, key))
                return (p);
            p =  &(*p)->link;
            q = q->link;
        }
    }
    return (0);
}

/*
 * Some tables use hash tables and some use linked lists
 * This is the global symbol search routine
 */
SYM *basesearch(char *na, TABLE *table, int checkaccess)
{
    SYM *thead = table->head;
    SYM **p;
    p = ((SYM **)LookupHash(na, defhash, HASHTABLESIZE));
    if (p)
    {
        p =  (*p)->next;
    }
    return (SYM*)p;
}

//-------------------------------------------------------------------------

SYM *search(char *na, TABLE *table)
{
    return basesearch(na, table, 1);
}

/* The global symbol insert routine */
void insert(SYM *sp, TABLE *table)

{
    AddHash(sp, defhash, HASHTABLESIZE);
}
