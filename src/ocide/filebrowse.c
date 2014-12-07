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
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "header.h"
#include "winconst.h"

#define DELTA 7
#define MAX_BROWSE 50

extern HWND hwndFrame, hwndClient, hwndASM;

FILEBROWSE *fileBrowseCursor = NULL;
FILEBROWSE *fileBrowseInfo;
int fileBrowseCount;
static HWND active;
static FILEBROWSE *ordered[MAX_BROWSE];

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
            if (GetParent(hwndASM) == current || active == current)
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
void FileBrowseLineChange(DWINFO *info, int lineno, int delta)
{
    FILEBROWSE *newBrowse;
    FILEBROWSE **p = &fileBrowseInfo;
    fileBrowseCursor = NULL;
    SendMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
    if (delta == 0 && fileBrowseInfo && fileBrowseInfo->info == info && fileBrowseInfo->lineno == lineno)
        return;
    while (*p)
    {
        FILEBROWSE **next = &(*p)->next;
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
                    if ((*p)->lineno +delta < lineno)
                    {
                        FILEBROWSE *q = *p;
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
        FILEBROWSE **next = &(*p)->next;
        if ((*p)->info == info)
        {
            if (((*p)->lineno < lineno && (*p)->lineno + DELTA >= lineno)
                || ((*p)->lineno >= lineno && (*p)->lineno - DELTA <= lineno))
            {
                FILEBROWSE *q = *p;
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
                FILEBROWSE *q = *p;
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
    newBrowse = (FILEBROWSE *)malloc(sizeof(FILEBROWSE));
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
void FileBrowseClose(DWINFO *info)
{
    FILEBROWSE **p = &fileBrowseInfo;
    fileBrowseCursor = NULL;
    while (*p)
    {
        if ((*p)->info == info)
        {
            FILEBROWSE *q = *p;
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
    FILEBROWSE *p = fileBrowseInfo;
    fileBrowseCursor = NULL;
    while (p)
    {
        FILEBROWSE *next = p->next;
        free(p);
        p = next;
    }
    fileBrowseInfo = NULL;
    fileBrowseCount = 0;
}
static int fbcomp(const void *a, const void *b)
{
    const FILEBROWSE *ab = *(FILEBROWSE **)a;
    const FILEBROWSE *bb = *(FILEBROWSE **)b;
    int n = strcmp(ab->info->dwName, bb->info->dwName);
    if (n != 0)
        return n;
    if (ab->lineno < bb->lineno)
        return -1;
    if (ab->lineno > bb->lineno)
        return 1;
    return 0;
}
static char *GetName(int k)
{
    static char names[MAX_BROWSE][20];
    char buf[256], *p;
    memset(buf, 0, 256);
    *(short*)buf = 256;

    SendMessage(ordered[k]->info->dwHandle, EM_GETLINE, (WPARAM)ordered[k]->lineno - 1, (LPARAM)
        buf);
    p = buf;
    while (isspace(*p))
        p++;
    if (*p)
    {
        int i;
        for (i=0; i < sizeof(names[0])-1 && *p; i++)
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
        FILEBROWSE *p = fileBrowseInfo;
        HMENU popup = CreatePopupMenu();    
        int i = 0, j;
        memset(ordered, 0, sizeof(ordered));
        while (p && i < MAX_BROWSE)
        {
            ordered[i++] = p;
            p = p->next;
        }
        qsort(ordered, i, sizeof(FILEBROWSE *), fbcomp);
    
        for (j=0; j < i;)
        {
            FILEBROWSE *cur = ordered[j];
            HMENU internalPop = CreatePopupMenu();
            for ( ;j < i && cur->info == ordered[j]->info; j++)
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