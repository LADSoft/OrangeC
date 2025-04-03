/* Software License Agreement
 * 
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 * 
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
