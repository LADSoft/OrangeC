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
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "header.h"

extern HMENU hMenuMain;
extern HWND hwndClient;

DWINFO *mrulist[MAX_MRU],  *mruprojlist[MAX_MRU];

static HMENU hMRUSubMenu;


void MRUToMenu(int which)
{
    DWINFO **list;
    int base, base1, base2;
    MENUITEMINFO mi;
    char buf[256];
    int i, currentOffset;
    int maxed;
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maxed);
    if (which)
    {
        list = mruprojlist;
        base = ID_MRU_PROJ_LIST;
        base1 = 0; // menu index.  Must change if RC file changes
        base2 = 2;
    }
    else
    {
        list = mrulist;
        base = ID_MRU_LIST;
        base1 = 1; // see above 
        base2 = 2;
    }

    hMRUSubMenu = GetSubMenu(hMenuMain, maxed + 0);
    hMRUSubMenu = GetSubMenu(hMRUSubMenu, base1);
    hMRUSubMenu = GetSubMenu(hMRUSubMenu, base2);
    currentOffset = GetMenuItemCount(hMRUSubMenu);
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA;
    mi.fType = MFT_STRING;

    for (i = 0; i < MAX_MRU; i++)
    {
        mi.wID = i + base;
        mi.dwTypeData = buf;
        if (list[i])
        {
            sprintf(buf, "%d: %s", i + 1, list[i]->dwName);
            if (i >= currentOffset)
                InsertMenuItem(hMRUSubMenu, i, TRUE, &mi);
            else
                SetMenuItemInfo(hMRUSubMenu, i, TRUE, &mi);
        }
    }
}

//-------------------------------------------------------------------------

void MRUToProfile(int which)
{
    char key[20], buf[256],  *name;
    DWINFO **list;
    int i = 0;
    if (which)
    {
        list = mruprojlist;
        name = "MRUP";
    }
    else
    {
        list = mrulist;
        name = "MRU";
    }
    for (i = 0; i < MAX_MRU; i++)
    if (list[i])
    {
        sprintf(key, "%s%d", name, i);
        sprintf(buf, "%s;%s", list[i]->dwName, list[i]->dwTitle);
        StringToProfile(key, buf);
    }
}

//-------------------------------------------------------------------------

void ProfileToMRU(int which)
{
    int i;
    char key[50],  *buf,  *name;
    DWINFO **list;
    if (which)
    {
        list = mruprojlist;
        name = "MRUP";
    }
    else
    {
        list = mrulist;
        name = "MRU";
    }
    for (i = 0; i < MAX_MRU; i++)
        if (list[i])
        {
            free(list[i]);
            list[i] = 0;
        }
    for (i = 0; i < MAX_MRU; i++)
    {
        sprintf(key, "%s%d", name, i);
        buf = ProfileToString(key, "");
        if (buf[0])
        {
            DWINFO *x = calloc(1,sizeof(DWINFO));
            int j = 0;
            memset(x, 0, sizeof(*x));

            while (*buf &&  *buf != ';')
                x->dwName[j++] =  *buf++;
            j = 0;
            if (*buf)
                buf++;
            while (*buf)
                x->dwTitle[j++] =  *buf++;
            list[i] = x;
        }
    }

}

//-------------------------------------------------------------------------

void InsertMRU(DWINFO *data, int which)
{
    int i;
    DWINFO *x = calloc(1,sizeof(DWINFO)),  **list;
    if (!x)
        return ;
    if (which)
        list = mruprojlist;
    else
        list = mrulist;
    memcpy(x, data, sizeof(DWINFO));
    for (i = 0; i < MAX_MRU; i++)
    if (list[i] && !xstricmpz(x->dwName, list[i]->dwName))
    {
        if (i == 0)
            return ;
        free(list[i]);
        memmove(list + 1, list, i *sizeof(DWINFO*));
        list[0] = x;
        return ;
    }
    if (list[MAX_MRU - 1])
        free(list[MAX_MRU - 1]);
    memmove(list + 1, list, (MAX_MRU - 1) *sizeof(DWINFO*));
    list[0] = x;
}
void MoveMRUUp(int index, int which)
{
    DWINFO *xx, **list;
    if (index == 0)
        return ;    
    if (which)
        list = mruprojlist;
    else
        list = mrulist;
    
    xx = list[index];
    memmove( list+1, list, index *sizeof(DWINFO*));
    list[0] = xx;
    MRUToMenu(which);
    
}