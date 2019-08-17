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

extern HINSTANCE hInstance;
extern char szCWSFilter[], szCTGFilter[];
extern HWND hwndFrame;
extern PROJECTITEM* activeProject;
extern PROJECTITEM* workArea;
extern HWND prjTreeWindow;
extern HTREEITEM prjSelectedItem;
extern HFONT projFont, boldProjFont, italicProjFont;
extern char szInstallPath[];

WNDPROC oldPrjEditProc;
HWND hwndEdit;
PROJECTITEM* editItem;
char* szprjEditClassName = "xccprjEditClass";

PROJECTITEM* GetItemInfo(HTREEITEM item);

void MoveChildrenUp(PROJECTITEM* data)
{
    PROJECTITEM* children = data->children;
    data->children = NULL;
    while (children)
    {
        PROJECTITEM** ins = &data->parent->children;
        HTREEITEM pos = TVI_FIRST;
        PROJECTITEM* next = children->next;
        while (*ins && stricmp((*ins)->displayName, children->displayName) < 0)
        {
            pos = (*ins)->hTreeItem;
            ins = &(*ins)->next;
        }
        children->next = *ins;
        (*ins) = children;
        children->parent = data->parent;
        TVInsertItem(prjTreeWindow, data->parent->hTreeItem, pos, children);
        ResAddItem(children);
        TreeView_Expand(prjTreeWindow, children->hTreeItem, children->expanded ? TVE_EXPAND : TVE_COLLAPSE);
        children = next;
    }
}
void FreeSubTree(PROJECTITEM* data, BOOL save)
{
    PROJECTITEM* children = data->children;
    PROJECTITEM** rmv;
    while (children)
    {
        PROJECTITEM* next = children->next;
        FreeSubTree(children, save);
        children = next;
    }
    if (data->parent)
    {
        rmv = &data->parent->children;
        while (*rmv)
        {
            if ((*rmv) == data)
            {
                *rmv = (*rmv)->next;
                ResFree(data, save);
                free(data);
                break;
            }
            rmv = &(*rmv)->next;
        }
    }
    else
    {
        ResFree(data, TRUE);
        free(data);
    }
}
void ProjectRemoveOne(PROJECTITEM* data)
{
    MarkChanged(data, data->type == PJ_PROJ);
    TreeView_DeleteItem(prjTreeWindow, data->hTreeItem);
    ResDeleteItem(data);
    if (data->type != PJ_PROJ)
    {
        PROJECTITEM* p = data;
        while (p->type != PJ_PROJ)
            p = p->parent;
        p->clean = TRUE;
    }
    if (data->type != PJ_FILE || !RetrieveInternalDepend(data->realName))
        FreeSubTree(data, TRUE);
    if (data == activeProject)
    {
        activeProject = workArea->children;
        while (activeProject && !activeProject->loaded)
            activeProject = activeProject->next;
        MarkChanged(activeProject, TRUE);
        InvalidateRect(prjTreeWindow, 0, 1);
    }
}
void ProjectRemoveAll(void)
{
    PROJECTITEM* root = workArea;
    root = root->children;
    while (root)
    {
        PROJECTITEM* next = root->next;
        if (root->type == PJ_PROJ)
            ProjectRemoveOne(root);
        root = next;
    }
}
void ProjectRemove(void)
{
    PROJECTITEM* data = GetItemInfo(prjSelectedItem);
    if (data)
    {
        BOOL del = FALSE;
        switch (data->type)
        {
            case PJ_PROJ:
                del = MessageBox(hwndFrame, "This project will be removed from the work area.", "Project Remove Item",
                                 MB_OKCANCEL | MB_APPLMODAL) == IDOK;
                if (del && data->changed)
                    SaveProject(data);
                break;
            case PJ_FILE:
                del = MessageBox(hwndFrame, "This file will be removed from the work area.", "Project Remove Item",
                                 MB_OKCANCEL | MB_APPLMODAL) == IDOK;
                break;
            case PJ_FOLDER:
                del = MessageBox(hwndFrame, "This folder will be removed from the work area.", "Project Remove Item",
                                 MB_OKCANCEL | MB_APPLMODAL) == IDOK;
                if (del)
                    MoveChildrenUp(data);
                break;
        }
        if (del)
        {
            ProjectRemoveOne(data);
        }
    }
}
void ProjectRename(void)
{
    PROJECTITEM* data = GetItemInfo(prjSelectedItem);
    if (data && (data->type == PJ_PROJ || data->type == PJ_FOLDER || data->type == PJ_FILE))
    {
        RECT r, s;
        editItem = data;
        TreeView_GetItemRect(prjTreeWindow, data->hTreeItem, &r, TRUE);
        TreeView_GetItemRect(prjTreeWindow, data->hTreeItem, &s, FALSE);
        hwndEdit = CreateWindow(szprjEditClassName, data->displayName, WS_CHILD | ES_AUTOHSCROLL | WS_BORDER, r.left, r.top,
                                s.right - r.left, r.bottom - r.top, GetWindowHandle(DID_PROJWND), (HMENU)449, hInstance, 0);
        SendMessage(hwndEdit, EM_SETSEL, 0, (LPARAM)-1);
        SendMessage(hwndEdit, EM_SETLIMITTEXT, 64, 0);
        ShowWindow(hwndEdit, SW_SHOW);
        SetFocus(hwndEdit);
    }
}
void DoneRenaming(void)
{
    char buf[MAX_PATH];
    char oldName[MAX_PATH];
    strcpy(oldName, editItem->realName);
    buf[0] = 0;
    GetWindowText(hwndEdit, buf, MAX_PATH);
    if (buf[0] != 0)
    {
        if (strcmp(buf, editItem->displayName))
        {
            PROJECTITEM** rmv = &editItem->parent->children;
            switch (editItem->type)
            {
                char buf2[MAX_PATH], buf3[MAX_PATH], *p;
                case PJ_FILE:
                case PJ_PROJ:
                    strcpy(buf2, editItem->realName);
                    p = strrchr(buf2, '\\');
                    if (p)
                        p++;
                    else
                        p = buf2;
                    strcpy(p, buf);
                    strcpy(buf3, editItem->realName);
                    if (editItem->type == PJ_PROJ)
                    {
                        char* q = strrchr(editItem->realName, '.');
                        if (q)
                        {
                            strcat(buf, q);
                            strcat(buf2, q);
                            strcat(buf2, ".cpj");
                            strcat(buf3, ".cpj");
                        }
                        else
                        {
                            DestroyWindow(hwndEdit);
                            return;
                        }
                    }
                    if (!MoveFileEx(buf3, buf2, 0))
                    {
                        DestroyWindow(hwndEdit);
                        return;
                    }
                    if (editItem->type == PJ_FILE)
                    {
                        EditRenameFile(editItem->realName, buf2);
                    }
                    break;
            }
            if (editItem->type == PJ_FILE || editItem->type == PJ_PROJ || editItem->type == PJ_WS)
            {
                char* p = stristr(editItem->realName, editItem->displayName);
                if (!p)
                    return;
                strcpy(p, buf);
            }
            strcpy(editItem->displayName, buf);
            if (editItem->type == PJ_PROJ)
            {
                char* q = strrchr(editItem->displayName, '.');
                *q = 0;
            }
            while (*rmv && (*rmv) != editItem)
                rmv = &(*rmv)->next;
            if (*rmv)
            {
                *rmv = (*rmv)->next;
            }
            TreeView_DeleteItem(prjTreeWindow, editItem->hTreeItem);
            ResDeleteItem(editItem);
            switch (editItem->type)
            {
                case PJ_FILE:
                    AddFile(editItem->parent, editItem->realName, FALSE);
                    ResFree(editItem, TRUE);
                    free(editItem);
                    break;
                case PJ_FOLDER:
                    CreateFolder(editItem->parent, editItem->displayName, TRUE);
                    ResFree(editItem, TRUE);
                    free(editItem);
                    break;
                case PJ_PROJ:
                {
                    PROJECTITEM** ins = &editItem->parent->children;
                    HTREEITEM pos = TVI_FIRST;
                    while (*ins && stricmp((*ins)->displayName, editItem->displayName) < 0)
                    {
                        pos = (*ins)->hTreeItem;
                        ins = &(*ins)->next;
                    }
                    editItem->next = NULL;
                    RecursiveCreateTree(editItem->parent->hTreeItem, pos, editItem);
                    editItem->next = *ins;
                    *ins = editItem;
                    MarkChanged(editItem, FALSE);
                    MarkChanged(editItem, TRUE);
                }
                break;
            }
        }
    }
    DestroyWindow(hwndEdit);
}
static int CustomDraw(HWND hwnd, LPNMTVCUSTOMDRAW draw)
{
    PROJECTITEM* p;
    switch (draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW;
        case CDDS_ITEMPREPAINT:
            p = (PROJECTITEM*)draw->nmcd.lItemlParam;
            if (p->type == PJ_PROJ && !p->loaded)
            {
                draw->clrText = RetrieveSysColor(COLOR_GRAYTEXT);
                draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                SelectObject(draw->nmcd.hdc, italicProjFont);
                return CDRF_NEWFONT;
            }
            else
            {
                if (activeProject == p)
                {
                    SelectObject(draw->nmcd.hdc, boldProjFont);
                    return CDRF_NEWFONT;
                }
                else if (((PROJECTITEM*)draw->nmcd.lItemlParam)->hTreeItem == TreeView_GetDropHilight(prjTreeWindow))
                {
                    draw->clrText = RetrieveSysColor(COLOR_HIGHLIGHTTEXT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_HIGHLIGHT);
                }
                else if (draw->nmcd.uItemState & (CDIS_SELECTED))
                {
                    draw->clrText = RetrieveSysColor(COLOR_HIGHLIGHT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                }
                else if (draw->nmcd.uItemState & (CDIS_HOT))
                {
                    draw->clrText = RetrieveSysColor(COLOR_INFOTEXT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_INFOBK);
                }
                else if (draw->nmcd.uItemState == 0)
                {
                    draw->clrText = RetrieveSysColor(COLOR_WINDOWTEXT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                }
                SelectObject(draw->nmcd.hdc, projFont);
                return CDRF_NEWFONT;
            }
            return CDRF_DODEFAULT;
        default:
            return CDRF_DODEFAULT;
    }
}
static BOOL SendImportCommand(char* fname, BOOL target)
{
    DWORD bRet;
    STARTUPINFO stStartInfo;
    PROCESS_INFORMATION stProcessInfo;
    DWORD retCode;
    char cmd[5000];
    sprintf(cmd, "\"%s\\bin\\upgradedb.exe\" %s \"%s\"", szInstallPath, target ? "proj" : "ws", fname);
    memset(&stStartInfo, 0, sizeof(STARTUPINFO));
    memset(&stProcessInfo, 0, sizeof(PROCESS_INFORMATION));

    stStartInfo.cb = sizeof(STARTUPINFO);
    bRet = CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, 0, szInstallPath, &stStartInfo, &stProcessInfo);
    if (!bRet)
    {
        return FALSE;
    }
    MsgWait(stProcessInfo.hProcess, INFINITE);
    GetExitCodeProcess(stProcessInfo.hProcess, &retCode);
    return retCode == 0;
}
void ImportProject(BOOL ctg)
{
    OPENFILENAME ofn;

    if (OpenFileDialog(&ofn, 0, GetWindowHandle(DID_PROJWND), FALSE, FALSE, ctg ? szCTGFilter : szCWSFilter,
                       ctg ? "Open Old Target File" : "Open Old Workspace File"))
    {
        char* q = stristr(ofn.lpstrFile, ctg ? ".ctg" : ".cws");
        if (!q)
            strcat(ofn.lpstrFile, ctg ? ".ctg" : ".cws");

        if (!SendImportCommand(ofn.lpstrFile, ctg))
        {
            ExtendedMessageBox("Conversion error", 0, "File %s could not be converted", ofn.lpstrFile);
        }
        else
        {
            if (ctg)
            {
                if (workArea)
                {
                    PROJECTITEM* p = workArea->children;
                    q = strrchr(ofn.lpstrFile, '.');
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
                        PROJECTITEM** ins = &workArea->children;
                        HTREEITEM pos = TVI_FIRST;
                        strcpy(p->realName, ofn.lpstrFile);
                        strcpy(p->displayName, ofn.lpstrFileTitle);
                        q = stristr(p->displayName, ".cpj");
                        if (q)
                            *q = 0;
                        p->type = PJ_PROJ;
                        p->parent = workArea;
                        while (*ins && stricmp((*ins)->displayName, p->displayName) < 0)
                        {
                            pos = (*ins)->hTreeItem;
                            ins = &(*ins)->next;
                        }
                        p->parent = workArea;
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
            else
            {
                q = strrchr(ofn.lpstrFile, '.');
                if (q)
                    *q = 0;
                strcat(ofn.lpstrFile, ".cwa");
                LoadWorkArea(ofn.lpstrFile, TRUE);
                SetWorkAreaMRU(workArea);
            }
        }
    }
}
LRESULT CALLBACK prjEditWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    NMHDR nm;
    switch (iMessage)
    {
        case WM_CHAR:
            if (wParam == 13)
            {
                nm.code = N_EDITDONE;
                nm.hwndFrom = hwnd;
                nm.idFrom = GetWindowLong(hwnd, GWL_ID);
                SendMessage(GetParent(hwnd), WM_NOTIFY, 0, (LPARAM)&nm);
                return 0;
            }
            if (wParam == 27)
            {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        case WM_KILLFOCUS:
            DestroyWindow(hwnd);
            return 0;
    }
    return CallWindowProc(oldPrjEditProc, hwnd, iMessage, wParam, lParam);
}
