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

#include "helpid.h"
#include "header.h"
#include <ctype.h>
#include <stdlib.h>

#define N_EDITDONE -4001

extern LOGFONT EditFont;
extern HINSTANCE hInstance;
extern HTREEITEM prjSelectedItem;
extern HWND prjTreeWindow;

PROJECTITEM* GetItemInfo(HTREEITEM item);

PROJECTITEM* CreateFolder(PROJECTITEM* p, char* name, BOOL always)
{
    PROJECTITEM* folder;
    if (!always)
    {
        PROJECTITEM** ins = &p->children;
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
        PROJECTITEM** ins = &p->children;
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
    PROJECTITEM* data = GetItemInfo(prjSelectedItem);
    if (data && (data->type == PJ_FOLDER || data->type == PJ_PROJ))
    {
        CreateFolder(data, "New Folder", TRUE);
        ExpandParents(data);
    }
}
