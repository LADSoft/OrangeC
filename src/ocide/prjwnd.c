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

extern int making;
extern enum DebugStates uState;
extern HINSTANCE hInstance;
extern HWND hwndClient, hwndFrame;
extern HWND hwndTab;
extern HWND hwndTabCtrl;
extern char szInstallPath[];
extern char szWorkAreaTitle[256];
extern PROJECTITEM *workArea;
extern WNDPROC oldPrjEditProc;
extern HWND hwndEdit;
extern PROJECTITEM *editItem;
extern char *szprjEditClassName;
extern LOGFONT systemDialogFont;

LRESULT CALLBACK prjEditWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);

HWND hwndProject;
PROJECTITEM *activeProject;
CRITICAL_SECTION projectMutex;
HWND prjTreeWindow;
HIMAGELIST treeIml;
HFONT projFont, boldProjFont, italicProjFont;

static char szProjectClassName[] = "xccProjectClass";
static HBITMAP folderClose, folderOpen, mainIml;
static int ilfolderClose, ilfolderOpen;
static int treeViewSelected;
HTREEITEM prjSelectedItem;

static HCURSOR dragCur, noCur;

static void SetExt(OPENFILENAME *ofn, char *ext)
{
    char *p = strrchr(ofn->lpstrFileTitle, '.');
    if (!p || stricmp(p, ext))
        strcat(ofn->lpstrFileTitle, p); 
}

//-------------------------------------------------------------------------

int imageof(PROJECTITEM *data, char *name)
{
    if (data->type == PJ_WS)
        return IL_CWA;
    if (data->type == PJ_FOLDER)
        return ilfolderClose;
    name = strrchr(name, '.');
    if (data->type == PJ_PROJ)
    {
        if (!xstricmpz(name, ".exe"))
        {
            SETTING *s;
            PropSearchProtos(data, "__PROJECTTYPE", &s);
            if (s && atoi(s->value) == BT_CONSOLE)
                return IL_CONSOLE;
            else
                return IL_GUI;
        }
        if (!xstricmpz(name, ".lib"))
            return IL_LIB;
        if (!xstricmpz(name, ".dll"))
            return IL_DLL;
        return IL_GUI;
    }
    if (!name)
        return IL_FILES;
    if (!xstricmpz(name, ".asm"))
        return IL_ASM;
    if (!xstricmpz(name, ".c") || !xstricmpz(name, ".cpp") || !xstricmpz(name, ".cxx"))
        return IL_C;
    if (!xstricmpz(name, ".rc") || !xstricmpz(name, ".bmp") 
            || !xstricmpz(name, ".cur") || !xstricmpz(name, ".ico"))
        return IL_RES;
    if (!xstricmpz(name, ".h"))
        return IL_H;
    if (!xstricmpz(name, ".cwa"))
        return IL_CWA;
    return IL_FILES;
}

void ResyncProjectIcons(void)
{
    PROJECTITEM *p = workArea;
    if (p)
    {
        p = workArea->children;
        while (p)
        {
            TV_ITEM t;
            memset(&t, 0, sizeof(t));
            t.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
            t.hItem = p->hTreeItem;
            t.iImage = t.iSelectedImage = imageof(p, p->realName);
            TreeView_SetItem(prjTreeWindow, &t);
            p = p->next;
        }
        InvalidateRect(prjTreeWindow, 0, 1);
    }
}
void MarkChanged(PROJECTITEM *item, BOOL ws)
{
    if (ws)
    {
        while (item && item->type != PJ_WS)
            item = item->parent ;
        if (item)
            item->changed = TRUE;
    }
    else
    {
        while (item && item->type != PJ_PROJ)
            item = item->parent ;
        if (item)
            item->changed = TRUE;
    }
}
//-------------------------------------------------------------------------

PROJECTITEM *GetItemInfo(HTREEITEM item)
{
    TV_ITEM xx ;
    if (!item)
        return NULL;
    xx.hItem = item;
    xx.mask = TVIF_PARAM ;
    if (SendMessage(prjTreeWindow, TVM_GETITEM,0 ,(LPARAM)&xx))
        return (PROJECTITEM *)xx.lParam ;
    return NULL;
}

//-------------------------------------------------------------------------

void TVInsertItem(HWND hTree, HTREEITEM hParent, HTREEITEM after, 
                       PROJECTITEM *data)
{
    TV_INSERTSTRUCT t;
    memset(&t, 0, sizeof(t));
    t.hParent = hParent;
    t.hInsertAfter = after;
    t.UNNAMED_UNION item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
    t.UNNAMED_UNION item.hItem = 0;
    t.UNNAMED_UNION item.pszText = data->displayName;
    t.UNNAMED_UNION item.cchTextMax = strlen(data->displayName);
    t.UNNAMED_UNION item.iImage = t.UNNAMED_UNION item.iSelectedImage = imageof(data, data->realName);
    t.UNNAMED_UNION item.lParam = (LPARAM)data ;
    data->hTreeItem = TreeView_InsertItem(hTree, &t);
}
void ExpandParents(PROJECTITEM *p)
{
    while (p)
    {
        TreeView_Expand(prjTreeWindow, p->hTreeItem, TVE_EXPAND);
        p = p->parent;
    }
}
void RecursiveCreateTree(HTREEITEM parent, HTREEITEM pos, PROJECTITEM *proj)
{
    while (proj)
    {
           TVInsertItem(prjTreeWindow, parent, pos, proj);
        ResAddItem(proj);
        if (proj->children)
        {
            RecursiveCreateTree(proj->hTreeItem, TVI_LAST, proj->children);
            TreeView_Expand(prjTreeWindow, proj->hTreeItem, proj->expanded ? TVE_EXPAND : TVE_COLLAPSE);
        }
        proj = proj->next;
    }
}
void ProjectSetActive()
{
    PROJECTITEM *data = GetItemInfo(prjSelectedItem);
    if (data && data->type == PJ_PROJ)
    {
        TV_ITEM t;
        memset(&t, 0, sizeof(t));
        activeProject = data;
        MarkChanged(data, TRUE);
        t.mask = TVIF_TEXT;
        t.hItem = data->hTreeItem;
        t.pszText = data->displayName;
        t.cchTextMax = strlen(data->displayName);
        TreeView_SetItem(prjTreeWindow, &t);
        InvalidateRect(prjTreeWindow, 0, 1);
        FreeJumpSymbols();
        LoadJumpSymbols();
    }
}
void DragTo(HTREEITEM dstItem, HTREEITEM srcItem)
{
    PROJECTITEM *srcData = GetItemInfo(srcItem);
    PROJECTITEM *dstData = GetItemInfo(dstItem);
    if (srcData && dstData && srcData->parent != dstData)
    {
        PROJECTITEM *p = dstData->parent;
        while(p)
            if (p == srcData)
                return;
            else
                p = p->parent;
        if (srcData->type == PJ_FOLDER || srcData->type == PJ_FILE)
        {
            if (dstData->type == PJ_FOLDER || dstData->type == PJ_PROJ)
            {
                PROJECTITEM **rmv = &srcData->parent->children;
                MarkChanged(srcData,FALSE);
                while (*rmv && *rmv != srcData)
                    rmv = &(*rmv)->next;
                if (*rmv)
                {
                    PROJECTITEM **ins = &dstData->children;
                    HTREEITEM pos = TVI_FIRST;
                    (*rmv) = (*rmv)->next;

                    TreeView_DeleteItem(prjTreeWindow, srcData->hTreeItem);
                    ResDeleteItem(srcData);
                    if (srcData->type == PJ_FILE)
                    {
                        while (*ins && (*ins)->type == PJ_FOLDER)
                        {
                            pos = (*ins)->hTreeItem;
                            ins = &(*ins)->next;
                        }
                    }
                    while (*ins && (*ins)->type == srcData->type && stricmp((*ins)->displayName, srcData->displayName) < 0)
                    {
                        pos = (*ins)->hTreeItem;
                        ins = &(*ins)->next;
                    }
                    srcData->parent = dstData;
                    srcData->next = NULL;
                    RecursiveCreateTree(dstData->hTreeItem, pos, srcData);
                    srcData->next = *ins;
                    *ins = srcData;
                    MarkChanged(srcData,FALSE);
                }
            }
        }
    }
}
//-------------------------------------------------------------------------

HTREEITEM FindItemRecursive(PROJECTITEM *l, DWINFO *info)
{
    while (l)
    {
        if (l->type == PJ_PROJ || l->type == PJ_FOLDER)
        {
            HTREEITEM rv = FindItemRecursive(l->children, info);
            if (rv)
                return rv;
        }
        else
        {
            if (!stricmp(l->realName, info->dwName))
                return l->hTreeItem;
        }
        l = l->next;
    }
    return NULL;
}
HTREEITEM FindItemByWind(HWND hwnd)
{
    DWINFO *info = (DWINFO*)GetWindowLong(hwnd, 0);
    if (!info)
        return 0 ;
    if (!workArea)
        return 0;
    return FindItemRecursive(workArea->children, info);
}

void CreateProjectMenu(void)
{
    PROJECTITEM *cur = GetItemInfo(prjSelectedItem);
    HMENU menu = NULL;
    if (cur)
    {
        switch(cur->type)
        {
            case PJ_WS:
                menu = LoadMenuGeneric(hInstance, "WORKAREAMENU");
                break;
            case PJ_PROJ:
                if (cur->loaded)
                    menu = LoadMenuGeneric(hInstance, "PROJECTMENU");
                else
                    menu = LoadMenuGeneric(hInstance, "PROJECTREMOVEMENU");
                break;
            case PJ_FOLDER:

                menu = LoadMenuGeneric(hInstance, "FOLDERMENU");
                break;
            case PJ_FILE:
                menu = LoadMenuGeneric(hInstance, "FILEMENU");
                break;
            default:
                menu = NULL;
                break;
        }
    }
    else
    {
        menu = LoadMenuGeneric(hInstance, "WORKAREAMENU");
    }
    if (menu)
    {
        HMENU popup = GetSubMenu(menu, 0);
        POINT pos;
        GetCursorPos(&pos);
        InsertBitmapsInMenu(popup);
        TrackPopupMenuEx(popup, TPM_TOPALIGN | TPM_LEFTBUTTON, pos.x,
            pos.y, hwndFrame, NULL);
        DestroyMenu(menu);
    }
}
//-------------------------------------------------------------------------
static int CustomDraw(HWND hwnd, LPNMTVCUSTOMDRAW draw)
{
    PROJECTITEM *p;
    switch(draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT :
            return CDRF_NOTIFYITEMDRAW ;
        case CDDS_ITEMPREPAINT:
            p = (PROJECTITEM *)draw->nmcd.lItemlParam;
            if (p->type == PJ_PROJ && !p->loaded)
            {
                draw->clrText= RetrieveSysColor(COLOR_GRAYTEXT);
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
                else if (((PROJECTITEM *)draw->nmcd.lItemlParam)->hTreeItem == TreeView_GetDropHilight(prjTreeWindow))
                {
                    draw->clrText= RetrieveSysColor(COLOR_HIGHLIGHTTEXT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_HIGHLIGHT);
                }
                else if (draw->nmcd.uItemState & (CDIS_SELECTED ))
                {
                    draw->clrText= RetrieveSysColor(COLOR_HIGHLIGHT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                }
                else if (draw->nmcd.uItemState & (CDIS_HOT))
                {
                    draw->clrText= RetrieveSysColor(COLOR_INFOTEXT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_INFOBK);
                }
                else if (draw->nmcd.uItemState == 0)
                {
                    draw->clrText= RetrieveSysColor(COLOR_WINDOWTEXT);
                    draw->clrTextBk = RetrieveSysColor(COLOR_WINDOW);
                }
                SelectObject(draw->nmcd.hdc, projFont);
                return CDRF_NEWFONT;    
            }
            return CDRF_DODEFAULT;
    }
}

LRESULT CALLBACK ProjectProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    int i;
    RECT rs;
    NM_TREEVIEW *nm;
    DWINFO info;
    LPNMTVKEYDOWN key;
    PROJECTITEM *data;
    TVHITTESTINFO hittest;
    HWND win;
    HTREEITEM oldSel;
    static HCURSOR origCurs;
    static BOOL dragging;
    static BOOL inView;
    static HTREEITEM srcItem, dstItem;
    switch (iMessage)
    {
        LOGFONT lf;
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case WM_SETTEXT:
            return SendMessage(hwndTab, iMessage, wParam, lParam);
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            SetFocus(hwnd);
            break;
        case WM_NOTIFY:
            nm = (NM_TREEVIEW*)lParam;
            switch (nm->hdr.code)
            {
            case NM_CUSTOMDRAW:
                return CustomDraw(hwnd, (LPNMTVCUSTOMDRAW)nm);
            case N_EDITDONE:
                DoneRenaming();
                break;
            case TVN_BEGINDRAG:
                GetCursorPos(&hittest.pt);
                ScreenToClient(prjTreeWindow, &hittest.pt);
                srcItem = TreeView_HitTest(prjTreeWindow, &hittest);
                data = GetItemInfo(srcItem);
                if (data && (data->type == PJ_FILE || data->type == PJ_FOLDER))
                {
                    dragging = TRUE;
                    SetCapture(hwnd);
                    origCurs = SetCursor(dragCur);
                    inView = TRUE;
                }
                break;
            case TVN_KEYDOWN:
                key = (LPNMTVKEYDOWN)lParam;
                switch (key->wVKey)
                {
                    case VK_INSERT:
                        if (GetKeyState(VK_CONTROL) &0x80000000)
                        {
                            data = GetItemInfo(prjSelectedItem);
                            if (data)
                            {
                                int msg = -1;
                                switch (data->type)
                                {
                                    case PJ_WS:
                                        msg = IDM_EXISTINGPROJECT;
                                        break;
                                    case PJ_PROJ:
                                        msg = IDM_NEWFOLDER;
                                        break;
                                    case PJ_FOLDER:
                                        msg = IDM_EXISTINGFILE;
                                        break;
                                }
                                if (msg != -1)
                                    PostMessage(hwnd, WM_COMMAND, msg, 0);
                            }
                        }
                        else if (GetKeyState(VK_SHIFT) &0x80000000)
                        {
                            data = GetItemInfo(prjSelectedItem);
                            if (data)
                            {
                                int msg = -1;
                                switch (data->type)
                                {
                                    case PJ_WS:
                                        msg = IDM_NEWPROJECT;
                                        break;
                                    case PJ_PROJ:
                                        msg = IDM_NEWFOLDER;
                                        break;
                                    case PJ_FOLDER:
                                        msg = IDM_NEWFILE_P;
                                        break;
                                }
                                if (msg != -1)
                                    PostMessage(hwnd, WM_COMMAND, msg, 0);
                            }
                        }
                        else 
                        {
                            data = GetItemInfo(prjSelectedItem);
                            if (data && (data->type != PJ_WS))
                                PostMessage(hwnd, WM_COMMAND, IDM_RENAME, 0);
                        }
                        break;
                    case VK_DELETE:
                        if (!(GetKeyState(VK_CONTROL) &0x80000000) && !(GetKeyState(VK_SHIFT) &0x8000000))
                        {
                            data = GetItemInfo(prjSelectedItem);
                            if (data && (data->type == PJ_FOLDER || data->type == PJ_FILE))
                                PostMessage(hwnd, WM_COMMAND, IDM_REMOVE, 0);
                        }
                        break;
                    case VK_RETURN:
                        SendMessage(hwnd, WM_COMMAND, IDM_OPENFILES, 0);
                        break;
                    }
                    break;
                case NM_DBLCLK:
                    oldSel = prjSelectedItem;
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(prjTreeWindow, &hittest.pt);
                    prjSelectedItem = TreeView_HitTest(prjTreeWindow, &hittest);
                    if (prjSelectedItem)
                        PostMessage(hwnd, WM_COMMAND, IDM_OPENFILES, 0);
                    prjSelectedItem = oldSel;
                    return 0;
                case NM_RCLICK:
                    GetCursorPos(&hittest.pt);
                    ScreenToClient(prjTreeWindow, &hittest.pt);
                    prjSelectedItem = TreeView_HitTest(prjTreeWindow, &hittest);
                    if (prjSelectedItem)
                    {
                        TreeView_SelectItem(prjTreeWindow, prjSelectedItem);
                    }
                    CreateProjectMenu();
                    break;
                case TVN_SELCHANGED:
                    nm = (NM_TREEVIEW*)lParam;
                    prjSelectedItem = nm->itemNew.hItem;
                    if (prjSelectedItem == 0)
                        prjSelectedItem = workArea->hTreeItem;
                    break;
                case TVN_ITEMEXPANDED:
                    nm = (NM_TREEVIEW *)lParam;
                    data = GetItemInfo(nm->itemNew.hItem);
                    if (data)
                    {
                        if (data->type == PJ_FOLDER)
                        {
                            TV_ITEM setitem;
                            memset(&setitem, 0, sizeof(setitem));
                            setitem.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                            setitem.iImage = setitem.iSelectedImage = 
                                nm->action == TVE_EXPAND ? ilfolderOpen : ilfolderClose;
                            setitem.hItem = nm->itemNew.hItem;
                            TreeView_SetItem(prjTreeWindow, &setitem);
                        }
                        if (nm->action == TVE_EXPAND)
                        {
                            data->expanded = TRUE;
                        }
                        else
                            data->expanded = FALSE;
                        return 0;
                    }
                    break;
                case TVN_DELETEITEM:
                    nm = (NM_TREEVIEW *)lParam;
                    if (nm->itemOld.hItem == prjSelectedItem)
                        prjSelectedItem = TreeView_GetSelection(prjTreeWindow);
                    break;
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDM_IMPORT_CWS:
                ImportProject(FALSE);
                break;
            case IDM_IMPORT_CTG:
                ImportProject(TRUE);
                break;
            case IDM_DOSWINDOW:
            {
                DosWindow(activeProject ? activeProject->realName : NULL, NULL, NULL, NULL, NULL);
            }
                break;
            case IDM_MAKEWINDOW:
            {
                char exec[MAX_PATH];
                sprintf(exec, "%s\\bin\\imake.exe", szInstallPath);
                DosWindow(activeProject ? activeProject->realName : NULL, exec, "", "Custom Make", "Make Is Complete.");
            }
                break;
            case IDM_RUN:
                SaveWorkArea(workArea);
                dbgRebuildMain(wParam);
                break;
            case IDM_SETACTIVEPROJECT:
                ProjectSetActive();
                break;
            case IDM_NEWFILE_P:
                ProjectNewFile();
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case IDM_EXISTINGFILE:
                ProjectExistingFile();
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case IDM_NEWPROJECT:
                ProjectNewProject();
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case IDM_EXISTINGPROJECT:
                ProjectExistingProject();
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                break ;
            case IDM_REMOVE:
                ProjectRemove();
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case IDM_RENAME:
                ProjectRename();
                break;
            case IDM_NEWFOLDER:
                ProjectNewFolder();
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                break;
            case IDM_NEWWS:
                if (uState != notDebugging)
                {
                    if (ExtendedMessageBox("WorkArea", MB_OKCANCEL, 
                        "This action requires the debugger to be stopped.") != IDOK)
                    {
                        break;
                    }
                    abortDebug();
                }
                dmgrHideWindow(DID_TABWND, FALSE);
                ProjectNewWorkArea();
                break;
            case IDM_OPENWS:
                if (uState != notDebugging)
                {
                    if (ExtendedMessageBox("WorkArea", MB_OKCANCEL, 
                        "This action requires the debugger to be stopped.") != IDOK)
                    {
                        break;
                    }
                    abortDebug();
                }
                dmgrHideWindow(DID_TABWND, FALSE);
                ProjectExistingWorkArea();
                break;
            case IDM_CLOSEWS:
                if (making)
                    break;
                if (uState != notDebugging)
                {
                    if (ExtendedMessageBox("WorkArea", MB_OKCANCEL, 
                        "This action requires the debugger to be stopped.") != IDOK)
                    {
                        break;
                    }
                    abortDebug();
                }
                CloseWorkArea();
                break;
            case IDM_SAVEWS:
                SaveAllProjects(workArea, TRUE);
                break;
            case IDM_COMPILEFILEFROMTREE:
            {
                PROJECTITEM *data = GetItemInfo(prjSelectedItem);
                if (data && data->type == PJ_FILE) 
                {
                    unlink(data->outputName);
                    Maker(data, TRUE);
                }        
            }
                break;
            case IDM_COMPILEFILE:
                win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                if (IsWindow(win) && IsEditWindow(win))
                {
                    HTREEITEM item = FindItemByWind(win);
                    PROJECTITEM *data = GetItemInfo(item);
                    if (data) {
                        unlink(data->outputName);
                        Maker(data, TRUE);
                    }
                }
                break;
            case IDM_GENMAKE:
                if (workArea && workArea->children)
                {
                    genMakeFile(workArea);
                }
                else
                {
                    ExtendedMessageBox("Makefile Generation", MB_SETFOREGROUND |
                        MB_SYSTEMMODAL, 
                        "You need at least one project to generate a make file");
                }
                break;
            case IDM_MAKE:
                if (HIWORD(wParam))
                    if (GetKeyState(VK_CONTROL) &0x80000000)
                        SendMessage(hwnd, WM_COMMAND, IDM_COMPILEFILE, 0);
                    else if (GetKeyState(VK_SHIFT) &0x80000000)
                        Maker(activeProject, FALSE);
                    else
                        Maker(workArea, FALSE);
                else
                    Maker(workArea, FALSE);
                break;
            case IDM_BUILDALL:
                Maker(workArea, TRUE);
                break;
            case IDM_BUILDSELECTED:
                Maker(activeProject, FALSE);
                break;
            case IDM_STOPBUILD:
                StopBuild();
                break;
            case IDM_CALCULATEDEPENDS:
                CalculateProjectDepends(GetItemInfo(prjSelectedItem));
                break;
            case IDM_RUNNODEBUG:
            {
                SaveWorkArea(workArea);
                RunProgram(activeProject);
                break;
            }
            case IDM_SELECTPROFILE:
                SelectProfileDialog();
                break;
            case IDM_ACTIVEPROJECTPROPERTIES:
                if (activeProject)
                    prjSelectedItem = activeProject->hTreeItem;
                // fall through
            case IDM_PROJECTPROPERTIES:
                data = GetItemInfo(prjSelectedItem);
                ShowBuildProperties(data);
                break;
            case IDM_OPENFILES:
                data = GetItemInfo(prjSelectedItem);
                if (data)
                    if (data->type == PJ_FILE)
                    {
                        if (strlen(data->realName) >= 3 && !stricmp(data->realName + strlen(data->realName) -3, ".rc"))
                        {
                            NavigateToResource(data);
                        }
                        else
                        {
                            strcpy(info.dwName, data->realName);
                            strcpy(info.dwTitle, data->displayName);
                            info.dwLineNo =  - 1;
                            info.logMRU = FALSE;
                            info.newFile = FALSE;
                            CreateDrawWindow(&info, TRUE);
                        }
                    }
                break;
            case IDM_CLOSE:
                SendMessage(hwnd, WM_CLOSE, 0, 0);
                break;
            default:
                return DefWindowProc(hwnd, iMessage, wParam, lParam);
            }
            break;
        case WM_LBUTTONUP:
            if (dragging)
            {
                SetCursor(origCurs);
                ReleaseCapture();
                dragging = FALSE;
                TreeView_SelectDropTarget(prjTreeWindow, NULL);
                if (inView && dstItem != srcItem && srcItem && dstItem)
                {
                    DragTo(dstItem, srcItem);
                }
            }
            break;
        case WM_MOUSEMOVE:
            if (dragging)
            {
                hittest.pt.x = (long)(short)LOWORD(lParam);
                hittest.pt.y = (long)(short)HIWORD(lParam);
                
                dstItem = TreeView_HitTest(prjTreeWindow, &hittest);
                if (dstItem && dstItem != srcItem)
                {
                    PROJECTITEM *srcData = GetItemInfo(srcItem);
                    data = GetItemInfo(dstItem);
                    if (srcData && data)
                    {
                        PROJECTITEM *p = data->parent;
                        while (p)
                            if (p == srcData)
                                break;
                            else
                                p = p->parent;
                        if (p)
                        {
                            if (inView)
                            {
                                inView = FALSE;
                                SetCursor(noCur);
                                TreeView_SelectDropTarget(prjTreeWindow, NULL);
                            }
                            break;
                        }
                    }
                    if (data && (data->type == PJ_PROJ || data->type == PJ_FOLDER))
                    {
                        if (!inView)
                        {
                            inView = TRUE;
                            SetCursor(dragCur);
                        }
                        TreeView_SelectDropTarget(prjTreeWindow, dstItem);
                    }
                    else
                    {
                        if (inView)
                        {
                            inView = FALSE;
                            SetCursor(noCur);
                            TreeView_SelectDropTarget(prjTreeWindow, NULL);
                        }
                    }
                }
                else
                {
                    if (inView)
                    {
                        inView = FALSE;
                        SetCursor(noCur);
                        TreeView_SelectDropTarget(prjTreeWindow, NULL);
                    }
                }
            }
            break;
        case WM_SETFOCUS:
            PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
            SetFocus(prjTreeWindow);
            break;
        case WM_CREATE:
            hwndProject = hwnd;
            GetClientRect(hwnd, &rs);

            treeViewSelected = 0;
            dragCur = LoadCursor(hInstance, "ID_DRAGCUR");
            noCur = LoadCursor(hInstance, "ID_NODRAGCUR");
            folderClose = LoadBitmap(hInstance, "ID_FOLDERCLOSE");
            folderOpen = LoadBitmap(hInstance, "ID_FOLDEROPEN");
            treeIml = ImageList_Create(16, 16, ILC_COLOR24, IL_IMAGECOUNT+2, 0);
            
            mainIml = LoadBitmap(hInstance, "ID_FILES");
            ChangeBitmapColor(mainIml, 0xffffff, RetrieveSysColor(COLOR_WINDOW));
            ImageList_Add(treeIml, mainIml, NULL);
            ilfolderClose = ImageList_Add(treeIml, folderClose, 0);
            ilfolderOpen = ImageList_Add(treeIml, folderOpen, 0);
            DeleteObject(folderClose);
            DeleteObject(folderOpen);
            DeleteObject(mainIml);
            prjTreeWindow = CreateWindowEx(0, WC_TREEVIEW, "", WS_VISIBLE |
                WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_TRACKSELECT,
                0, 0, rs.right, rs.bottom, hwnd, (HMENU)ID_TREEVIEW,
                hInstance, NULL);
            i = GetWindowLong(prjTreeWindow, GWL_STYLE);
            TreeView_SetImageList(prjTreeWindow, treeIml, TVSIL_NORMAL);
            lf = systemDialogFont;
            projFont = CreateFontIndirect(&lf);
            lf.lfItalic = TRUE;
            italicProjFont = CreateFontIndirect(&lf);
            lf.lfItalic = FALSE;
            lf.lfWeight = FW_BOLD;
            boldProjFont = CreateFontIndirect(&lf);
            SendMessage(prjTreeWindow, WM_SETFONT, (WPARAM)boldProjFont, 0);
            return 0;
        case WM_CLOSE:
            SaveAllProjects(workArea, FALSE);
            break;
        case WM_DESTROY:
            FreeSubTree(workArea, FALSE);
            DestroyWindow(prjTreeWindow);
            DeleteObject(projFont);
            DeleteObject(boldProjFont);
            DeleteObject(italicProjFont);
            DestroyCursor(dragCur);
            DestroyCursor(noCur);
            hwndProject = 0;
            break;
        case WM_SIZE:
            MoveWindow(prjTreeWindow, 0, 0, LOWORD(lParam), HIWORD(lParam), 0);
            break;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterProjectWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &ProjectProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szProjectClassName;
    RegisterClass(&wc);

    GetClassInfo(0, "edit", &wc);
    oldPrjEditProc = wc.lpfnWndProc;
    wc.lpfnWndProc = &prjEditWndProc;
    wc.lpszClassName = szprjEditClassName;
    wc.hInstance = hInstance;
    RegisterClass(&wc);

    InitializeCriticalSection(&projectMutex);
}

//-------------------------------------------------------------------------

void CreateProjectWindow(void)
{
    RECT rect;
    HWND parent;
    parent = hwndTabCtrl;
    GetTabRect(&rect);
    hwndProject = CreateWindow(szProjectClassName, szWorkAreaTitle,
        WS_VISIBLE | WS_CHILD, rect.left, rect.top, rect.right - rect.left,
        rect.bottom - rect.top, parent, 0, hInstance, 0);
}

