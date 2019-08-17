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

#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "header.h"
#include "idewinconst.h"

extern LOGFONT systemDialogFont;

static char* szextTreeWindClassName = "ladSoftextTreeWindow";
static char* szColumnTreeWindClassName = "ladSoftColumnTreeWindow";
static char* szextEditWindClassName = "ladSoftextEditWindow";
static int wndoffstree, wndoffsedit;
static WNDPROC oldproc, oldeditproc;
static LOGFONT fontdata = {-11,
                           0,
                           0,
                           0,
                           FW_NORMAL,
                           FALSE,
                           FALSE,
                           FALSE,
                           ANSI_CHARSET,
                           OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS,
                           DEFAULT_QUALITY,
                           FF_DONTCARE,
                           "Helvetica"};

#define DISPLAY_MAX 500
typedef struct
{
    HWND hwndHeader;
    HWND hwndTree;
    HFONT watchFont;
    HTREEITEM displaylist[DISPLAY_MAX];
    int displaycount;
    int sel;
} COLUMNINFO;

typedef struct
{
    HWND hwndEdit;
    HTREEITEM editItem;
    int divider;
} TREEINFO;
LRESULT CALLBACK extEditWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_CHAR:
            if (wParam == 13)
            {
                SendMessage(GetParent(hwnd), TCN_EDITDONE, 0, 0);
                return 0;
            }
    }
    return CallWindowProc(oldeditproc, hwnd, iMessage, wParam, lParam);
}

int LoadItemText(HWND hwnd, HTREEITEM htreeItem, char* buf, int indent)
{
    TV_ITEM item;
    TCData* td;
    memset(&item, 0, sizeof(item));
    item.hItem = htreeItem;
    item.mask = TVIF_PARAM;
    TreeView_GetItem(hwnd, &item);
    td = (TCData*)item.lParam;
    if (td)
    {
        int size = strlen(td->col1Text) + indent;
        int n = 40 - size, i;
        if (n < 0)
            n = 0;
        if (td->col2Text)
        {
            size += n + 1;
            size += strlen(td->col2Text);
        }
        size++;
        if (buf)
        {
            for (i = 0; i < indent; i++)
                strcat(buf, " ");
            strcat(buf, td->col1Text);
            if (td->col2Text)
            {
                for (i = 0; i < n; i++)
                    strcat(buf, " ");

                strcat(buf, " ");
                strcat(buf, td->col2Text);
            }
            strcat(buf, "\n");
        }
        return size;
    }
    return 0;
}
int GetClipboardText(HWND hwnd, HTREEITEM item, char* buf, int indent)
{
    int rv = 0;
    HTREEITEM current;
    if (item == NULL)
    {
        current = TreeView_GetRoot(hwnd);
    }
    else
    {
        rv += LoadItemText(hwnd, item, buf, indent);
        current = TreeView_GetChild(hwnd, item);
    }
    if (current)
    {
        do
            rv += GetClipboardText(hwnd, current, buf, indent + 2);
        while ((current = TreeView_GetNextSibling(hwnd, current)));
    }
    return rv;
}
void CopyText(HWND hwnd)
{
    int len = GetClipboardText(hwnd, NULL, NULL, -2) + 1;
    char* p = malloc(len);
    if (p)
    {
        p[0] = 0;
        GetClipboardText(hwnd, NULL, p, -2);
        TextToClipBoard(hwnd, p);
        free(p);
    }
}
//-------------------------------------------------------------------------

LRESULT CALLBACK extTreeWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static char buf[256];
    RECT r;
    TREEINFO* ptr;
    HTREEITEM hTreeItem;
    TCData* td;
    TV_HITTESTINFO tvh;
    TV_ITEM item;
    NM_TREEVIEW x;
    switch (iMessage)
    {
        case WM_NOTIFY:
            //            if (((NMHDR*)lParam)->code == TVN_SELCHANGING)
            //
            return TRUE;
            break;
        case WM_CREATE:
            ptr = (TREEINFO*)calloc(1, sizeof(TREEINFO));
            ptr->hwndEdit = 0;
            SetWindowLong(hwnd, wndoffstree, (int)ptr);
            break;
        case WM_DESTROY:
            ptr = (TREEINFO*)GetWindowLong(hwnd, wndoffstree);
            if (ptr->hwndEdit)
                DestroyWindow(ptr->hwndEdit);
            free(ptr);
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            GetUpdateRect(hwnd, &r, FALSE);
            {
                PAINTSTRUCT ps;
                HDC hDC = BeginPaint(hwnd, &ps), hdouble;
                HBITMAP bitmap;
                RECT rect;
                GetClientRect(hwnd, &rect);
                hdouble = CreateCompatibleDC(hDC);
                bitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
                SelectObject(hdouble, bitmap);
                FillRect(hdouble, &rect, (HBRUSH)(COLOR_WINDOW + 1));
                CallWindowProc(oldproc, hwnd, WM_PRINT, (WPARAM)hdouble, PRF_CLIENT);
                SendMessage(GetParent(hwnd), TCN_PAINT, (WPARAM)hdouble, (LPARAM)&r);
                BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
                DeleteObject(bitmap);
                DeleteDC(hdouble);
                EndPaint(hwnd, &ps);
                return 0;
            }
        case WM_KEYDOWN:
            if (wParam == 'C' && (GetKeyState(VK_CONTROL) & 0x80000000))
            {
                CopyText(hwnd);
            }
            break;
        case WM_SETFOCUS:
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            break;
        case WM_CHAR:
            if (wParam == 3)  // CTRL-C - this is being done to stop beeping...
                return 0;
            break;
        case WM_LBUTTONDOWN:
            ptr = (TREEINFO*)GetWindowLong(hwnd, wndoffstree);
            if (!ptr->hwndEdit)
            {
                tvh.pt.x = LOWORD(lParam);
                tvh.pt.y = HIWORD(lParam);
                if (tvh.pt.x < ptr->divider)
                {
                    SetFocus(hwnd);
                    break;
                }
                TreeView_HitTest(hwnd, &tvh);
                hTreeItem = tvh.hItem;
                if (hTreeItem && tvh.pt.x >= ptr->divider)
                {
                    char* val;
                    x.hdr.code = TCN_EDITQUERY;
                    x.itemOld.mask = 0;
                    x.itemNew.mask = 0;
                    x.itemNew.hItem = hTreeItem;
                    if ((val = (char*)SendMessage(GetParent(hwnd), WM_NOTIFY, 0, (LPARAM)&x)))
                    {
                        HFONT font = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
                        ptr->editItem = hTreeItem;
                        TreeView_GetItemRect(hwnd, hTreeItem, &r, FALSE);
                        r.left = ptr->divider;
                        item.hItem = hTreeItem;
                        item.mask = TVIF_PARAM;
                        TreeView_GetItem(hwnd, &item);
                        td = (TCData*)item.lParam;
                        ptr->hwndEdit = CreateWindow(szextEditWindClassName, val, WS_CHILD | ES_AUTOHSCROLL, r.left + 2, r.top,
                                                     r.right - r.left - 2, r.bottom - r.top, hwnd, (HMENU)449,
                                                     (HINSTANCE)GetWindowLong(GetParent(hwnd), GWL_HINSTANCE), 0);
                        SendMessage(ptr->hwndEdit, WM_SETFONT, (WPARAM)font, MAKELPARAM(0, 0));
                        SendMessage(ptr->hwndEdit, EM_SETSEL, 0, (LPARAM)-1);
                        SendMessage(ptr->hwndEdit, EM_SETLIMITTEXT, 40, 0);
                        ShowWindow(ptr->hwndEdit, SW_SHOW);
                        SetFocus(ptr->hwndEdit);
                        return 0;
                    }
                }
                return 0;
            }
            // FALL THROUGH
        case TCN_EDITDONE:
            ptr = (TREEINFO*)GetWindowLong(hwnd, wndoffstree);
            SendMessage(ptr->hwndEdit, WM_GETTEXT, 50, (LPARAM)buf);
            DestroyWindow(ptr->hwndEdit);
            ptr->hwndEdit = 0;
            x.hdr.code = TCN_EDITDONE;
            x.itemOld.mask = 0;
            x.itemNew.mask = TVIF_TEXT;
            x.itemNew.hItem = ptr->editItem;
            x.itemNew.pszText = buf;
            SendMessage(GetParent(hwnd), WM_NOTIFY, 0, (LPARAM)&x);
            InvalidateRect(GetParent(hwnd), 0, 0);
            return 0;
        case TCF_SETDIVIDER:
            ptr = (TREEINFO*)GetWindowLong(hwnd, wndoffstree);
            ptr->divider = lParam - GetSystemMetrics(SM_CXBORDER);
            return 0;
    }
    return CallWindowProc(oldproc, hwnd, iMessage, wParam, lParam);
}
//-------------------------------------------------------------------------

LRESULT CALLBACK ColumnTreeWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HTREEITEM titem;
    HD_LAYOUT hdl;
    WINDOWPOS wp;
    COLUMNINFO* ptr;
    RECT r, *rp;
    HD_ITEM hie;
    TCHeader* h;
    HD_NOTIFY* n;
    TV_DISPINFO* t;
    LPTV_INSERTSTRUCT is;
    int i;
    TV_ITEM item;
    TCData* td;
    TV_HITTESTINFO tvh;
    PAINTSTRUCT ps;
    HDC dc;
    NM_TREEVIEW* ntv;
    if (iMessage >= TV_FIRST && iMessage < TV_FIRST + 100)
    {
        ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
        switch (iMessage)
        {
            case TVM_HITTEST:
                tvh.pt = ((TV_HITTESTINFO*)lParam)->pt;
                ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
                GetRelativeRect(hwnd, ptr->hwndTree, &r);  // undefined in local context
                tvh.pt.x -= r.left;
                tvh.pt.y -= r.top;
                if ((titem = TreeView_HitTest(ptr->hwndTree, &tvh)))
                {
                    ((TV_HITTESTINFO*)lParam)->flags = tvh.flags;
                    ((TV_HITTESTINFO*)lParam)->hItem = tvh.hItem;
                }
                return (LRESULT)titem;
            case TVM_INSERTITEM:
                is = (LPTV_INSERTSTRUCT)lParam;
                is->UNNAMED_UNION item.mask |= TVIF_TEXT | TVIF_PARAM;
                is->UNNAMED_UNION item.pszText = LPSTR_TEXTCALLBACK;
                titem = (HTREEITEM)SendMessage(ptr->hwndTree, iMessage, wParam, lParam);
                return (LRESULT)titem;
            default:
                return SendMessage(ptr->hwndTree, iMessage, wParam, lParam);
        }
    }
    else if (iMessage >= HDM_FIRST && iMessage < HDM_FIRST + 100)
    {
        ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
        return SendMessage(ptr->hwndHeader, iMessage, wParam, lParam);
    }
    switch (iMessage)
    {
        case WM_ERASEBKGND:
            return 1;
        case WM_NOTIFY:
            n = (HD_NOTIFY*)lParam;
            ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
            switch (n->hdr.code)
            {
                case NM_RCLICK:
#ifdef XXXXX
                    ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
                    GetCursorPos(&pos);
                    ScreenToClient(ptr->hwndTree, &pos);
                    titem = TreeView_HitTest(ptr->hwndTree, &pos);
                    for (i = 0; i < ptr->displaycount; i++)
                        if (titem == ptr->displaylist[i])
                        {
                            ptr->sel = i;
                            InvalidateRect(ptr->hwndTree, 0, 0);
                            break;
                        }
#endif
                    return SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
                case TVN_SELCHANGING:
                    return TRUE;
                case HDN_ENDTRACK:
                    SendMessage(ptr->hwndTree, TCF_SETDIVIDER, 0, n->pitem->cxy);
                    InvalidateRect(ptr->hwndTree, 0, 1);
                    return 0;
                case TVN_GETDISPINFO:
                    t = (LPNMTVDISPINFO)n;
                    if (TreeView_GetItemRect(ptr->hwndTree, t->item.hItem, &r, TRUE))
                    {
                        if (ptr->displaycount < DISPLAY_MAX)
                            ptr->displaylist[ptr->displaycount++] = t->item.hItem;
                        strcpy(t->item.pszText, "");
                        //                    t->item.cchTextMax = 0;
                    }
                    return 0;
                case TVN_DELETEITEM:
                    ntv = (NM_TREEVIEW*)lParam;
                    if (ptr->displaycount)
                        for (i = 0; i < ptr->displaycount; i++)
                            if (ptr->displaylist[i] == ntv->itemOld.hItem)
                            {
                                ptr->sel = -1;
                                memcpy(&ptr->displaylist[i], &ptr->displaylist[i + 1],
                                       (ptr->displaycount - i - 1) * sizeof(HTREEITEM));
                                ptr->displaycount--;
                                return 0;
                            }
                    return 0;
            }
            // fall through
        case WM_COMMAND:
            return SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
        case WM_CREATE:
            ptr = (COLUMNINFO*)calloc(1, sizeof(COLUMNINFO));
            ptr->displaycount = 0;
            ptr->sel = -1;
            SetWindowLong(hwnd, 0, (int)ptr);
            GetClientRect(hwnd, &r);
            ptr->hwndHeader =
                CreateWindow(WC_HEADER, 0, WS_CLIPSIBLINGS | WS_CHILD | HDS_HORZ | WS_BORDER, r.left, r.top, r.right - r.left,
                             r.bottom - r.top, hwnd, 0, (HINSTANCE)GetWindowLong(GetParent(hwnd), GWL_HINSTANCE), 0);
            hdl.prc = &r;
            hdl.pwpos = &wp;
            SendMessage(ptr->hwndHeader, HDM_LAYOUT, 0, (LPARAM)&hdl);
            //         wp.x += 2*GetSystemMetrics(SM_CXDLGFRAME );
            //         wp.cx -= 4 * GetSystemMetrics(SM_CXDLGFRAME) ;
            //         wp.y += 2*GetSystemMetrics(SM_CYDLGFRAME );
            //         wp.cy -= 4 * GetSystemMetrics(SM_CYDLGFRAME) ;
            ptr->watchFont = CreateFontIndirect(&systemDialogFont);
            SendMessage(ptr->hwndHeader, WM_SETFONT, (WPARAM)ptr->watchFont, 0);
            SetWindowPos(ptr->hwndHeader, wp.hwndInsertAfter, wp.x, wp.y, wp.cx, wp.cy, wp.flags | SWP_SHOWWINDOW);
            r.left = wp.x;
            r.right = wp.x + wp.cx;
            r.top = wp.y + wp.cy;
            ptr->hwndTree = CreateWindowEx(0, szextTreeWindClassName, 0,
                                           WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_LINESATROOT |
                                               TVS_HASBUTTONS,
                                           r.left, r.top, r.right - r.left, r.bottom - r.top, hwnd, (HMENU)1000,
                                           (HINSTANCE)GetWindowLong(GetParent(hwnd), GWL_HINSTANCE), 0);
            SendMessage(ptr->hwndTree, WM_SETFONT, (WPARAM)ptr->watchFont, 0);
            hie.mask = HDI_WIDTH;
            SendMessage(ptr->hwndHeader, HDM_GETITEM, 0, (LPARAM)&hie);
            SendMessage(ptr->hwndTree, TCF_SETDIVIDER, 0, hie.cxy);
            break;

        case WM_DESTROY:
            ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
            DestroyWindow(ptr->hwndHeader);
            DeleteObject(ptr->watchFont);
            free((void*)ptr);
            break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            break;
        case WM_SIZE:
            ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
            r.left = r.top = 0;
            r.right = LOWORD(lParam);
            r.bottom = HIWORD(lParam);
            hdl.prc = &r;
            hdl.pwpos = &wp;
            SendMessage(ptr->hwndHeader, HDM_LAYOUT, 0, (LPARAM)&hdl);
            //         wp.x += 2*GetSystemMetrics(SM_CXDLGFRAME );
            //         wp.cx -= 4 * GetSystemMetrics(SM_CXDLGFRAME) ;
            //         wp.y += 2*GetSystemMetrics(SM_CYDLGFRAME );
            //         wp.cy -= 4 * GetSystemMetrics(SM_CYDLGFRAME) ;
            SetWindowPos(ptr->hwndHeader, wp.hwndInsertAfter, wp.x, wp.y, wp.cx, wp.cy, wp.flags);
            r.left = wp.x;
            r.right = wp.x + wp.cx;
            r.top = wp.y + wp.cy;
            MoveWindow(ptr->hwndTree, r.left, r.top, r.right - r.left, r.bottom - r.top, 0);
            GetClientRect(ptr->hwndHeader, &r);
            hie.mask = HDI_WIDTH;
            hie.cxy = (r.right - r.left) / 2;
            SendMessage(ptr->hwndHeader, HDM_SETITEM, 0, (LPARAM)&hie);
            SendMessage(ptr->hwndHeader, HDM_SETITEM, 1, (LPARAM)&hie);
            SendMessage(ptr->hwndTree, TCF_SETDIVIDER, 0, hie.cxy);
            InvalidateRect(ptr->hwndHeader, 0, 1);
            InvalidateRect(ptr->hwndTree, 0, 1);
            break;
        case TCF_SETHEADER:
            ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
            h = (TCHeader*)lParam;
            GetWindowRect(ptr->hwndHeader, &r);
            hie.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
            if (h->colBmp1)
                hie.mask |= HDI_BITMAP;
            hie.pszText = h->colText1;
            hie.hbm = h->colBmp1;
            hie.cxy = (r.right - r.left) / 2;
            hie.cchTextMax = strlen(h->colText1);
            hie.fmt = HDF_LEFT | HDF_STRING;
            SendMessage(ptr->hwndHeader, HDM_INSERTITEM, 100, (LPARAM)&hie);
            hie.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
            if (h->colBmp2)
                hie.mask |= HDI_BITMAP;
            hie.pszText = h->colText2;
            hie.hbm = h->colBmp2;
            hie.cxy = (r.right - r.left) / 2;
            hie.cchTextMax = strlen(h->colText2);
            hie.fmt = HDF_LEFT | HDF_STRING;
            SendMessage(ptr->hwndHeader, HDM_INSERTITEM, 100, (LPARAM)&hie);
            SendMessage(ptr->hwndTree, TCF_SETDIVIDER, 0, hie.cxy);
            return 0;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        case WM_ACTIVATEME:
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            break;
        case WM_SETFONT:
        case WM_GETFONT:
            ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
            return SendMessage(ptr->hwndTree, iMessage, wParam, lParam);
        case TCN_PAINT:
            rp = (RECT*)lParam;
            ptr = (COLUMNINFO*)GetWindowLong(hwnd, 0);
            {
                hie.mask = HDI_WIDTH;
                SendMessage(ptr->hwndHeader, HDM_GETITEM, 0, (LPARAM)&hie);
                hie.cxy -= GetSystemMetrics(SM_CXBORDER);
                rp->left = hie.cxy;
                if (rp->left < rp->right)
                {
                    LOGBRUSH lbrush;
                    HBRUSH graybrush;
                    HDC dc = (HDC)wParam;
                    int lined = FALSE;
                    HFONT font = (HFONT)SendMessage(ptr->hwndTree, WM_GETFONT, 0, 0);
                    lbrush.lbStyle = BS_SOLID;
                    lbrush.lbColor = 0xff0000;
                    graybrush = CreateBrushIndirect(&lbrush);
                    //                    dc = GetDC(ptr->hwndTree);
                    font = SelectObject(dc, font);
                    if (GetWindowLong(hwnd, GWL_STYLE) & TCS_LINE)
                    {
                        HPEN pen;
                        pen = CreatePen(PS_SOLID, 1, 0);

                        lined = TRUE;
                        pen = SelectObject(dc, pen);
                        MoveToEx(dc, rp->left, rp->top, 0);
                        LineTo(dc, rp->left, rp->bottom + 1);
                        pen = SelectObject(dc, pen);
                        DeleteObject(pen);
                    }
                    for (i = 0; i < ptr->displaycount; i++)
                    {
                        COLORREF color, bgcolor;
                        item.hItem = ptr->displaylist[i];
                        item.mask = TVIF_PARAM;
                        TreeView_GetItem(ptr->hwndTree, &item);
                        td = (TCData*)item.lParam;
                        TreeView_GetItemRect(ptr->hwndTree, ptr->displaylist[i], &r, TRUE);
                        if (td->col1Text)
                        {
                            HRGN rgn;
                            rgn = CreateRectRgn(r.left, r.top, rp->left - 2, r.bottom);
                            SelectClipRgn(dc, rgn);
                            if (ptr->sel == i)
                            {
                                color = SetTextColor(dc, RetrieveSysColor(COLOR_WINDOW));
                                bgcolor = SetBkColor(dc, td->col1Color);
                            }
                            else
                                color = SetTextColor(dc, td->col1Color);
                            TextOut(dc, r.left, r.top, td->col1Text, strlen(td->col1Text));
                            SetTextColor(dc, color);
                            if (ptr->sel == i)
                                SetBkColor(dc, bgcolor);
                            SelectClipRgn(dc, NULL);
                            DeleteObject(rgn);
                        }
                        if (td->col2Text)
                        {
                            if (ptr->sel == i)
                            {
                                color = SetTextColor(dc, RetrieveSysColor(COLOR_WINDOW));
                                bgcolor = SetBkColor(dc, td->col2Color);
                            }
                            else
                                color = SetTextColor(dc, td->col2Color);
                            TextOut(dc, rp->left + (lined ? 3 : 0), r.top, td->col2Text, strlen(td->col2Text));
                            SetTextColor(dc, color);
                            if (ptr->sel == i)
                                SetBkColor(dc, bgcolor);
                        }
                    }
                    SelectObject(dc, font);
                    //                    ReleaseDC(ptr->hwndTree, dc);
                    DeleteObject(graybrush);
                }
                ptr->displaycount = 0;
            }
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterextTreeWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    GetClassInfo(0, WC_TREEVIEW, &wc);
    oldproc = wc.lpfnWndProc;
    wc.lpfnWndProc = &extTreeWndProc;
    wc.lpszClassName = szextTreeWindClassName;
    wc.hInstance = hInstance;
    wc.cbWndExtra += (4 - wc.cbWndExtra % 4);
    wndoffstree = wc.cbWndExtra;
    wc.cbWndExtra += 4;
    RegisterClass(&wc);

    GetClassInfo(0, "edit", &wc);
    oldeditproc = wc.lpfnWndProc;
    wc.lpfnWndProc = &extEditWndProc;
    wc.lpszClassName = szextEditWindClassName;
    wc.hInstance = hInstance;
    wc.cbWndExtra += (4 - wc.cbWndExtra % 4);
    wndoffsedit = wc.cbWndExtra;
    wc.cbWndExtra += 4;
    RegisterClass(&wc);

    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &ColumnTreeWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(COLOR_WINDOW);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szColumnTreeWindClassName;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

HWND CreateextTreeWindow(HWND parent, int style, RECT* r, TCHeader* h)
{
    HWND hwnd = CreateWindowEx(WS_EX_NOPARENTNOTIFY, szColumnTreeWindClassName, "", WS_CLIPSIBLINGS | WS_CHILD | style, r->left,
                               r->top, r->right - r->left, r->bottom - r->top, parent, (HMENU)0,
                               (HINSTANCE)GetWindowLong(parent, GWL_HINSTANCE), 0);

    if (hwnd)
        SendMessage(hwnd, TCF_SETHEADER, 0, (LPARAM)h);
    return hwnd;
}
