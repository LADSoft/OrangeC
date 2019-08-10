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

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "header.h"

extern HMENU hMenuMain;
extern HWND hwndClient;

DWINFO *mrulist[MAX_MRU], *mruprojlist[MAX_MRU];

static HMENU hMRUSubMenu;

void MRUToMenu(int which)
{
    DWINFO** list;
    int base, base1;
    MENUITEMINFO mi;
    char buf[256];
    int i, currentOffset;
    int maxed;
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maxed);
    if (which)
    {
        list = mruprojlist;
        base = ID_MRU_PROJ_LIST;
        base1 = 2;  // menu index.  Must change if RC file changes
    }
    else
    {
        list = mrulist;
        base = ID_MRU_LIST;
        base1 = 9;  // see above
    }

    hMRUSubMenu = GetSubMenu(hMenuMain, maxed);
    hMRUSubMenu = GetSubMenu(hMRUSubMenu, base1);
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
    char key[20], buf[256], *name;
    DWINFO** list;
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
    char key[50], *buf, *name;
    DWINFO** list;
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
            DWINFO* x = calloc(1, sizeof(DWINFO));
            int j = 0;
            memset(x, 0, sizeof(*x));

            while (*buf && *buf != ';')
                x->dwName[j++] = *buf++;
            j = 0;
            if (*buf)
                buf++;
            while (*buf)
                x->dwTitle[j++] = *buf++;
            list[i] = x;
        }
    }
}

//-------------------------------------------------------------------------

void InsertMRU(DWINFO* data, int which)
{
    int i;
    DWINFO *x = calloc(1, sizeof(DWINFO)), **list;
    if (!x)
        return;
    if (which)
        list = mruprojlist;
    else
        list = mrulist;
    memcpy(x, data, sizeof(DWINFO));
    for (i = 0; i < MAX_MRU; i++)
        if (list[i] && !xstricmpz(x->dwName, list[i]->dwName))
        {
            if (i == 0)
                return;
            free(list[i]);
            memmove(list + 1, list, i * sizeof(DWINFO*));
            list[0] = x;
            return;
        }
    if (list[MAX_MRU - 1])
        free(list[MAX_MRU - 1]);
    memmove(list + 1, list, (MAX_MRU - 1) * sizeof(DWINFO*));
    list[0] = x;
}
void MoveMRUUp(int index, int which)
{
    DWINFO *xx, **list;
    if (index == 0)
        return;
    if (which)
        list = mruprojlist;
    else
        list = mrulist;

    xx = list[index];
    memmove(list + 1, list, index * sizeof(DWINFO*));
    list[0] = xx;
    MRUToMenu(which);
}