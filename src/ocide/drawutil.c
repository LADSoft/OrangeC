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
#include <process.h>
#include "helpid.h"
#include "header.h"
#include "regexp.h"
#include "rc.h"
#include "rcgui.h"
#include <ctype.h>

#ifndef CCHILDREN_TITLEBAR
#    define CCHILDREN_TITLEBAR 5
#endif

#ifndef STATE_SYSTEM_PRESSED
#    define STATE_SYSTEM_PRESSED 8
#endif
extern DWINFO* editWindows;
extern PROJECTITEM* workArea;
extern HINSTANCE hInstance;
extern HWND hwndFrame;
extern HANDLE ewSem;
extern LOGFONT systemDialogFont;
extern HWND hwndClient, hwndFrame;

struct saveData
{
    BOOL asProject;
    void* data;
};
static HBITMAP closenrml, closepress;

void InitDrawUtil(void)
{
    RECT r;
    HDC dc = GetWindowDC(hwndFrame);
    HDC memDC = CreateCompatibleDC(dc);
    HBITMAP hbmp = CreateCompatibleBitmap(dc, 20, 20);

    r.left = r.top = 0;
    r.right = r.bottom = 20;
    SelectObject(memDC, hbmp);
    DrawFrameControl(memDC, &r, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT);
    closenrml = SelectObject(memDC, hbmp);
    hbmp = CreateCompatibleBitmap(dc, 20, 20);
    DrawFrameControl(memDC, &r, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT | DFCS_PUSHED);
    closepress = SelectObject(memDC, hbmp);
    //    DeleteObject(hbmp);
    DeleteDC(memDC);
    ReleaseDC(hwndClient, dc);
}
int FileAttributes(char* name)
{
    int rv = GetFileAttributes(name);
    if (rv == -1)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            return 0;
        return -1;  // any other error, it is read only file
    }
    else
        return rv;
}

void GetFileList(HWND hwndLV, PROJECTITEM* pj, int* items, BOOL changed)
{
    if (pj->type == PJ_FILE)
    {
        int rv = FALSE;
        if (changed)
        {
            FILETIME time;
            if (!pj->resData)
            {
                DWINFO* ptr = GetFileInfo(pj->realName);
                if (ptr)
                {
                    int a = FileAttributes(ptr->dwName);
                    rv = FALSE;
                    if (a == -1)
                        a = 0;
                    if (FileTime(&time, ptr->dwName))
                    {
                        rv = (time.dwHighDateTime != ptr->time.dwHighDateTime || time.dwLowDateTime != ptr->time.dwLowDateTime);
                        ptr->time = time;
                    }
                    if (a & FILE_ATTRIBUTE_READONLY)
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
            DWINFO* ptr = GetFileInfo(pj->realName);
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
            struct saveData* sd = calloc(1, sizeof(struct saveData));
            if (sd)
            {
                DWINFO* ptr;
                sd->asProject = TRUE;
                sd->data = pj;
                memset(&item, 0, sizeof(item));
                item.iItem = (*items)++;
                item.iSubItem = 0;
                item.mask = LVIF_PARAM;
                item.lParam = (LPARAM)sd;
                item.pszText = "";  // LPSTR_TEXTCALLBACK ;
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
        PROJECTITEM* cur = pj->children;
        while (cur)
        {
            GetFileList(hwndLV, cur, items, changed);
            cur = cur->next;
        }
    }
}
void GetSecondaryFileList(HWND hwndLV, int* items, BOOL changed)
{
    DWINFO* ptr;
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
                if (a == -1)
                    a = 0;
                if (FileTime(&time, ptr->dwName))
                {
                    rv = (time.dwHighDateTime != ptr->time.dwHighDateTime || time.dwLowDateTime != ptr->time.dwLowDateTime);
                    ptr->time = time;
                }
                if (a & FILE_ATTRIBUTE_READONLY)
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
                struct saveData* sd = calloc(1, sizeof(struct saveData));
                if (sd)
                {
                    sd->asProject = FALSE;
                    sd->data = ptr;
                    memset(&item, 0, sizeof(item));
                    item.iItem = (*items)++;
                    item.iSubItem = 0;
                    item.mask = LVIF_PARAM;
                    item.lParam = (LPARAM)sd;
                    item.pszText = "";  // LPSTR_TEXTCALLBACK ;
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
    DWINFO* ptr;
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
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

static void SetOKText(HWND hwnd, char* text)
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
        PROJECTITEM* pj = NULL;
        DWINFO* ptr = NULL;
        struct saveData* sd = NULL;
        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM;
        if (!ListView_GetItem(hwndLV, &item))
            break;
        sd = (struct saveData*)item.lParam;
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
                    DWINFO* ptr = GetFileInfo(pj->realName);
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
                        SendMessage(ptr->self, WM_COMMAND, IDM_SAVE, 0);
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
    switch (draw->nmcd.dwDrawStage)
    {
        case CDDS_PREPAINT:
        case CDDS_ITEMPREPAINT:
            return CDRF_NOTIFYSUBITEMDRAW;
        case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
            if (draw->nmcd.uItemState & (CDIS_SELECTED))
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

long APIENTRY FileSaveProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
                    LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
                    struct saveData* sd;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_STATE;
                    switch (plvdi->item.iSubItem)
                    {
                        case 2:
                            sd = (struct saveData*)plvdi->item.lParam;
                            if (sd->asProject)
                            {
                                PROJECTITEM* pj = sd->data;
                                plvdi->item.pszText = pj->displayName;
                            }
                            else
                            {
                                DWINFO* ptr = sd->data;
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
            switch (wParam & 0xffff)
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

long APIENTRY FileChangeProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
                    LV_DISPINFO* plvdi = (LV_DISPINFO*)lParam;
                    struct saveData* sd;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_STATE;
                    switch (plvdi->item.iSubItem)
                    {
                        case 2:
                            sd = (struct saveData*)plvdi->item.lParam;
                            if (sd->asProject)
                            {
                                PROJECTITEM* pj = sd->data;
                                plvdi->item.pszText = pj->displayName;
                            }
                            else
                            {
                                DWINFO* ptr = sd->data;
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
            switch (wParam & 0xffff)
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

void CheckEditWindowChangedThread(void* aa)
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

void CheckEditWindowChanged(void) { _beginthread((BEGINTHREAD_FUNC)CheckEditWindowChangedThread, 0, NULL); }

//-------------------------------------------------------------------------

int QuerySaveAll(void) { return DialogBox(hInstance, "DLG_FILESAVE", hwndFrame, (DLGPROC)FileSaveProc); }

int PaintMDITitleBar(HWND hwnd, int iMessage, WPARAM wParam, LPARAM lParam)
{
#define WM_GETTITLEBARINFOEX 0x33f
    typedef struct tagTITLEBARINFOEX
    {
        DWORD cbSize;
        RECT rcTitleBar;
        DWORD rgstate[CCHILDREN_TITLEBAR + 1];
        RECT rgrect[CCHILDREN_TITLEBAR + 1];
    } TITLEBARINFOEX, *PTITLEBARINFOEX, *LPTITLEBARINFOEX;
    TITLEBARINFOEX tbi;
    char buf[256];
    SIZE sz;
    int rv = 0;
    HDC hdc;
    RECT r, rclient;
    POINT pt;
    int n;
    HBRUSH brush = GetStockObject(NULL_BRUSH);
    HPEN pen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
    HPEN pen2 =
        CreatePen(PS_SOLID, 0,
                  GetSysColor((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) == hwnd ? COLOR_GRADIENTACTIVECAPTION
                                                                                           : COLOR_GRADIENTINACTIVECAPTION));
    HPEN pen3;
    HDC hdcMem, hdcDraw;
    HBITMAP bitmap;
    HFONT xfont;
    n = systemDialogFont.lfHeight;
    systemDialogFont.lfHeight = -16;
    xfont = CreateFontIndirect(&systemDialogFont);
    systemDialogFont.lfHeight = n;
    hdc = GetWindowDC(hwnd);
    memset(&tbi, 0, sizeof(tbi));
    tbi.cbSize = sizeof(tbi);
    SendMessage(hwnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)&tbi);
    MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&tbi.rgrect[5], 2);
    pt.x = pt.y = n = GetSystemMetrics(SM_CXDLGFRAME) * 2;  //+ GetSystemMetrics(SM_CXBORDER);
    pen3 = CreatePen(PS_SOLID, pt.y,
                     GetSysColor((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) == hwnd ? COLOR_GRADIENTACTIVECAPTION
                                                                                              : COLOR_GRADIENTINACTIVECAPTION));
    GetClientRect(hwnd, &rclient);
    MapWindowPoints(hwnd, HWND_DESKTOP, (LPPOINT)&rclient, 2);
    GetWindowRect(hwnd, &r);
    rclient.top -= r.top;
    rclient.left -= r.left;
    rclient.bottom -= r.top;
    rclient.right -= r.left;
    r.right -= r.left;
    r.left -= r.left;
    r.bottom -= r.top;
    r.top -= r.top;
    brush = SelectObject(hdc, brush);
    pen = SelectObject(hdc, pen);
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);
    pen = SelectObject(hdc, pen2);
    Rectangle(hdc, r.left + 1, r.top + 1, r.right - 1, r.bottom - 1);
    pen2 = SelectObject(hdc, pen3);
    n = 4;
    Rectangle(hdc, r.left + 1 + n, r.top + 1 + n + GetSystemMetrics(SM_CYSMCAPTION), r.right - 4, r.bottom - 4);
    //    rv = DefMDIChildProc(hwnd, iMessage, wParam, lParam);
    pen3 = SelectObject(hdc, pen);
    Rectangle(hdc, rclient.left - 1, rclient.top - 1, rclient.right + 1, rclient.bottom + 1);
    r.top += 2;
    r.bottom = rclient.top;
    r.left += 2;
    r.right -= 2;
    hdcDraw = CreateCompatibleDC(hdc);
    bitmap = CreateCompatibleBitmap(hdc, r.right - r.left + 4, rclient.top);
    SelectObject(hdcDraw, bitmap);
    FillRect(hdcDraw, &r,
             (HBRUSH)(((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) == hwnd ? COLOR_GRADIENTACTIVECAPTION
                                                                                    : COLOR_GRADIENTINACTIVECAPTION) +
                      1));
    r.bottom = r.top + GetSystemMetrics(SM_CYSMCAPTION);
    GradientFillCaption(hdcDraw, &r, (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) == hwnd);  // undefined in local context
    hdcMem = CreateCompatibleDC(hdc);

    if (tbi.rgstate[5] == STATE_SYSTEM_PRESSED)
        SelectObject(hdcMem, closepress);
    else
        SelectObject(hdcMem, closenrml);
    BitBlt(hdcDraw, tbi.rgrect[5].left + rclient.left + 2, tbi.rgrect[5].top + rclient.top + 2, 20, 20, hdcMem, 0, 0, SRCCOPY);
    DeleteDC(hdcMem);
    GetWindowText(hwnd, buf, sizeof(buf));
    SelectObject(hdcDraw, xfont);
    GetTextExtentPoint32(hdcDraw, buf, strlen(buf), &sz);
    n = (r.right + r.left + 4 - sz.cx) / 2;
    SetBkMode(hdcDraw, TRANSPARENT);
    TextOut(hdcDraw, n, 4, buf, strlen(buf));
    SelectObject(hdcDraw, xfont);

    BitBlt(hdc, r.left, r.top, r.right - r.left, rclient.top - r.top - 1, hdcDraw, r.left, r.top, SRCCOPY);

    brush = SelectObject(hdc, brush);
    pen = SelectObject(hdc, pen);
    DeleteObject(xfont);
    DeleteObject(bitmap);
    DeleteObject(pen3);
    DeleteObject(pen2);
    DeleteObject(pen);
    DeleteDC(hdcDraw);
    ReleaseDC(hwnd, hdc);
    return rv;
}
static RECT ResFSRect;
void PopupResFullScreen(HWND hwnd)
{
    if (!(GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP))
    {
        GetWindowRect(hwnd, &ResFSRect);
        MapWindowPoints(HWND_DESKTOP, hwndClient, (LPPOINT)&ResFSRect, 2);
        RECT wrect;
        GetEditPopupFrame(&wrect);
        SetParent(hwnd, HWND_DESKTOP);
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_POPUP);
        SetWindowPos(hwnd, HWND_TOP, wrect.left, wrect.top, wrect.right - wrect.left, wrect.bottom - wrect.top, SWP_SHOWWINDOW);
    }
}
void ReleaseResFullScreen(HWND hwnd)
{
    if (GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP)
    {
        SetParent(hwnd, hwndClient);
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_POPUP);
        SetWindowPos(hwnd, 0, ResFSRect.left, ResFSRect.top, ResFSRect.right - ResFSRect.left, ResFSRect.bottom - ResFSRect.top,
                     SWP_SHOWWINDOW);
    }
}