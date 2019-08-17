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
#define BUTTONWIDTH 110
#define BUTTONHEIGHT 26
extern HINSTANCE hInstance;
extern HWND hwndClient, hwndFrame;
extern LOGFONT systemDialogFont;

int errorButtons = ERR_ERRORS | ERR_WARNINGS;

static HWND hwndError;
static char szErrorClassName[] = "xccErrorClass";
static HWND hwndLV, hwndErrButton, hwndWarnButton, hwndBackground;
static int curSel;
static HBITMAP hIcoError, hIcoWarning;
static HIMAGELIST tagImageList;

static char* szErrorTitle = "Error List";

static ERRWNDDATA* errlist[1000];
static int errcount, lasterrcount;
static int errors, warnings;
static void CopyText(HWND hwnd)
{
    char* p = NULL;
    int sz = 1;
    int n = errcount;
    int btns = errorButtons;
    int i;
    for (i = 0; i < n && i < errcount; i++)
    {
        char buf[1000];
        if ((errlist[i]->isWarning && (btns & ERR_WARNINGS)) || (!errlist[i]->isWarning && (btns & ERR_ERRORS)))
        {
            sprintf(buf, "%d\t%s\t%s\t%d\t%s\n", i + 1, errlist[i]->isWarning ? "Warn " : "Error", errlist[i]->file,
                    errlist[i]->lineno, errlist[i]->error);
            sz += strlen(buf);
        }
    }
    p = calloc(sz, sizeof(char));
    if (p)
    {
        for (i = 0; i < n && i < errcount; i++)
        {
            char buf[1000];
            if ((errlist[i]->isWarning && (btns & ERR_WARNINGS)) || (!errlist[i]->isWarning && (btns & ERR_ERRORS)))
            {
                sprintf(buf, "%d\t%s\t%s\t%d\t%s\n", i + 1, errlist[i]->isWarning ? "Warn " : "Error", errlist[i]->file,
                        errlist[i]->lineno, errlist[i]->error);
                strcat(p, buf);
            }
        }
        TextToClipBoard(hwnd, p);
        free(p);
    }
}

//-------------------------------------------------------------------------

LRESULT CALLBACK ErrorProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HFONT font;
    LV_ITEM item;
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
            strcpy(info.dwName, errlist[pressed.lParam]->file);
            q = strrchr(info.dwName, '\\');
            if (q)
                strcpy(info.dwTitle, q + 1);
            else
                strcpy(info.dwTitle, info.dwName);
            info.dwLineNo = TagNewLine(errlist[pressed.lParam]->file, errlist[pressed.lParam]->lineno);
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
                    case 2: /* error */
                        strcpy(name, errlist[p->item.lParam]->error);
                        break;
                    case 3: /*file*/
                        q = strrchr(errlist[p->item.lParam]->file, '\\');
                        if (!q)
                            q = errlist[p->item.lParam]->file;
                        else
                            q++;
                        strcpy(name, q);
                        break;
                    case 4: /*line*/
                        if (errlist[p->item.lParam]->lineno >= 0)
                        {
                            sprintf(name, "%d", TagNewLine(errlist[p->item.lParam]->file, errlist[p->item.lParam]->lineno));
                        }
                        else
                        {
                            strcpy(name, "N/A");
                        }
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
            if (HIWORD(wParam) == BN_CLICKED)
            {
                if (lParam == (LPARAM)hwndWarnButton)
                {
                    errorButtons = errorButtons ^ ERR_WARNINGS;
                }
                else
                {
                    errorButtons = errorButtons ^ ERR_ERRORS;
                }
                PostMessage(hwnd, WM_USER, 0, 0);
                SavePreferences();
            }
            break;
        case WM_SETERRDATA:
            if (errcount >= 999)
            {
                free((ERRWNDDATA*)lParam);
            }
            else
            {
                errlist[errcount++] = (ERRWNDDATA*)lParam;
                if (((ERRWNDDATA*)lParam)->isWarning)
                    warnings++;
                else
                    errors++;
                PostMessage(hwnd, WM_USER, 0, 0);
            }
            break;
        case WM_CLEARERRDATA:
            for (i = 0; i < errcount; i++)
            {
                free(errlist[i]);
                errlist[i] = 0;
            }
            errcount = 0;
            errors = 0, warnings = 0;
            SendMessage(hwnd, WM_USER, 0, 0);
            break;
        case WM_USER:
        {
            int k = 0;
            char buf[40];
            sprintf(buf, "%d errors", errors);
            SetWindowText(hwndErrButton, buf);
            sprintf(buf, "%d warnings", warnings);
            SetWindowText(hwndWarnButton, buf);

            if (errcount != lasterrcount)
            {
                lasterrcount = errcount;
                ListView_DeleteAllItems(hwndLV);
                memset(&item, 0, sizeof(item));
                for (i = 0; i < errcount; i++)
                {
                    if ((errlist[i]->isWarning && (errorButtons & ERR_WARNINGS)) ||
                        (!errlist[i]->isWarning && (errorButtons & ERR_ERRORS)))
                    {
                        item.iItem = k++;
                        item.iSubItem = 0;
                        item.mask = LVIF_IMAGE | LVIF_PARAM;
                        item.lParam = i;
                        item.iImage = errlist[i]->isWarning ? 1 : 0;
                        ListView_InsertItem(hwndLV, &item);
                    }
                }
            }
        }
        break;
        case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT pDis = (LPDRAWITEMSTRUCT)lParam;
            char staticText[256];
            HICON image = pDis->hwndItem == hwndErrButton ? hIcoError : hIcoWarning;
            BOOL state = !!(pDis->hwndItem == hwndErrButton ? errorButtons & ERR_ERRORS : errorButtons & ERR_WARNINGS);
            int len = SendMessage(pDis->hwndItem, WM_GETTEXT, sizeof(staticText), (LPARAM)staticText);
            SIZE sz;
            RECT r;
            POINT textpos, iconpos;
            GetClientRect(pDis->hwndItem, &r);
            GetTextExtentPoint32(pDis->hDC, staticText, len, &sz);
            SetBkMode(pDis->hDC, TRANSPARENT);
            iconpos.x = 4;
            iconpos.y = (r.bottom - r.top - 20) / 2;
            textpos.x = r.right - 4 - sz.cx;
            textpos.y = (r.bottom - r.top - sz.cy) / 2;
            DrawFrameControl(pDis->hDC, &pDis->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH | (state ? DFCS_PUSHED : 0));
            TextOut(pDis->hDC, pDis->rcItem.left + textpos.x, pDis->rcItem.top + textpos.y, staticText, len);
            DrawIconEx(pDis->hDC, pDis->rcItem.left + iconpos.x, pDis->rcItem.top + iconpos.y, image, 20, 20, 0, NULL, DI_NORMAL);
        }
            return 0;
        case WM_CREATE:
            hwndError = hwnd;
            GetClientRect(hwnd, &r);
            hwndBackground = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, r.right - r.bottom,
                                          BUTTONHEIGHT + 4, hwnd, 0, hInstance, 0);
            hwndErrButton = CreateWindow("button", "0 errors", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 2, 2, BUTTONWIDTH,
                                         BUTTONHEIGHT, hwnd, 0, hInstance, 0);
            hwndWarnButton = CreateWindow("button", "0 warnings", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 4 + BUTTONWIDTH + 2, 2,
                                          BUTTONWIDTH, BUTTONHEIGHT, hwnd, 0, hInstance, 0);
            BringWindowToTop(hwndErrButton);
            BringWindowToTop(hwndWarnButton);
            font = CreateFontIndirect(&systemDialogFont);
            SendMessage(hwndErrButton, WM_SETFONT, (WPARAM)font, 0);
            SendMessage(hwndWarnButton, WM_SETFONT, (WPARAM)font, 0);
            hwndLV = CreateWindowEx(0, WC_LISTVIEW, "", LVS_REPORT | LVS_SINGLESEL | WS_CHILD | WS_VISIBLE | WS_BORDER, 0,
                                    BUTTONHEIGHT + 4, r.right - r.left, r.bottom - r.top - BUTTONHEIGHT - 4, hwnd, 0, hInstance, 0);
            ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
            ApplyDialogFont(hwndLV);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
            lvC.cx = 30;
            lvC.iSubItem = 0;
            ListView_InsertColumn(hwndLV, 0, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 50;
            lvC.iSubItem = 1;
            lvC.pszText = "Id";
            ListView_InsertColumn(hwndLV, 1, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 300;
            lvC.iSubItem = 2;
            lvC.pszText = "Error";
            ListView_InsertColumn(hwndLV, 2, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 120;
            lvC.iSubItem = 3;
            lvC.pszText = "File";
            ListView_InsertColumn(hwndLV, 3, &lvC);
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 50;
            lvC.iSubItem = 4;
            lvC.pszText = "Line";
            ListView_InsertColumn(hwndLV, 4, &lvC);
            ListView_SetImageList(hwndLV, tagImageList, LVSIL_SMALL);
            break;
        case WM_SIZE:
            r.left = r.top = 0;
            r.right = LOWORD(lParam);
            r.bottom = HIWORD(lParam);
            MoveWindow(hwndLV, r.left, r.top + BUTTONHEIGHT + 4, r.right - r.left, r.bottom - r.top - BUTTONHEIGHT - 4, 1);
            MoveWindow(hwndBackground, r.left, r.top, r.right - r.left, BUTTONHEIGHT + 4, 1);
            lvC.mask = LVCF_WIDTH;
            lvC.cx = r.right - r.left - 270;
            ListView_SetColumn(hwndLV, 2, &lvC);
            break;
        case WM_DESTROY:
            hwndError = 0;
            break;
        case WM_SETFOCUS:
            break;
        case WM_KILLFOCUS:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterErrorWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW + CS_VREDRAW + CS_DBLCLKS;
    wc.lpfnWndProc = &ErrorProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = szErrorClassName;
    RegisterClass(&wc);

    hIcoError = LoadImage(hInstance, MAKEINTRESOURCE(ID_ERRICO), IMAGE_ICON, 20, 20, 0);
    hIcoWarning = LoadImage(hInstance, MAKEINTRESOURCE(ID_WARNICO), IMAGE_ICON, 20, 20, 0);
    tagImageList = ImageList_Create(20, 20, ILC_COLOR | ILC_MASK, 2, 0);
    ImageList_AddIcon(tagImageList, hIcoError);
    ImageList_AddIcon(tagImageList, hIcoWarning);
}

//-------------------------------------------------------------------------

HWND CreateErrorWindow(void)
{
    hwndError = CreateInternalWindow(DID_ERRWND, szErrorClassName, szErrorTitle);
    return hwndError;
}
