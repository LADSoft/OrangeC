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

#include "header.h"
#include "operands.h"
#include "opcodes.h"
#include <ctype.h>

#define ERR_ERRORS 1
#define ERR_WARNINGS 2
#define BUTTONWIDTH 95
#define BUTTONHEIGHT 26
extern HINSTANCE hInstance;
extern HWND hwndClient, hwndFrame;
extern LOGFONT systemDialogFont;

char* varinfohist[MAX_COMBO_HISTORY];

static char brsName[256];
static HWND hwndBrowse;
static char szBrowseClassName[] = "xccBrowseClass";
static HWND hwndLV, hwndErrButton, hwndWarnButton, hwndBackground;
static int curSel;
static HWND hwndButton, hwndCombo, hwndEdit;
static WNDPROC oldproc;
static char* szBrowseTitle = "Variable Usages";
static BOOL displayFull = TRUE;
static HIMAGELIST tagImageList;
static HBITMAP buttonbmp;
typedef struct
{
    char file[MAX_PATH];
    char name[512];
    int line;
    int definition : 1;
    int declaration : 1;
} BROWSEWNDDATA;
static BROWSEWNDDATA** browselist;
static int browsecount;
static int browsemax;
static void CopyText(HWND hwnd)
{
    int i;
    char* p = calloc(browsecount, MAX_PATH + 20);
    int sz = 1;
    int n = browsecount;
    if (p)
    {
        for (i = 0; i < n && i < browsecount; i++)
        {
            char buf[1000];
            sprintf(buf, "%d: %s\t%d\t%s\n", i + 1, browselist[i]->name, browselist[i]->line, browselist[i]->file);
            strcat(p, buf);
        }
        TextToClipBoard(hwnd, p);
        free(p);
    }
}
static void FreeUsageList(void)
{
    int i;
    for (i = 0; i < browsecount; i++)
        free(browselist[i]);
    free(browselist);
    browsecount = browsemax = 0;
    browselist = NULL;
}
static void InsertUsage(char* name, char* file, int line, BOOL definition, BOOL declaration)
{
    char unmangled[2048];
    BROWSEWNDDATA* newData = calloc(1, sizeof(BROWSEWNDDATA));
    unmangle(unmangled, name);
    newData->declaration = declaration;
    newData->definition = definition;
    strcpy(newData->file, file);
    newData->line = line;
    strcpy(newData->name, unmangled);
    if (browsecount >= browsemax)
    {
        if (!browsecount)
            browsemax = 10;
        else
            browsemax = 2 * browsemax;
        browselist = realloc(browselist, browsemax * sizeof(BROWSEWNDDATA*));
    }
    browselist[browsecount++] = newData;
}
static BOOL LookupLineInfo(sqlite3* db, BROWSELIST* bl)
{
    int qual = -1;
    // this query to attempt to find it in the current file
    static char* query = {
        "SELECT FileNames.Name, LineNumbers.Startline, LineNumbers.qual FROM LineNumbers "
        "    JOIN FileNames ON FileNames.id = LineNumbers.fileId "
        "        AND LineNumbers.symbolId == ?"};
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        while (bl)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_int(handle, 1, bl->id);
            while (!done)
            {
                switch (rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        done = TRUE;
                        break;
                    case SQLITE_ROW:
                    {
                        char* file = (char*)sqlite3_column_text(handle, 0);
                        int newStartLine = sqlite3_column_int(handle, 1);
                        int newQual = sqlite3_column_int(handle, 2);
                        InsertUsage(bl->name, file, newStartLine, !(newQual & 2), !!(newQual & 2));
                    }
                    break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            bl = bl->next;
        }
        sqlite3_finalize(handle);
    }
    return qual != -1;
}
static BOOL LookupUsageInfo(sqlite3* db, BROWSELIST* bl)
{
    int qual = -1;
    // this query to attempt to find it in the current file
    static char* query = {
        "SELECT FileNames.Name, Usages.Startline FROM Usages "
        "    JOIN FileNames ON FileNames.id = Usages.fileId "
        "        AND Usages.symbolId == ?"};
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        while (bl)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_int(handle, 1, bl->id);
            while (!done)
            {
                switch (rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        done = TRUE;
                        break;
                    case SQLITE_ROW:
                    {
                        char* file = (char*)sqlite3_column_text(handle, 0);
                        int newStartLine = sqlite3_column_int(handle, 1);
                        InsertUsage(bl->name, file, newStartLine, FALSE, FALSE);
                    }
                    break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            bl = bl->next;
        }
        sqlite3_finalize(handle);
    }
    return qual != -1;
}
static void CreateUsageList(void)
{
    HWND hwnd = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
    PROJECTITEM* pj;
    sqlite3* db = BrowseOpenDBByHWND(hwnd, &pj);
    if (db)
    {
        char mangled[2048], *srcName = brsName;
        BROWSELIST* rv = NULL;
        FreeUsageList();

        GetQualifiedName(mangled, &srcName, FALSE, FALSE);

        if (!strrchr(mangled + 1, '@'))
        {
            int id;
            mangled[0] = '_';
            if ((id = LookupSymbolBrowse(db, mangled)))
            {
                BROWSELIST* next = calloc(sizeof(BROWSELIST), 1);
                next->next = rv;
                rv = next;
                strcpy(next->name, mangled + 1);
                next->id = id;
            }
            rv = LookupCPPNamesBrowse(db, mangled, rv);
            mangled[0] = '@';
        }
        rv = LookupCPPNamesBrowse(db, mangled, rv);
        LookupLineInfo(db, rv);
        LookupUsageInfo(db, rv);
        while (rv)
        {
            BROWSELIST* next = rv->next;
            free(rv);
            rv = next;
        }
        PostMessage(hwndBrowse, WM_USER, 0, 0);
        DBClose(db);  // undefined in local context
    }
}
//-------------------------------------------------------------------------

static LRESULT CALLBACK EditHook(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_LBUTTONDOWN:
            SetFocus(hwnd);
            break;
        case WM_SYSCHAR:
        case WM_SYSDEADCHAR:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            // I don't know why I have to do this.   MEnu access keys don't work
            // unless I do though.
            HWND hwnd1 = GetParent(hwnd);
            while (hwnd1 != HWND_DESKTOP)
            {
                hwnd = hwnd1;
                hwnd1 = GetParent(hwnd);
            }
            return SendMessage(hwnd, iMessage, wParam, lParam);
        }
        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_RETURN:
                    if (GetKeyState(VK_SHIFT) & 0x80000000)
                        return SendMessage(hwndBrowse, iMessage, wParam, lParam);
                    else
                        return SendMessage(hwndBrowse, WM_COMMAND, IDC_RETURN, 0);
                case VK_ESCAPE:
                    if (GetKeyState(VK_SHIFT) & 0x80000000)
                        return SendMessage(hwndBrowse, iMessage, wParam, lParam);
                    else
                        return SendMessage(hwndBrowse, WM_COMMAND, IDC_ESCAPE, 0);
                case VK_LEFT:
                case VK_RIGHT:
                case VK_UP:
                case VK_DOWN:
                    if (GetKeyState(VK_SHIFT) & 0x80000000)
                        return SendMessage(hwndBrowse, iMessage, wParam, lParam);
                    break;
                case VK_NEXT:
                case VK_PRIOR:
                    return SendMessage(hwndBrowse, iMessage, wParam, lParam);
            }
            break;
        case WM_KEYUP:
        case WM_CHAR:
            if (wParam == VK_RETURN)
                return 0;
            if (wParam == VK_TAB)
                return 0;
            break;
    }
    return CallWindowProc(oldproc, hwnd, iMessage, wParam, lParam);
}
LRESULT CALLBACK BrowseProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HFONT font;
    LV_ITEM item;
    POINT pt;
    LV_COLUMN lvC;
    RECT r;
    int i;
    LPNMHDR nmh;
    static LVITEM pressed;
    switch (iMessage)
    {
        case WM_USER + 1:
        {
            DWINFO info;
            char* q;
            memset(&info, 0, sizeof(info));
            strcpy(info.dwName, browselist[pressed.lParam]->file);
            q = strrchr(info.dwName, '\\');
            if (q)
                strcpy(info.dwTitle, q + 1);
            else
                strcpy(info.dwTitle, info.dwName);
            info.dwLineNo = TagNewLine(browselist[pressed.lParam]->file, browselist[pressed.lParam]->line);
            info.logMRU = FALSE;
            info.newFile = FALSE;
            CreateDrawWindow(&info, TRUE);
            break;
        }
        case WM_CTLCOLORSTATIC:
        {
            return (LRESULT)(HBRUSH)(COLOR_INACTIVECAPTION + 1);
        }
        case WM_CTLCOLORBTN:
        {
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }

        case WM_NOTIFY:
            nmh = (LPNMHDR)lParam;
            if (nmh->code == NM_SETFOCUS)
            {
                PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
                SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            }
            else if (nmh->code == LVN_GETDISPINFO)
            {
                LV_DISPINFO* p = (LV_DISPINFO*)lParam;
                char name[512] = "", *q;
                switch (p->item.iSubItem)
                {
                    case 1: /* id*/
                        sprintf(name, "%d", p->item.lParam + 1);
                        break;
                    case 2: /*name*/
                        strcpy(name, browselist[p->item.lParam]->name);
                        break;
                    case 3: /* browse */
                        sprintf(name, "%d", browselist[p->item.lParam]->line);
                        break;
                    case 4: /*file*/
                        q = strrchr(browselist[p->item.lParam]->file, '\\');
                        if (!q)
                            q = browselist[p->item.lParam]->file;
                        else
                            q++;
                        strcpy(name, q);
                        break;
                }
                if (name[0])
                {
                    p->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    p->item.mask &= ~LVIF_STATE;
                    p->item.pszText = name;
                }
            }
            else if (((LPNMHDR)lParam)->code == NM_DBLCLK)
            {
                LVHITTESTINFO hittest;
                GetCursorPos(&hittest.pt);
                ScreenToClient(hwndLV, &hittest.pt);
                if (ListView_SubItemHitTest(hwndLV, &hittest) >= 0)
                {
                    LVITEM lvitem;
                    lvitem.iItem = hittest.iItem;
                    lvitem.iSubItem = 0;
                    lvitem.mask = LVIF_PARAM;
                    ListView_GetItem(hwndLV, &lvitem);
                    memcpy(&pressed, &lvitem, sizeof(pressed));
                    SendMessage(hwnd, WM_USER + 1, 0, 0);
                }
            }
            else if (nmh->code == LVN_KEYDOWN)
            {
                switch (((LPNMLVKEYDOWN)lParam)->wVKey)
                {
                    case 'C':
                        if (GetKeyState(VK_CONTROL) & 0x80000000)
                        {
                            CopyText(hwnd);
                        }
                        break;
                    case VK_UP:
                        if (curSel > 0)
                            SendMessage(hwnd, WM_USER, curSel - 1, 0);
                        break;
                    case VK_DOWN:
                        if (curSel < ListView_GetItemCount(hwndLV) - 1)
                            SendMessage(hwnd, WM_USER, curSel + 1, 0);
                        break;
                }
            }
            break;
        case WM_COMMAND:
            switch (wParam)
            {
                case IDC_RETURN:
                    GetWindowText(hwndEdit, brsName, sizeof(brsName));
                    SendMessage(hwndCombo, WM_SAVEHISTORY, 0, 0);
                    CreateUsageList();
                    return 0;
                case IDC_ESCAPE:
                    SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)brsName);
                    return 0;
                default:
                    if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == 300)
                    {
                        displayFull = !displayFull;
                        PostMessage(hwnd, WM_USER, 0, 0);
                    }
                    if (HIWORD(wParam) == CBN_SELENDOK && LOWORD(wParam) == 500)
                    {
                        PostMessage(hwnd, WM_COMMAND, IDC_RETURN, 0);
                    }
                    break;
            }
            break;

        case WM_USER:
        {
            int k = 0;
            char buf[40];
            sprintf(buf, "%d Usages", browsecount);
            SetWindowText(hwndButton, buf);

            ListView_DeleteAllItems(hwndLV);
            memset(&item, 0, sizeof(item));
            for (i = 0; i < browsecount; i++)
            {
                if (displayFull || browselist[i]->definition || browselist[i]->declaration)
                {
                    item.iItem = k++;
                    item.iSubItem = 0;
                    item.mask = LVIF_IMAGE | LVIF_PARAM;
                    item.lParam = i;
                    item.iImage = browselist[i]->definition ? 0 : browselist[i]->declaration ? 1 : 2;
                    ListView_InsertItem(hwndLV, &item);
                }
            }
        }
        break;
        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT pDis = (LPDRAWITEMSTRUCT)lParam;
            HDC memdc;
            char staticText[256];
            BOOL state = !!displayFull;
            int len = SendMessage(pDis->hwndItem, WM_GETTEXT, sizeof(staticText), (LPARAM)staticText);
            SIZE sz;
            RECT r;
            POINT textpos, iconpos;
            GetClientRect(pDis->hwndItem, &r);
            GetTextExtentPoint32(pDis->hDC, staticText, len, &sz);
            SetBkMode(pDis->hDC, TRANSPARENT);
            iconpos.x = 4;
            iconpos.y = (r.bottom - r.top - 16) / 2;
            textpos.x = r.right - 4 - sz.cx;
            textpos.y = (r.bottom - r.top - sz.cy) / 2;
            DrawFrameControl(pDis->hDC, &pDis->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH | (state ? DFCS_PUSHED : 0));
            TextOut(pDis->hDC, pDis->rcItem.left + textpos.x, pDis->rcItem.top + textpos.y, staticText, len);
            memdc = CreateCompatibleDC(pDis->hDC);
            SelectObject(memdc, buttonbmp);
            TransparentBlt(pDis->hDC, pDis->rcItem.left + iconpos.x, pDis->rcItem.top + iconpos.y, 16, 16, memdc, 0, 0, 16, 16,
                           0xc0c0c0);
            DeleteDC(memdc);
        }
            return 0;
        case WM_CREATE:
            hwndBrowse = hwnd;
            GetClientRect(hwnd, &r);
            hwndBackground = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, r.right - r.bottom,
                                          BUTTONHEIGHT + 4, hwnd, 0, hInstance, 0);
            hwndButton = CreateWindow("button", "0 Usages", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 2, 2, BUTTONWIDTH, BUTTONHEIGHT,
                                      hwnd, (HMENU)300, hInstance, 0);
            ApplyDialogFont(hwndButton);
            hwndCombo =
                CreateWindow("COMBOBOX", "", WS_CHILD + WS_CLIPSIBLINGS + WS_BORDER + WS_VISIBLE + CBS_DROPDOWN + CBS_AUTOHSCROLL,
                             BUTTONWIDTH + 10, 4, 200, 100, hwnd, (HMENU)500, hInstance, 0);
            ApplyDialogFont(hwndCombo);
            SubClassHistoryCombo(hwndCombo);
            SendMessage(hwndCombo, WM_SETHISTORY, 0, (LPARAM)varinfohist);
            pt.x = pt.y = 5;
            hwndEdit = ChildWindowFromPoint(hwndCombo, pt);
            oldproc = (WNDPROC)SetWindowLong(hwndEdit, GWL_WNDPROC, (int)EditHook);
            BringWindowToTop(hwndButton);
            BringWindowToTop(hwndCombo);
            font = CreateFontIndirect(&systemDialogFont);
            SendMessage(hwndErrButton, WM_SETFONT, (WPARAM)font, 0);
            SendMessage(hwndWarnButton, WM_SETFONT, (WPARAM)font, 0);
            hwndLV = CreateWindowEx(0, WC_LISTVIEW, "", LVS_REPORT | LVS_SINGLESEL | WS_CHILD | WS_VISIBLE | WS_BORDER, 0,
                                    BUTTONHEIGHT + 4, r.right - r.left, r.bottom - r.top - BUTTONHEIGHT - 4, hwnd, 0, hInstance, 0);
            ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
            ApplyDialogFont(hwndLV);
            GetWindowRect(hwndLV, &r);
            lvC.mask = LVCF_WIDTH;
            lvC.cx = 50;
            lvC.iSubItem = 0;
            ListView_InsertColumn(hwndLV, 0, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 50;
            lvC.iSubItem = 1;
            lvC.pszText = "Id";
            ListView_InsertColumn(hwndLV, 1, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 250;
            lvC.iSubItem = 2;
            lvC.pszText = "Name";
            ListView_InsertColumn(hwndLV, 2, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 50;
            lvC.iSubItem = 3;
            lvC.pszText = "Line";
            ListView_InsertColumn(hwndLV, 3, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = r.right - r.left - 375;
            lvC.iSubItem = 4;
            lvC.pszText = "File";
            ListView_InsertColumn(hwndLV, 4, &lvC);
            ListView_SetImageList(hwndLV, tagImageList, LVSIL_SMALL);
            break;
        case WM_RESETHISTORY:
            SendMessage(hwndCombo, WM_SETHISTORY, 0, (LPARAM)varinfohist);
            break;
        case WM_SIZE:
            r.left = r.top = 0;
            r.right = LOWORD(lParam);
            r.bottom = HIWORD(lParam);
            MoveWindow(hwndLV, r.left, r.top + BUTTONHEIGHT + 4, r.right - r.left, r.bottom - r.top - BUTTONHEIGHT - 4, 1);
            MoveWindow(hwndBackground, r.left, r.top, r.right - r.left, BUTTONHEIGHT + 4, 1);
            lvC.mask = LVCF_WIDTH;
            lvC.cx = r.right - r.left - 375;
            ListView_SetColumn(hwndLV, 4, &lvC);
            break;
        case WM_DESTROY:
            hwndBrowse = 0;
            break;
        case WM_SETFOCUS:
            break;
        case WM_KILLFOCUS:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterBrowseWindow(HINSTANCE hInstance)
{
    HBITMAP bmp, nbmp;
    WNDCLASS wc;
    HDC dc, mdc, mdc2;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW + CS_VREDRAW + CS_DBLCLKS;
    wc.lpfnWndProc = &BrowseProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = szBrowseClassName;
    RegisterClass(&wc);

    bmp = LoadImage(hInstance, "ID_VLOC", IMAGE_BITMAP, 48, 16, 0);
    dc = GetDC(HWND_DESKTOP);
    mdc = CreateCompatibleDC(dc);
    mdc2 = CreateCompatibleDC(dc);
    nbmp = CreateCompatibleBitmap(dc, 16, 16);
    nbmp = SelectObject(mdc, nbmp);
    bmp = SelectObject(mdc2, bmp);
    BitBlt(mdc, 0, 0, 16, 16, mdc2, 0, 0, SRCCOPY);
    bmp = SelectObject(mdc2, bmp);
    nbmp = SelectObject(mdc, nbmp);
    DeleteDC(mdc);
    ReleaseDC(HWND_DESKTOP, dc);

    bmp = ConvertToTransparent(bmp, 0xc0c0c0);
    tagImageList = ImageList_Create(16, 16, ILC_COLOR | ILC_MASK, 3, 0);
    ImageList_Add(tagImageList, bmp, NULL);
    DeleteObject(bmp);

    buttonbmp = nbmp;
}

//-------------------------------------------------------------------------

HWND CreateBrowseWindow(void)
{
    hwndBrowse = CreateInternalWindow(DID_BROWSEWND, szBrowseClassName, szBrowseTitle);
    return hwndBrowse;
}
