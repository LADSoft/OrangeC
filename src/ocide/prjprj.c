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

extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern char szProjectFilter[] ;
extern int profileDebugMode;
extern char currentProfileName[256];
extern char *sysProfileName;
extern HWND hwndProject;
extern HWND prjTreeWindow;
extern PROJECTITEM *activeProject;
extern int defaultWorkArea;
extern PROJECTITEM *workArea;
extern HIMAGELIST treeIml;

static int newMode;
static char newName[MAX_PATH];
static char newTitle[MAX_PATH];
static char browsePath[MAX_PATH];

static int ParseNewProjectData(HWND hwnd)
{
    int rv = 1;
    int newDir = IsDlgButtonChecked(hwnd, IDC_NEWDIR) == BST_CHECKED;
    GetWindowText(GetDlgItem(hwnd, IDC_FILENEWPROJECT), newTitle, sizeof(newTitle));
    GetWindowText(GetDlgItem(hwnd, IDC_PROJECTNEWPROJECT), newName, sizeof(newName));
    newMode = ListView_GetSelectionMark(GetDlgItem(hwnd, IDC_LVNEWPROJECT));
    if (newTitle[0] == 0 || newName[0] == 0)
    {
        ExtendedMessageBox("Add New Project", 0, "Please specify a new project name");
        rv = 0;
    }
    else
    {
        WIN32_FIND_DATA aa;
        HANDLE ff;
        if (newDir)
        {
            char *p, *q;
            if (newName[strlen(newName)-1] != '\\')
                strcat(newName, "\\");
            strcat(newName, newTitle);
            p = strrchr(newName, '.');
            q = strrchr(newName, '\\');
            // strip any extension they have from the dir name
            if (p && (!q || q < p))
                *p = 0;
        }
        ff = FindFirstFile(newName, &aa);
        if (ff != INVALID_HANDLE_VALUE)
        {
            FindClose(ff);
            if (aa.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                int foundone = FALSE;
                char nbuf[MAX_PATH];
                strcpy(nbuf, newName);
                strcat(nbuf, "\\*.*");
                ff = FindFirstFile(nbuf, &aa);
                if (ff != INVALID_HANDLE_VALUE)
                {
                    do {
                        if (strcmp(aa.cFileName, ".") && strcmp(aa.cFileName, ".."))
                        {
                            if (!strstr(aa.cFileName, ".cwa"))
                                foundone = TRUE;
                            break;
                        }
                    }
                    while (FindNextFile(ff, &aa));
                    FindClose(ff);
                }
                if (foundone)
                {
                    if (ExtendedMessageBox("Exists", MB_YESNO, "Project directory exists and is not empty.\nCreate project anyway?") != IDYES)
                        rv = 0;
                }
            }
            else
            {
                ExtendedMessageBox("Invalid File", 0, "Project directory cannot be created");
                rv = 0;
            }
        }
        else
        {
            CreateFullPath(newName);
        }
        if (rv == 1)
        {
            strcat(newName, "\\");
            strcat(newName, newTitle);
            switch(newMode)
            {
                case BT_CONSOLE:
                case BT_WINDOWS:
                default:
                    if (strlen(newName) < 5 || stricmp(newName + strlen(newName)-4, ".exe") != 0)
                       strcat(newName, ".exe");
                    break;
                case BT_DLL:
                    if (strlen(newName) < 5 || stricmp(newName + strlen(newName)-4, ".dll") != 0)
                       strcat(newName, ".exe");
                    break;
                case BT_LIBRARY:
                    if (strlen(newName) < 3 || stricmp(newName + strlen(newName)-2, ".l") != 0)
                       strcat(newName, ".l");
                    break;
            }
        }
    }
    if (rv)
        rv = VerifyPath(newName);
    return rv;
}
static int CreateNewProjectData(HWND hwnd)
{
    int items = 0;
    LV_ITEM item;
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_LVNEWPROJECT);
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_FULLROWSELECT);
    ListView_SetImageList(hwndLV, ImageList_Duplicate(treeIml), LVSIL_SMALL);

    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM ;
    lvC.cx = r.right - r.left;
    lvC.iSubItem = 0;
    ListView_InsertColumn(hwndLV, 0, &lvC);

    memset(&item, 0, sizeof(item));
    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE;
    item.iImage = IL_CONSOLE;
    item.pszText = "Console";
    ListView_InsertItem(hwndLV, &item);
    
    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE;
    item.iImage = IL_GUI;
    item.pszText = "Windows GUI";
    ListView_InsertItem(hwndLV, &item);
    
    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE;
    item.iImage = IL_DLL;
    item.pszText = "Windows DLL";
    ListView_InsertItem(hwndLV, &item);
    
    item.iItem = items++;
    item.iSubItem = 0;
    item.mask = LVIF_TEXT | LVIF_IMAGE;
    item.iImage = IL_LIB;
    item.pszText = "Static Library";    
    ListView_InsertItem(hwndLV, &item);
    
    ListView_SetSelectionMark(hwndLV, 0);
    ListView_SetItemState(hwndLV, 0, LVIS_SELECTED, LVIS_SELECTED);

    newTitle[0] = 0;
    if (!defaultWorkArea)
    {
        char *p;
        strcpy(newName,workArea->realName);
        p = strrchr(newName, '\\');
        if (p)
            p[0] = 0;
    }
    else
    {
        GetDefaultProjectsPath(newName);
    }
    CheckDlgButton(hwnd, IDC_NEWDIR, BST_CHECKED);
    SendDlgItemMessage(hwnd, IDC_FILENEWPROJECT, WM_SETTEXT, 0, (LPARAM) newTitle);
    SendDlgItemMessage(hwnd, IDC_FILENEWPROJECT, EM_SETLIMITTEXT, MAX_PATH, 0);
    SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, WM_SETTEXT, 0, (LPARAM) newName);
    SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, EM_SETLIMITTEXT, MAX_PATH, 0);
    strcpy(browsePath, newName);
    return items;
}
static int CustomDrawNewProject(HWND hwnd, LPNMLVCUSTOMDRAW draw)
{
    HWND hwndLV;
    switch(draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT :
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            hwndLV = GetDlgItem(hwnd, IDC_LVNEWPROJECT);
            // we do it this way so the selection won't go away when we focus another control
            if (ListView_GetItemState(hwndLV, draw->nmcd.dwItemSpec, LVIS_SELECTED) & LVIS_SELECTED)
            {
                draw->clrText = RetrieveSysColor(COLOR_HIGHLIGHTTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_HIGHLIGHT);
            }
            else
            {
                draw->clrText = RetrieveSysColor(COLOR_WINDOWTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
            }
            return CDRF_NEWFONT;
        default:
            return CDRF_DODEFAULT;
    }
}
//-------------------------------------------------------------------------

long APIENTRY NewProjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
           if (!CreateNewProjectData(hwnd))
           {
                EndDialog(hwnd, 1);
           }
            else
            {
                CenterWindow(hwnd);
            }
            return 1;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                SetWindowLong(hwnd, DWL_MSGRESULT, CustomDrawNewProject(hwnd, (LPNMLVCUSTOMDRAW)lParam));
                return TRUE;
            }
            else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
            {
                switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                {
                    case VK_INSERT:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_LVNEWPROJECT);
                            ListView_SetSelectionMark(hwndLV, -1);
                        }
                        else
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_LVNEWPROJECT);
                            int i = ListView_GetSelectionMark(hwndLV);
                            ListView_SetSelectionMark(hwndLV, i);
                            ListView_SetItemState(hwndLV, i, LVIS_SELECTED, LVIS_SELECTED);
                        }
                        break;
                }
            }
            return 0;
        case WM_COMMAND:
            switch (wParam &0xffff)
            {
            case IDC_NEWPROJECTBROWSE:
                BrowseForFile(hwndFrame, "New Project Directory", browsePath, MAX_PATH);
                SendDlgItemMessage(hwnd, IDC_PROJECTNEWPROJECT, WM_SETTEXT, 0, (LPARAM)browsePath);
                break;
            case IDOK:
                if (ParseNewProjectData(hwnd))
                    EndDialog(hwnd, IDOK);
                break;
            case IDCANCEL:
                EndDialog(hwnd, IDCANCEL);
                break;
            case IDHELP:
                ContextHelp(IDH_NEW_PROJECT_DIALOG);
                break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
    }
    return 0;
}
void SetProjectType(PROJECTITEM *pj, int newMode)
{
    int n = profileDebugMode;
    char num[256];
    SETTING *set;
    sprintf(num ,"%d", newMode);

    profileDebugMode = 0;
    set = calloc(1, sizeof(SETTING));
    InsertSetting(pj,set);
    set->id = strdup("__PROJECTTYPE");
    set->type = e_numeric;
    set->value = strdup(num);

    profileDebugMode = 1;
    set = calloc(1, sizeof(SETTING));
    InsertSetting(pj,set);
    set->id = strdup("__PROJECTTYPE");
    set->type = e_numeric;
    set->value = strdup(num);

    profileDebugMode = n;
}
void PropagateAllProjectTypes(void)
{
    int n = profileDebugMode;
    char m[256];
    PROJECTITEM *pj = workArea->children;
    strcpy(m, currentProfileName);
    profileDebugMode = 0;
    while (pj)
    {
        SETTING *s;
        char *rls = NULL;
        strcpy(currentProfileName, sysProfileName);
        s = PropFind(GetSettings(pj->profiles), "__PROJECTTYPE");
        if (s)
        {
            rls = s->value;
            strcpy(currentProfileName, m);
            SetProjectType(pj, atoi(rls));
            MarkChanged(pj, FALSE);
        }
        pj = pj->next;
    }
    profileDebugMode = n;
    strcpy(currentProfileName, m);
}
void ProjectNewProject(void)
{
    PROJECTITEM *data = workArea;
    if (data && data->type == PJ_WS)
    {
        if (DialogBox(hInstance, "IDD_NEWPROJECT", hwndFrame, (DLGPROC)NewProjectProc) == IDOK)
        {
            PROJECTITEM *p = workArea->children;
            while (p)
                if (!stricmp(p->realName, newName))
                    return;
                else
                    p = p->next;
            p = calloc(1, sizeof(PROJECTITEM));
            if (p)
            {
                PROJECTITEM **ins = &workArea->children;
                HTREEITEM pos = TVI_FIRST;
                int imagetype;
                strcpy( p->displayName, newTitle);
                strcpy( p->realName, newName);
                p->expanded = TRUE;
                p->parent = workArea;
                p->type = PJ_PROJ;
                p->loaded = TRUE;
                SetProjectType(p, newMode);
                imagetype = imageof(p, p->realName);
                if (!activeProject)
                {
                    activeProject = p;
                    MarkChanged(activeProject, TRUE);
                }
                while (*ins && stricmp((*ins)->displayName, p->displayName) < 0)
                {
                    pos = (*ins)->hTreeItem;
                    ins = &(*ins)->next;
                }
                p->next = *ins;
                *ins = p;
                TVInsertItem(prjTreeWindow, workArea->hTreeItem, pos, p);
                ResAddItem(p);
                CreateFolder(p, "Include Files", FALSE);
                CreateFolder(p, "Source Files", FALSE);
                ExpandParents(p);
                MarkChanged(p, TRUE);
                MarkChanged(p, FALSE);
            }
        }
    }
}
void ProjectExistingProject(void)
{
    PROJECTITEM *data = workArea;
    if (data && data->type == PJ_WS)
    {
        OPENFILENAME ofn;

        if (OpenFileDialog(&ofn, 0, hwndProject, FALSE, FALSE, szProjectFilter,
                               "Open existing project"))
        {
            PROJECTITEM *p = workArea->children;
            char *q;
            q = stristr(ofn.lpstrFile,".cpj");
            if (q)
                *q = 0;            
            while (p)
                if (!stricmp(p->realName, ofn.lpstrFile))
                    return;
                else
                    p = p->next;
            p = calloc(1, sizeof(PROJECTITEM));
            if (p)
            {
                PROJECTITEM **ins = &workArea->children;
                HTREEITEM pos = TVI_FIRST;
                strcpy(p->realName, ofn.lpstrFile);
                q = strrchr(p->realName, '\\');
                if (!q)
                    q = p->realName;
                else
                    q++;
                strcpy(p->displayName, q);
                p->type = PJ_PROJ;
                p->parent = data;
                while (*ins && stricmp((*ins)->displayName, p->displayName) < 0)
                {
                    pos = (*ins)->hTreeItem;
                    ins = &(*ins)->next;
                }
                p->parent = data;
                RestoreProject(p, FALSE);
                RecursiveCreateTree(workArea->hTreeItem, pos, p);
                p->next = *ins;
                *ins = p;
                ExpandParents(p);
                activeProject = p;
                MarkChanged(activeProject, TRUE);
            }
        }
    }
}
//-------------------------------------------------------------------------
void SaveAllProjects(PROJECTITEM *workArea, BOOL always)
{
    if (workArea)
    {
        PROJECTITEM *p = workArea->children;
        if (always || workArea->changed)
            SaveWorkArea(workArea);
        while (p)
        {
            if (p->changed)
            {
                char *q = strrchr(p->realName, '\\');
                if (q)
                    q++;
                else
                    q = p->realName;
            if (ExtendedMessageBox("Save projects", MB_YESNO,"Project %s has changed.\nDo you want to save it?", q) == IDYES)
                    SaveProject(p);
            }
            p = p->next;
        }
    }
}
void LoadProject(char *name)
{
    PROJECTITEM *p = calloc(1, sizeof(PROJECTITEM)), **ins = NULL;
    if (p)
    {
        strcpy(p->realName, name);
        p->type = PJ_PROJ;
        p->displayName[0] = 0;
        RestoreProject(p, TRUE);
        if (p->displayName[0])
        {
            while ((*ins) && stricmp((*ins)->displayName, p->displayName) < 0)
                ins = &(*ins)->next;
            p->next = *ins;
            *ins = p;
            TreeView_DeleteAllItems(prjTreeWindow);
            ResDeleteAllItems();
            RecursiveCreateTree(TVI_ROOT, TVI_FIRST, workArea);
        }
        else
        {
            free(p);
        }
    }
}
//-------------------------------------------------------------------------

void IndirectProjectWindow(DWINFO *info)
{
    dmgrHideWindow(DID_TABWND, FALSE);
    LoadWorkArea(info->dwName, TRUE);
}
