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
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "header.h"
#include "idewinconst.h"

#define DELTA 7
#define MAX_BROWSE 50

extern HWND hwndFrame, hwndClient;

FILEBROWSE* fileBrowseCursor = NULL;
FILEBROWSE* fileBrowseInfo;
int fileBrowseCount;
static HWND active;
static FILEBROWSE* ordered[MAX_BROWSE];

void FileBrowseLeft()
{
    if (!fileBrowseCursor)
        return;
    else
    {
        fileBrowseCursor = fileBrowseCursor->prev;
        if (!fileBrowseCursor)
        {
            HWND current = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
            if (active == current)
            {
                SetFocus(current);
            }
            else
            {
                PostMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)active, 0);
            }
        }
    }
    if (fileBrowseCursor)
    {
        DWINFO info;
        strcpy(info.dwName, fileBrowseCursor->info->dwName);
        info.dwLineNo = fileBrowseCursor->lineno;
        info.logMRU = FALSE;
        info.newFile = FALSE;
        CreateDrawWindow(&info, TRUE);
    }
    SendMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
}
void FileBrowseRight()
{
    if (!fileBrowseCursor)
    {
        active = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
        fileBrowseCursor = fileBrowseInfo;
    }
    else if (fileBrowseCursor->next)
        fileBrowseCursor = fileBrowseCursor->next;
    else
        return;
    if (fileBrowseCursor)
    {
        DWINFO info;
        strcpy(info.dwName, fileBrowseCursor->info->dwName);
        info.dwLineNo = fileBrowseCursor->lineno;
        info.logMRU = FALSE;
        info.newFile = FALSE;
        CreateDrawWindow(&info, TRUE);
    }
    SendMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
}
void FileBrowseLineChange(DWINFO* info, int lineno, int delta)
{
    FILEBROWSE* newBrowse;
    FILEBROWSE** p = &fileBrowseInfo;
    fileBrowseCursor = NULL;
    SendMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    if (delta == 0 && fileBrowseInfo && fileBrowseInfo->info == info && fileBrowseInfo->lineno == lineno)
        return;
    while (*p)
    {
        FILEBROWSE** next = &(*p)->next;
        if ((*p)->info == info)
        {
            if ((*p)->lineno > lineno)
            {
                if (delta > 0)
                {
                    (*p)->lineno += delta;
                }
                else
                {
                    if ((*p)->lineno + delta < lineno)
                    {
                        FILEBROWSE* q = *p;
                        if (q->next)
                            q->next->prev = q->prev;
                        *p = (*p)->next;

                        fileBrowseCount--;
                        free(q);
                        next = p;
                    }
                    else
                    {
                        (*p)->lineno += delta;
                    }
                }
            }
        }
        p = next;
    }
    p = &fileBrowseInfo;
    while (*p)
    {
        FILEBROWSE** next = &(*p)->next;
        if ((*p)->info == info)
        {
            if (((*p)->lineno < lineno && (*p)->lineno + DELTA >= lineno) ||
                ((*p)->lineno >= lineno && (*p)->lineno - DELTA <= lineno))
            {
                FILEBROWSE* q = *p;
                if (q->next)
                    q->next->prev = q->prev;
                *p = (*p)->next;
                fileBrowseCount--;
                free(q);
                next = p;
            }
        }
        p = next;
    }
    if (fileBrowseCount >= MAX_BROWSE)
    {
        p = &fileBrowseInfo;
        while (*p)
        {
            if (!(*p)->next)
            {
                FILEBROWSE* q = *p;
                if (q->next)
                    q->next->prev = q->prev;
                *p = (*p)->next;
                fileBrowseCount--;
                free(q);
                break;
            }
            p = &(*p)->next;
        }
    }
    newBrowse = (FILEBROWSE*)malloc(sizeof(FILEBROWSE));
    if (newBrowse)
    {
        newBrowse->info = info;
        newBrowse->lineno = lineno;
        newBrowse->prev = NULL;
        newBrowse->next = fileBrowseInfo;
        if (newBrowse->next)
            newBrowse->next->prev = newBrowse;
        fileBrowseInfo = newBrowse;
        fileBrowseCount++;
    }
}
void FileBrowseClose(DWINFO* info)
{
    FILEBROWSE** p = &fileBrowseInfo;
    fileBrowseCursor = NULL;
    while (*p)
    {
        if ((*p)->info == info)
        {
            FILEBROWSE* q = *p;
            if (q->next)
                q->next->prev = q->prev;
            *p = (*p)->next;
            fileBrowseCount--;
            free(q);
        }
        else
        {
            p = &(*p)->next;
        }
    }
}
void FileBrowseCloseAll()
{
    FILEBROWSE* p = fileBrowseInfo;
    fileBrowseCursor = NULL;
    while (p)
    {
        FILEBROWSE* next = p->next;
        free(p);
        p = next;
    }
    fileBrowseInfo = NULL;
    fileBrowseCount = 0;
}
static int fbcomp(const void* a, const void* b)
{
    const FILEBROWSE* ab = *(FILEBROWSE**)a;
    const FILEBROWSE* bb = *(FILEBROWSE**)b;
    int n = strcmp(ab->info->dwName, bb->info->dwName);
    if (n != 0)
        return n;
    if (ab->lineno < bb->lineno)
        return -1;
    if (ab->lineno > bb->lineno)
        return 1;
    return 0;
}
static char* GetName(int k)
{
    static char names[MAX_BROWSE][20];
    char buf[256], *p;
    memset(buf, 0, 256);
    *(short*)buf = 256;

    SendMessage(ordered[k]->info->dwHandle, EM_GETLINE, (WPARAM)ordered[k]->lineno - 1, (LPARAM)buf);
    p = buf;
    while (isspace(*p))
        p++;
    if (*p)
    {
        int i;
        for (i = 0; i < sizeof(names[0]) - 1 && *p; i++)
            names[k][i] = *p++;
        names[k][i] = 0;
    }
    else
        strcpy(names[k], "--");
    return names[k];
}
void FileBrowseMenu(int x, int y, HWND hwndToolbar)
{
    if (fileBrowseInfo)
    {
        FILEBROWSE* p = fileBrowseInfo;
        HMENU popup = CreatePopupMenu();
        int i = 0, j;
        memset(ordered, 0, sizeof(ordered));
        while (p && i < MAX_BROWSE)
        {
            ordered[i++] = p;
            p = p->next;
        }
        qsort(ordered, i, sizeof(FILEBROWSE*), fbcomp);

        for (j = 0; j < i;)
        {
            FILEBROWSE* cur = ordered[j];
            HMENU internalPop = CreatePopupMenu();
            for (; j < i && cur->info == ordered[j]->info; j++)
            {
                InsertMenu(internalPop, -1, MF_BYPOSITION | MF_STRING, j + ID_FILEBROWSE_LIST, GetName(j));
            }
            InsertMenu(popup, -1, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)internalPop, cur->info->dwTitle);
        }
        InsertBitmapsInMenu(popup);
        TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTBUTTON, x, y, hwndFrame, NULL);
        DestroyMenu(popup);
    }
}
void FileBrowseTrigger(int i)
{
    if (ordered[i])
    {
        DWINFO info;
        strcpy(info.dwName, ordered[i]->info->dwName);
        info.dwLineNo = ordered[i]->lineno;
        info.logMRU = FALSE;
        info.newFile = FALSE;
        CreateDrawWindow(&info, TRUE);
    }
}