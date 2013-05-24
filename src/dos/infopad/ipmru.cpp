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
#define Uses_TMenuItem
#define Uses_TMenu
#include "tv.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "infopad.h"
#include "ipconfig.h"

MRU::~MRU()
{
    for (int i=0; i < count; i++)
        delete items[i];
    count = 0;
}
void MRU::Add(char *name, char *title, bool atEnd)
{
    for (int i=0; i < count; i++)
    {
        if (!stricmp(name, items[i]->name))
        {
            MoveToTop(i);
            return;
        }
    }
    MRUItem *p = new MRUItem(name, title);
    if (p)
    {
        if (count == limit)
        {
            delete items[--count];
        }
        if (atEnd)
        {
            items[count++] = p;
        }
        else
        {
            memmove(items+1, items, sizeof(MRUItem *) * count);
            items[0] = p;
            count++;
        }
    }
}
void MRU::Remove(char *name)
{
    for (int i=0; i < count; i++)
    {
        if (!stricmp(name, items[i]->name))
        {
            delete items[i];
            memmove(items +i, items + i + 1, sizeof(MRUItem *) * (count - i -1));
            count--;
            return;
        }
    }
}
MRUItem *MRU::Get(int index)
{
    if (index < count)
        return items[index];
    return NULL;
}
void MRU::MoveToTop(int index)
{
    if (index < count)
    {
        MRUItem *p = items[index];
        memmove(items+1, items, sizeof(MRUItem *) * index);
        items[0] = p;
    }
}

void MRU::ToMenu(TMenuItem *menu, TMenuItem *Trailer) 
{
    if (Trailer)
    {
        limit = 100;
        insertAt = menu;
        trailer = Trailer;
        while (insertAt->next)
            insertAt = insertAt->next;
    }
    if (trailer)
    {
        TMenuItem *mitems = NULL , **pItem = &mitems;
        for (int i=0; i < count && i < 10; i++)
        {
            *pItem = new TMenuItem(items[i]->title, WINDOW_MRU_BASE + i, (ushort)0);
            pItem = &(*pItem)->next;
        }
        if (count > 10)
            *pItem = trailer;
        if (insertAt->next)
            delete insertAt->next;
        insertAt->next = mitems;
    }
    else
    {
        if (count)
        {
            TMenuItem *mitems = NULL , **pItem = &mitems;
            for (int i=0; i < count ; i++)
            {
                *pItem = new TMenuItem(items[i]->title, FILE_MRU_BASE + i, (ushort)0);
                pItem = &(*pItem)->next;
            }
            delete menu->subMenu->items;
            menu->subMenu->items = menu->subMenu->deflt = mitems;
        }
    }
}
