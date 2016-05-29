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
#include <process.h>
#include "helpid.h"
#include "header.h"
#include "codecomp.h"
#include "regexp.h"
#include "rc.h"
#include "rcgui.h"
#include <ctype.h>

extern DWINFO *editWindows;
extern PROJECTITEM *workArea;
extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern HANDLE ewSem;

struct saveData
{
    BOOL asProject;
    void *data;
} ;
int FileAttributes(char *name)
{
    int rv = GetFileAttributes(name);
    if (rv ==  - 1)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            return 0;
        return  - 1; // any other error, it is read only file
    }
    else
        return rv;
}

void GetFileList(HWND hwndLV, PROJECTITEM *pj, int *items, BOOL changed)
{
    if (pj->type == PJ_FILE)
    {
        int rv = FALSE;
        if (changed)
        {
            FILETIME time;
            if (!pj->resData)
            {
                DWINFO *ptr = GetFileInfo(pj->realName);
                if (ptr)
                {
                    int a = FileAttributes(ptr->dwName);
                    rv = FALSE;
                    if (a ==  - 1)
                        a = 0;
                    if (FileTime(&time, ptr->dwName))
                    {
                        rv = (time.dwHighDateTime != ptr->time.dwHighDateTime ||
                            time.dwLowDateTime != ptr->time.dwLowDateTime);
                        ptr->time = time;
                    }
                    if (a &FILE_ATTRIBUTE_READONLY)
                        SendMessage(ptr->dwHandle, EM_SETREADONLY, 1, 0);
                    else
                        SendMessage(ptr->dwHandle, EM_SETREADONLY, 0, 0);
                }
            }
            else
            {
                FILETIME time;
                if (FileTime(&time, pj->realName))
                {
                    rv = (time.dwHighDateTime != pj->resData->time.dwHighDateTime ||
                        time.dwLowDateTime != pj->resData->time.dwLowDateTime);
                    pj->resData->time = time;
                }
            }
        }
        else if (!pj->resData)
        {
            DWINFO *ptr = GetFileInfo(pj->realName);
            if (ptr)
                rv = SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0);
        }
        else 
        {
            rv = ResCheckChanged(pj->resData->resources->resources);
        }
        if (rv)
        {
            int v;
            LV_ITEM item;
            struct saveData *sd = calloc(1, sizeof(struct saveData));
            if (sd)
            {
                DWINFO *ptr;
                sd->asProject = TRUE;
                sd->data = pj;
                memset(&item, 0, sizeof(item));
                item.iItem = (*items)++;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM;
                item.lParam = (LPARAM)sd;
                item.pszText = ""; // LPSTR_TEXTCALLBACK ;
                v = ListView_InsertItem(hwndLV, &item);
                ListView_SetCheckState(hwndLV, v, TRUE);
                ptr = GetFileInfo(pj->realName);
                if (ptr)
                    ptr->inSaveDialog = TRUE;
            }
        }
    }
    else
    {
        PROJECTITEM *cur = pj->children;
        while (cur)
        {
            GetFileList(hwndLV, cur, items, changed);
            cur = cur->next;
        }
    }
}
void GetSecondaryFileList(HWND hwndLV, int *items, BOOL changed)
{
    DWINFO *ptr;
    MSG msg;
    MsgWait(ewSem, INFINITE);
    ptr = editWindows;
    while (ptr)
    {
        if (ptr->active)
            ptr->deferClose = TRUE;
        ptr = ptr->next;
    }
    SetEvent(ewSem);
    ptr = editWindows;
    while (ptr)
    {
        if (ptr->active && !ptr->inSaveDialog)
        {
            int rv = FALSE;
            if (changed)
            {
                FILETIME time;
                int a = FileAttributes(ptr->dwName);
                rv = FALSE;
                if (a ==  - 1)
                    a = 0;
                if (FileTime(&time, ptr->dwName))
                {
                    rv = (time.dwHighDateTime != ptr->time.dwHighDateTime ||
                        time.dwLowDateTime != ptr->time.dwLowDateTime);
                    ptr->time = time;
                }
                if (a &FILE_ATTRIBUTE_READONLY)
                    SendMessage(ptr->dwHandle, EM_SETREADONLY, 1, 0);
                else
                    SendMessage(ptr->dwHandle, EM_SETREADONLY, 0, 0);
            }
            else
            {
                rv = SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0);
            }
            if (rv)
            {
                int v;
                LV_ITEM item;
                struct saveData *sd = calloc(1, sizeof(struct saveData));
                if (sd)
                {
                    sd->asProject = FALSE;
                    sd->data = ptr;
                    memset(&item, 0, sizeof(item));
                    item.iItem = (*items)++;
                    item.iSubItem = 0;
                    item.mask = LVIF_PARAM;
                    item.lParam = (LPARAM)sd;
                    item.pszText = ""; // LPSTR_TEXTCALLBACK ;
                    v = ListView_InsertItem(hwndLV, &item);
                    ListView_SetCheckState(hwndLV, v, TRUE);
                }
            }
        }
        ptr = ptr->next;
    }
    ptr = editWindows;
    while (ptr)
    {
        HWND xx = ptr->self;
		BOOL active = ptr->active;
        ptr = ptr->next;
        if (active)
            PostMessage(xx, WM_DEFERREDCLOSE, 0, 0);
    }
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        ProcessMessage(&msg);
        if (msg.message == WM_QUIT)
            break;
    }
        
}
//-------------------------------------------------------------------------

static int CreateFileSaveData(HWND hwnd, int changed)
{
    int items = 0;
    DWINFO *ptr;
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM ;
    lvC.cx = 20;
    lvC.iSubItem = 0;
    ListView_InsertColumn(hwndLV, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = 32;
    lvC.iSubItem = 1;
    ListView_InsertColumn(hwndLV, 1, &lvC);
    lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
    lvC.fmt = LVCFMT_LEFT;
    lvC.cx = r.right - r.left - 56;
    lvC.iSubItem = 2;
    ListView_InsertColumn(hwndLV, 2, &lvC);

    MsgWait(ewSem, INFINITE);
    ptr = editWindows;
    while (ptr)
    {
        if (ptr->active)
            ptr->inSaveDialog = FALSE;
        ptr = ptr->next;
    }
    SetEvent(ewSem);
    if (workArea)
        GetFileList(hwndLV, workArea, &items, changed);
    GetSecondaryFileList(hwndLV, &items, changed);
    
    if (items)
    {
        ListView_SetSelectionMark(hwndLV, 0);
        ListView_SetItemState(hwndLV, 0, LVIS_SELECTED, LVIS_SELECTED);
    }

    return items;
}

static void SetOKText(HWND hwnd, char *text)
{
    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
    HWND okbut = GetDlgItem(hwnd, IDOK);
    int i;
    for (i = 0;; i++)
    {
        LV_ITEM item;
        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        if (!ListView_GetItem(hwndLV, &item))
            break;
        if (ListView_GetCheckState(hwndLV, i))
        {
            SetWindowText(okbut, text);
            return;
        }
    }
    SetWindowText(okbut, "Ok");
}
//-------------------------------------------------------------------------

static void ParseFileSaveData(HWND hwnd, BOOL changed)
{
    LV_ITEM item;
    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
    int i;
    for (i = 0;; i++)
    {
        PROJECTITEM *pj = NULL;
        DWINFO *ptr = NULL;
        struct saveData *sd = NULL;
        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        if (!ListView_GetItem(hwndLV, &item))
            break;
        sd = (struct saveData *)item.lParam;
        if (sd->asProject)
            pj = sd->data;
        else
            ptr = sd->data;
        if (changed)
        {
            if (ListView_GetCheckState(hwndLV, i))
            {				
                if (ptr)
                {
                    LoadFile(ptr->self, ptr, TRUE);
                }		
                else if (pj->type == PJ_FILE)
                {
                    DWINFO *ptr = GetFileInfo(pj->realName);
                    if (ptr)
                        LoadFile(ptr->self, ptr, TRUE);
                }
                else
                {
                    ResReload(pj);
                }
            }
        }
        else
        {
            if ((pj && !pj->resData) || !pj)
            {
                if (!ptr)
                    ptr = GetFileInfo(pj->realName);
                if (ptr)
                {
                    if (ListView_GetCheckState(hwndLV, i))
                        SendMessage(ptr->self, WM_COMMAND, IDM_SAVE,
                            0);
                    else
                    {
                        TagLinesAdjust(ptr->dwName, TAGM_DISCARDFILE);
                    }
                }
            }
            else 
            {
                if (ListView_GetCheckState(hwndLV, i))
                    ResSave(pj->realName, pj->resData);
                else
                    ResFree(pj, FALSE);
            }
        }
        free(sd);
    }
}

static int CustomDraw(HWND hwnd, LPNMLVCUSTOMDRAW draw)
{
    switch(draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT :
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            if (draw->nmcd.uItemState & (CDIS_SELECTED ))
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

long APIENTRY FileSaveProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
           if (!CreateFileSaveData(hwnd, FALSE))
           {
                EndDialog(hwnd, 1);
           }
            else
                CenterWindow(hwnd);
            return 1;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                SetWindowLong(hwnd, DWL_MSGRESULT, CustomDraw(hwnd, (LPNMLVCUSTOMDRAW)lParam));
                return TRUE;
            }
            else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
            {
                switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                {
                    case VK_INSERT:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                            ListView_SetCheckState(hwndLV, -1, TRUE);
                            SetOKText(hwnd, "Save");
                        }
                        else
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                            int i = ListView_GetSelectionMark(hwndLV);
                            ListView_SetCheckState(hwndLV, i, TRUE);
                        }
                        break;
                    case VK_DELETE:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                            ListView_SetCheckState(hwndLV, -1, FALSE);
                            SetOKText(hwnd, "Save");
                        }
                        else
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                            int i = ListView_GetSelectionMark(hwndLV);
                            ListView_SetCheckState(hwndLV, i, FALSE);
                        }
                        break;
                }
            }
            if (wParam == IDC_FILELIST)
            {
                if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO)
                {
                    LV_DISPINFO *plvdi = (LV_DISPINFO*)lParam;
                    struct saveData *sd;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_STATE;
                    switch (plvdi->item.iSubItem)
                    {
                    case 2:
                        sd = (struct saveData *)plvdi->item.lParam;
                        if (sd->asProject)
                        {
                            PROJECTITEM *pj = sd->data;
                            plvdi->item.pszText = pj->displayName;
                        }
                        else
                        {
                            DWINFO *ptr = sd->data;
                            plvdi->item.pszText = ptr->dwTitle;
                        }
                        break;
                    default:
                        plvdi->item.pszText = "";
                        break;
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED)
                {
                    SetOKText(hwnd, "Save");
                }
            }
            return 0;
        case WM_COMMAND:
            switch (wParam &0xffff)
            {
            case IDOK:
                ParseFileSaveData(hwnd, FALSE);
                EndDialog(hwnd, IDOK);
                break;
            case IDCANCEL:
                EndDialog(hwnd, IDCANCEL);
                break;
            case IDC_SELECTALL:
                {
                    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                    ListView_SetCheckState(hwndLV, -1, TRUE);
                    SetOKText(hwnd, "Save");
                }
                break;
            case IDC_DESELECTALL:
                {
                    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                    ListView_SetCheckState(hwndLV, -1, FALSE);
                    SetOKText(hwnd, "Save");
                }
                break;
            case IDHELP:
                ContextHelp(IDH_SAVE_FILE_DIALOG);
                break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

long APIENTRY FileChangeProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            if (!CreateFileSaveData(hwnd, TRUE))
                EndDialog(hwnd, 1);
            else
                CenterWindow(hwnd);
            return 1;
        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW)
            {
                SetWindowLong(hwnd, DWL_MSGRESULT, CustomDraw(hwnd, (LPNMLVCUSTOMDRAW)lParam));
                return TRUE;
            }
            else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
            {
                switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                {
                    case VK_INSERT:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                            ListView_SetCheckState(hwndLV, -1, TRUE);
                            SetOKText(hwnd, "Reload");
                        }
                        else
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                            int i = ListView_GetSelectionMark(hwndLV);
                            ListView_SetCheckState(hwndLV, i, TRUE);
                        }
                        break;
                    case VK_DELETE:
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                            ListView_SetCheckState(hwndLV, -1, FALSE);
                            SetOKText(hwnd, "Reload");
                        }
                        else
                        {
                            HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                            int i = ListView_GetSelectionMark(hwndLV);
                            ListView_SetCheckState(hwndLV, i, FALSE);
                        }
                        break;
                }
            }
            if (wParam == IDC_FILELIST)
            {
                if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO)
                {
                    LV_DISPINFO *plvdi = (LV_DISPINFO*)lParam;
                    struct saveData *sd;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_STATE;
                    switch (plvdi->item.iSubItem)
                    {
                    case 2:
                        sd = (struct saveData *)plvdi->item.lParam;
                        if (sd->asProject)
                        {
                            PROJECTITEM *pj = sd->data;
                            plvdi->item.pszText = pj->displayName;
                        }
                        else
                        {
                            DWINFO *ptr = sd->data;
                            plvdi->item.pszText = ptr->dwTitle;
                        }
                        break;
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED)
                {            
                    SetOKText(hwnd, "Reload");
                }
            }
            break;
        case WM_COMMAND:
            switch (wParam &0xffff)
            {
            case IDOK:
                ParseFileSaveData(hwnd, TRUE);
                EndDialog(hwnd, IDOK);
                break;
            case IDCANCEL:
                EndDialog(hwnd, IDCANCEL);
                break;
            case IDC_SELECTALL:
                {
                    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                    ListView_SetCheckState(hwndLV, -1, TRUE);
                    SetOKText(hwnd, "Reload");
                }
                break;
            case IDC_DESELECTALL:
                {
                    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
                    ListView_SetCheckState(hwndLV, -1, FALSE);
                    SetOKText(hwnd, "Reload");
                }
                break;
            case IDHELP:
                ContextHelp(IDH_RELOAD_FILE_DIALOG);
                break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

void CheckEditWindowChangedThread(void *aa)
{
    static int sem;
    if (!sem)
    {
        sem++;
        DialogBox(hInstance, "DLG_FILECHANGE", hwndFrame, (DLGPROC)FileChangeProc);
        sem--;
    }
}

//-------------------------------------------------------------------------

void CheckEditWindowChanged(void)
{
    DWORD threadhand;
    _beginthread((BEGINTHREAD_FUNC)CheckEditWindowChangedThread, 0, NULL);
}

//-------------------------------------------------------------------------

int QuerySaveAll(void)
{
    return DialogBox(hInstance, "DLG_FILESAVE", hwndFrame, (DLGPROC)FileSaveProc);
}

