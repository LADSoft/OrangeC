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

#include "helpid.h"
#include "header.h"
#include <ctype.h>
#include <stdlib.h>

#define N_EDITDONE -4001

extern LOGFONT EditFont;
extern HINSTANCE hInstance;
extern HTREEITEM prjSelectedItem;
extern HWND prjTreeWindow;

PROJECTITEM *CreateFolder(PROJECTITEM *p, char *name, BOOL always )
{
    PROJECTITEM *folder;
    if (!always)
    {
        PROJECTITEM **ins = &p->children;
        while (*ins && stricmp((*ins)->displayName, name) < 0 && (*ins)->type == PJ_FOLDER)
        {
            ins = &(*ins)->next;
        }
        if (*ins && !stricmp((*ins)->displayName, name))
            return *ins;
    }
    folder = calloc(1, sizeof(PROJECTITEM));
    if (folder)
    {
        HTREEITEM pos = TVI_FIRST;
        PROJECTITEM **ins = &p->children;
        while (*ins && stricmp((*ins)->displayName, name) < 0 && (*ins)->type == PJ_FOLDER)
        {
            pos = (*ins)->hTreeItem;
            ins = &(*ins)->next;
        }
        strcpy(folder->displayName, name);
        folder->type = PJ_FOLDER;
        folder->parent = p;
        folder->next = *ins;
        *ins = folder;
        TVInsertItem(prjTreeWindow, p->hTreeItem, pos, folder);
        ResAddItem(folder);
        MarkChanged(folder, FALSE);
    }
    return folder;
}
void ProjectNewFolder(void)
{
    PROJECTITEM *data = GetItemInfo(prjSelectedItem);
    if (data && (data->type == PJ_FOLDER || data->type == PJ_PROJ))
    {
        CreateFolder(data, "New Folder", TRUE);
        ExpandParents(data);
    }
}
