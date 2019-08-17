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

// assumes tabs aren't going to get reset yet
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include "header.h"
#include <stdlib.h>
#include <ctype.h>
#include <richedit.h>
#include <limits.h>

extern LOGFONT systemDialogFont;
extern HINSTANCE hInstance;
extern KEYLIST C_keywordList[1];
extern HWND hwndShowFunc;

#define TRANSPARENT_COLOR 0x872395

HWND codecompleteBox;
BOOL inStructBox;

struct llist
{
    struct llist* next;
    char name[256];
    int flags;
};
LRESULT CALLBACK codecompProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HWND parent;
    static HWND hwndLB;
    static HBITMAP pictures;
    static char buf[256];
    static int len;
    static int begin_inserted;
    static int selected;
    PAINTSTRUCT ps;
    HDC dc;
    char buf2[256];
    EDITDATA* p;
    DRAWITEMSTRUCT* lpdis;
    MEASUREITEMSTRUCT* lpmis;
    TEXTMETRIC tm;
    RECT r;
    int fg, bg;
    int i = selected;
    HPEN pen;
    HBRUSH brush;
    static struct llist* stringdata;
    static int initialLen;
    char* temp;
    switch (iMessage)
    {
        case WM_MEASUREITEM:
            lpmis = (LPMEASUREITEMSTRUCT)lParam;
            dc = GetDC(hwnd);
            GetTextMetrics(dc, &tm);
            ReleaseDC(hwnd, dc);
            /* Set the height of the list box items. */

            lpmis->itemHeight = tm.tmHeight;
            return TRUE;

            //	 		case WM_COMPAREITEM:
            //				lpcis = (LPCOMPAREITEMSTRUCT)lParam;
            //				return strcmp((char *)lpcis->itemData1, (char *)lpcis->itemData2);
        case WM_DRAWITEM:

            lpdis = (LPDRAWITEMSTRUCT)lParam;
            /* If there are no list box items, skip this message. */

            /*
             * Draw the bitmap and text for the list box item. Draw a

             * rectangle around the bitmap if it is selected.
             */
            switch (lpdis->itemAction)
            {
                int flags, index;
                HDC hdcmem;
                case ODA_SELECT:
                case ODA_DRAWENTIRE:
                    if (lpdis->itemState == ODS_SELECTED)
                    {
                        FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)(COLOR_HIGHLIGHT + 1));
                        fg = SetTextColor(lpdis->hDC, RetrieveSysColor(COLOR_HIGHLIGHTTEXT));
                        bg = SetBkColor(lpdis->hDC, RetrieveSysColor(COLOR_HIGHLIGHT));
                    }
                    else
                    {
                        FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)(COLOR_INFOBK + 1));
                        bg = SetBkColor(lpdis->hDC, RetrieveSysColor(COLOR_INFOBK));
                    }

                    temp = (char*)SendMessage(lpdis->hwndItem, LB_GETITEMDATA, lpdis->itemID, 0);
                    {
                        struct llist* scan = stringdata;
                        while (scan)
                        {
                            if (!strcmp(scan->name, temp))
                            {
                                flags = scan->flags;
                                break;
                            }
                            scan = scan->next;
                        }
                    }
                    index = (flags & 128) ? 2 : (flags & 16) ? 1 : 0;
                    index += (flags & 3) * 3;
                    strcpy(buf2, temp);
                    hdcmem = CreateCompatibleDC(lpdis->hDC);
                    pictures = SelectObject(hdcmem, pictures);
                    TransparentBlt(lpdis->hDC, 3, (lpdis->rcItem.bottom + lpdis->rcItem.top - 14) / 2, 14, 14, hdcmem, index * 14,
                                   0, 14, 14, 0xc0c0c0);
                    pictures = SelectObject(hdcmem, pictures);
                    DeleteDC(hdcmem);
                    GetTextMetrics(lpdis->hDC, &tm);
                    TextOut(lpdis->hDC, 6 + 16, (lpdis->rcItem.bottom + lpdis->rcItem.top - tm.tmHeight) / 2, buf2, strlen(buf2));

                    if (lpdis->itemState == ODS_SELECTED)
                    {
                        SetTextColor(lpdis->hDC, fg);
                        //		 					font = SelectObject(lpdis->hDC, font);
                        //							DeleteObject(font);
                    }
                    SetBkColor(lpdis->hDC, bg);
                    break;

                case ODA_FOCUS:
                    /*
                     * Do not process focus changes. The focus caret

                     * (outline rectangle) indicates the selection.
                     * The Which one? (IDOK) button indicates the final
                     * selection.
                     */
                    break;
            }
            return TRUE;
        case WM_CREATE:
            pictures = LoadBitmap(hInstance, "ID_CCSTRUC");
            hwndLB = CreateWindow("LISTBOX", "",
                                  WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL | LBS_SORT |
                                      LBS_WANTKEYBOARDINPUT | WS_CLIPSIBLINGS,
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, (HMENU)100, hInstance, 0);
            ApplyDialogFont(hwndLB);
            SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xff, LWA_COLORKEY);
            break;
        case WM_DESTROY:
            SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
            DestroyWindow(hwndLB);
            hwndLB = NULL;
            codecompleteBox = NULL;
            inStructBox = FALSE;
            break;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            brush = CreateSolidBrush(TRANSPARENT_COLOR);
            FillRect(dc, &r, brush);
            DeleteObject(brush);

            pen = CreatePen(PS_SOLID, 0, RetrieveSysColor(COLOR_INFOTEXT));
            pen = SelectObject(dc, pen);
            RoundRect(dc, r.left, r.top, r.right, r.bottom, 5, 5);
            pen = SelectObject(dc, pen);
            DeleteObject(pen);

            EndPaint(hwnd, &ps);
            return 0;
        case WM_USER:
            parent = (HWND)lParam;
            p = (EDITDATA*)GetWindowLong(parent, 0);
            insertautoundo(parent, p, UNDO_AUTOEND);
            len = 0;
            selected = 0;
            initialLen = 0;
            begin_inserted = FALSE;
            // fallthrough
        case WM_USER + 1:
        {
            len = 0;
            selected = SendMessage(hwndLB, LB_GETCURSEL, 0, 0);
            if (selected != LB_ERR)
            {
                temp = (char*)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                //                    buf[len++] = temp[0];
                buf[len] = 0;
            }
        }
        break;
        case WM_USER + 2:
        {
            strcpy(buf, (char*)lParam);
            len = initialLen = strlen(buf);
            selected = SendMessage(hwnd, LB_SELECTSTRING, -1, (LPARAM)buf);
            if (selected != LB_ERR)
            {
                temp = (char*)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                strcpy(buf2, temp);
                if (!stricmp(buf, buf2))
                {
                    temp = (char*)SendMessage(hwndLB, LB_GETITEMDATA, selected + 1, 0);
                    if ((int)temp != LB_ERR)
                    {
                        if (strnicmp(temp, buf2, strlen(buf2)) != 0)
                            PostMessage(hwnd, WM_CLOSE, 0, 0);
                    }
                    else
                    {
                        PostMessage(hwnd, WM_CLOSE, 0, 0);
                    }
                }
            }
            return -1;
        }
        break;
        case WM_USER + 3:
        {
            struct llist* scan = stringdata;
            while (scan)
            {
                if (!strcmp(scan->name, (char*)lParam))
                    return TRUE;
                scan = scan->next;
            }
            return FALSE;
        }
        break;
        case WM_SETFOCUS:
            SetFocus(hwndLB);
            break;
        case WM_CLOSE:
            if (!begin_inserted)
            {
                p = (EDITDATA*)GetWindowLong(parent, 0);
                begin_inserted = TRUE;
                insertautoundo(parent, p, UNDO_AUTOBEGIN);
                if (len == 0)
                    doundo(parent, p);
            }
            break;
        case WM_COMMAND:
            if (HIWORD(wParam) == LBN_KILLFOCUS)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            else if (HIWORD(wParam) == LBN_DBLCLK)
            {
                PostMessage(hwndLB, WM_KEYDOWN, VK_RETURN, 0);
            }
            else if (HIWORD(wParam) == LBN_SELCHANGE)
            {
                selected = SendMessage(hwndLB, LB_GETCURSEL, 0, 0);
            }
            break;
        case WM_SIZE:
            MoveWindow(hwndLB, 2, 2, (lParam & 65535) - 4, (lParam >> 16) - 4, 1);
            break;
        case WM_VKEYTOITEM:
            switch (LOWORD(wParam))
            {
                case VK_RETURN:
                case VK_RIGHT:
                    if (len && selected != LB_ERR)
                    {
                        EDITDATA* p = (EDITDATA*)GetWindowLong(parent, 0);
                        int count;
                        begin_inserted = TRUE;
                        insertautoundo(parent, p, UNDO_AUTOBEGIN);
                        doundo(parent, p);

                        temp = (char*)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                        strcpy(buf, temp);
                        count = strlen(buf);
                        p->selstartcharpos -= initialLen;
                        p->selendcharpos = p->selstartcharpos + initialLen;
                        Replace(parent, p, buf, count);
                        p->selstartcharpos = p->selendcharpos = p->selstartcharpos + count;
                        MoveCaret(parent, p);
                    }
                    // fallthrough

                case VK_ESCAPE:
                case VK_LEFT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    return -2;
                case VK_UP:
                case VK_DOWN:
                case VK_PRIOR:
                case VK_NEXT:
                    PostMessage(hwnd, WM_USER + 1, 0, 0);
                    return -1;
                case VK_BACK:
                    if (len > initialLen)
                    {
                        len--;
                        buf[len] = 0;
                        if (len == 0)
                        {
                            selected = 0;
                            PostMessage(hwndLB, LB_SETCURSEL, -1, 0);
                        }
                        else
                            selected = SendMessage(hwnd, LB_SELECTSTRING, -1, (LPARAM)buf);
                    }
                    else
                    {
                        p = (EDITDATA*)GetWindowLong(parent, 0);
                        begin_inserted = TRUE;
                        insertautoundo(parent, p, UNDO_AUTOBEGIN);
                        doundo(parent, p);
                        PostMessage(hwnd, WM_CLOSE, 0, 0);
                    }
                    SendMessage(parent, WM_KEYDOWN, VK_BACK, lParam);
                    return -2;
                case VK_DELETE:
                    len = 0;
                    selected = 0;
                    buf[len] = 0;
                    SendMessage(hwndLB, LB_SETCURSEL, -1, 0);
                    p = (EDITDATA*)GetWindowLong(parent, 0);
                    insertautoundo(parent, p, UNDO_AUTOBEGIN);
                    doundo(parent, p);
                    insertautoundo(parent, p, UNDO_AUTOEND);
                    return -2;
                case VK_SHIFT:
                case VK_CONTROL:
                    break;
                default:
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        case VK_HOME:
                        case VK_END:
                            p = (EDITDATA*)GetWindowLong(parent, 0);
                            begin_inserted = TRUE;
                            insertautoundo(parent, p, UNDO_AUTOBEGIN);
                            //                            doundo(parent, p);
                            PostMessage(hwnd, WM_CLOSE, 0, 0);
                            SendMessage(parent, WM_KEYDOWN, wParam, lParam);
                            return -2;
                    }
                    break;
            }
            return -1;
        case WM_CHARTOITEM:
            wParam = LOWORD(wParam);
            SendMessage(parent, WM_CHAR, wParam, 0);
            if (len != 0 || !isspace(wParam))
            {
                if (!isalnum(wParam) && wParam != '_')
                {
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                }
                else
                {
                    buf[len++] = wParam;
                    buf[len] = 0;
                    selected = SendMessage(hwnd, LB_SELECTSTRING, -1, (LPARAM)buf);
                    if (selected != LB_ERR)
                    {
                        temp = (char*)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                        strcpy(buf2, temp);
                        if (!stricmp(buf, buf2))
                        {
                            // if it is the prefix of another item don't close out the box
                            temp = (char*)SendMessage(hwndLB, LB_GETITEMDATA, selected + 1, 0);
                            if (temp == (char*)-1 || strnicmp(buf, temp, len))
                                PostMessage(hwnd, WM_CLOSE, 0, 0);
                        }
                    }
                }
            }
            return -1;
        case LB_RESETCONTENT:
            SendMessage(hwndLB, LB_RESETCONTENT, 0, 0);
            while (stringdata)
            {
                struct llist* next = stringdata->next;
                free(stringdata);
                stringdata = next;
            }
            return 0;
        case LB_ADDSTRING:
        {
            struct llist *ll = malloc(sizeof(struct llist)), **find = &stringdata;
            if (ll)
            {
                int i = 0, rv;
                strncpy(ll->name, (char*)lParam, 256);
                ll->flags = wParam;
                ll->name[255] = 0;
                while ((*find) && strcmp(ll->name, (*find)->name) > 0)
                {
                    i++;
                    find = &(*find)->next;
                }
                ll->next = *find;
                *find = ll;
                rv = SendMessage(hwndLB, LB_INSERTSTRING, i, (LPARAM)ll->name);
                SendMessage(hwndLB, LB_SETCURSEL, selected = 0, 0);
                return rv;
            }
        }
            return 0;
        case LB_SELECTSTRING:
        {
            int i = wParam + 1;
            do
            {
                temp = (char*)SendMessage(hwndLB, LB_GETITEMDATA, i, 0);
                if ((int)temp != LB_ERR)
                    if (!strnicmp(temp, (char*)lParam, strlen((char*)lParam)))
                    {
                        int n = SendMessage(hwndLB, LB_GETCOUNT, 0, 0);
                        if (n > 8)
                            n = 8;
                        n = i - n / 2;
                        if (n < 0)
                            n = 0;
                        SendMessage(hwndLB, LB_SETTOPINDEX, n, 0);
                        SendMessage(hwndLB, LB_SETCURSEL, i, 0);
                        return i;
                    }
                i++;
            } while ((int)temp != LB_ERR);
            if (wParam != -1)
            {
                for (i = 0; i <= wParam; i++)
                {
                    buf2[0] = 0;
                    temp = (char*)SendMessage(hwndLB, LB_GETITEMDATA, selected, 0);
                    if ((int)temp != LB_ERR)
                        if (!strnicmp(temp, (char*)lParam, strlen((char*)lParam)))
                        {
                            int n = SendMessage(hwndLB, LB_GETCOUNT, 0, 0);
                            if (n > 8)
                                n = 8;
                            n = i - n / 2;
                            if (n < 0)
                                n = 0;
                            SendMessage(hwndLB, LB_SETTOPINDEX, n, 0);
                            SendMessage(hwndLB, LB_SETCURSEL, i, 0);
                            return i;
                        }
                }
            }
            SendMessage(hwndLB, LB_SETCURSEL, -1, 0);
            return LB_ERR;
        }
        default:
            if (iMessage >= LB_ADDSTRING && iMessage <= LB_SETCOUNT)
                return SendMessage(hwndLB, iMessage, wParam, lParam);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static BOOL TranslateStructName(HWND hwndLB, char* out, char* in)
{
    int nested = 0;
    char* p;
    char* last;
    p = last = in;
    while (*p)
    {
        switch (*p)
        {
            case '#':
                nested++;
                break;
            case '~':
                if (nested)
                    nested--;
                break;
            case '@':
                if (!nested)
                    last = p;
                break;
        }
        p++;
    }
    if (last[0] == '@' && last[1] == '$')
        return FALSE;
    unmangle(out, last);
    p = strchr(out, '(');
    if (p)
        *p = 0;
    if (SendMessage(hwndLB, WM_USER + 3, 0, (LPARAM)out))
        return FALSE;
    return TRUE;
}
int showStruct(HWND hwnd, EDITDATA* p, int ch)
{
    if (PropGetInt(NULL, "CODE_COMPLETION") == 0)
        return FALSE;
    if (instring(p->cd->text, &p->cd->text[p->selstartcharpos]))
        return FALSE;

    if (!codecompleteBox)
    {
        int pos = CPPScanBackward(p, p->selendcharpos, ch != ':');  // undefined in local context
        if (pos != p->selendcharpos)
        {
            DWINFO* info = (DWINFO*)GetWindowLong(GetParent(hwnd), 0);
            int lineno = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, p->selstartcharpos) + 1;

            CCSTRUCTDATA* structData;
            char buf[2048], *q = buf;
            int i;
            for (i = pos; i < p->selendcharpos; i++)
                buf[i - pos] = p->cd->text[i].ch;
            buf[i - pos] = 0;
            structData = GetStructType(&q, lineno, info->dwName, NULL, NULL);
            if (structData)
            {
                codecompleteBox = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE, "xcccodeclass", "",
                                                 (WS_POPUP) | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                                 CW_USEDEFAULT, hwnd, 0, hInstance, 0);
                inStructBox = TRUE;
                if (codecompleteBox)
                {
                    int count = 0, i;
                    int max = 0;
                    SIZE siz1;
                    int width, height;
                    POINT cpos;
                    TEXTMETRIC t;
                    HDC dc = GetDC(codecompleteBox);
                    xdrawline(hwnd, p, p->selstartcharpos);  // undefined in local context
                    GetTextMetrics(dc, &t);
                    SendMessage(codecompleteBox, LB_RESETCONTENT, 0, 0);
                    p->cd->selecting = FALSE;
                    width = 0;
                    for (i = 0; i < structData->fieldCount; i++)
                    {
                        int n;
                        char name[512];
                        char* buf = structData->data[i].fieldName;
                        if (TranslateStructName(codecompleteBox, name, buf))
                        {
                            // filter out private members from base classes
                            if ((!(structData->data[i].flags & 256) || (structData->data[i].flags & 3))
                                // filter out nonstatic members when accessed from a static context
                                && (ch != ':' || (structData->data[i].flags & 16)))
                            {
                                count++;
                                GetTextExtentPoint32(dc, name, strlen(name), &siz1);
                                if (siz1.cx + 16 > width)
                                    width = siz1.cx + 16;
                                n = SendMessage(codecompleteBox, LB_ADDSTRING, structData->data[i].flags, (LPARAM)name);
                            }
                        }
                    }
                    ccFreeStructData(structData);
                    ReleaseDC(codecompleteBox, dc);
                    if (count > 8)
                        count = 8;
                    if (width < 70)
                        width = 70;
                    if (count)
                    {
                        height = t.tmHeight * count;
                        width += 10 + GetSystemMetrics(SM_CXVSCROLL) + 2 * GetSystemMetrics(SM_CXFRAME);
                        GetCompletionPos(hwnd, p, &cpos, width, height);
                        SendMessage(codecompleteBox, WM_USER, 0, (LPARAM)hwnd);
                        MoveWindow(codecompleteBox, cpos.x, cpos.y, width + 4, height + 4, TRUE);
                        ShowWindow(codecompleteBox, SW_SHOW);
                        if (IsWindowVisible(hwndShowFunc))
                            ShowWindow(hwndShowFunc, SW_HIDE);
                        return TRUE;
                    }
                    else
                        DestroyWindow(codecompleteBox);
                }
            }
        }
    }
    return FALSE;
}
