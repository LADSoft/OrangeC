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

extern HINSTANCE hInstance;
extern HWND hwndTbFind;
extern int modifiedFind;

static int wndoffs;
static WNDPROC oldComboProc;
static WNDPROC oldEditProc;

static char* propname = "HISTBUF";

LRESULT CALLBACK historyEditComboProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_RETURN:
                case VK_ESCAPE:
                    SendMessage(GetParent(hwnd), WM_COMMAND, (wParam << 16) + 1698, (LPARAM)hwnd);
                    return 0;
            }
            break;
        case WM_KEYUP:
            if (hwnd == hwndTbFind)
                modifiedFind = TRUE;
            switch (wParam)
            {
                case VK_RETURN:
                case VK_ESCAPE:
                    return 0;
            }
            break;
        default:
            break;
    }
    return CallWindowProc(oldEditProc, hwnd, iMessage, wParam, lParam);
}

LRESULT CALLBACK historyComboProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    char** cbptr;
    char buf[MAX_PATH];
    int i;
    POINT pt;
    HWND hwndedit;
    pt.x = pt.y = 1;
    hwndedit = ChildWindowFromPoint(hwnd, pt);
    switch (iMessage)
    {
        case EM_GETMODIFY:
            return SendMessage(hwndedit, EM_GETMODIFY, wParam, lParam);
        case EM_SETMODIFY:
            return SendMessage(hwndedit, EM_SETMODIFY, wParam, lParam);

        case WM_SETMODIFY:
            return SendMessage(hwndedit, WM_SETTEXT, 0, (LPARAM) " ");
            // hack to fix a timing varagary that kept the find button disabled even though text was changing
        case WM_COMMAND:
            if (LOWORD(wParam) == 1698)
            {
                // return or escape key pressed in edit box...
                PostMessage((HWND)GetWindowLong(hwnd, GWL_HWNDPARENT), WM_COMMAND, (4000 + HIWORD(wParam)), (LPARAM)hwnd);
            }
            //         if ((wParam >> 16) == EN_CHANGE || (wParam >> 16) == EN_UPDATE)
            //            return SendMessage(GetParent(hwnd),WM_COMMAND,(wParam & 0xffff0000) + GetWindowLong(hwnd,GWL_ID),
            //            (LPARAM)hwnd) ;
            break;
        case WM_SETHISTORY:
            cbptr = (char**)lParam;
            SetProp(hwnd, propname, cbptr);
            for (i = 0; i < MAX_COMBO_HISTORY; i++)
                if (cbptr[i])
                {
                    SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)cbptr[i]);
                }
            SendMessage(hwnd, CB_SETCURSEL, 0, 0);

            return (0);
        case WM_SAVEHISTORY:
            cbptr = GetProp(hwnd, propname);
            if (cbptr)
            {
                int sel;  // = SendMessage(hwnd, CB_GETCURSEL,0,0) ;
                buf[SendMessage(hwndedit, WM_GETTEXT, MAX_PATH, (LPARAM)buf)] = 0;
                sel = SendMessage(hwnd, CB_FINDSTRINGEXACT, 0, (LPARAM)buf);
                if (sel == CB_ERR)
                {
                    if (buf[0])
                    {
                        if (cbptr[MAX_COMBO_HISTORY - 1])
                        {
                            free(cbptr[MAX_COMBO_HISTORY - 1]);
                        }
                        memmove(cbptr + 1, cbptr, (MAX_COMBO_HISTORY - 1) * sizeof(char*));
                        cbptr[0] = strdup(buf);
                        SendMessage(hwnd, CB_INSERTSTRING, 0, (LPARAM)buf);
                        //                  SendMessage(hwnd,CB_SETCURSEL,-1,0 );
                        //                  SendMessage(hwndedit,WM_SETTEXT,0,(LPARAM)buf) ;
                    }
                }
                else
                {
                    char* s = cbptr[sel];
                    memmove(cbptr + 1, cbptr, sel * sizeof(char*));
                    cbptr[0] = s;
                }
            }
            return 0;
        case WM_DESTROY:
            RemoveProp(hwnd, propname);
            break;
    }
    return CallWindowProc(oldComboProc, hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void SubClassHistoryCombo(HWND combo)
{
    POINT pt;
    HWND editWnd;
    pt.x = 5;
    pt.y = 5;
    editWnd = ChildWindowFromPoint(combo, pt);
    SetWindowLong(combo, GWL_WNDPROC, (int)historyComboProc);
    SetWindowLong(editWnd, GWL_WNDPROC, (int)historyEditComboProc);
}

static DWORD CALLBACK neEditComboProc(HWND hwnd, unsigned iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_CHAR:
        case WM_DEADCHAR:
        case WM_KEYDOWN:
        case WM_KEYUP:
            return 0;
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
        case EM_EXSETSEL:
        case EM_SETSEL:
            return 0;
        case WM_SETFOCUS:
            CallWindowProc(oldEditProc, hwnd, iMessage, wParam, lParam);
            HideCaret(hwnd);
            return 0;
    }
    return CallWindowProc(oldEditProc, hwnd, iMessage, wParam, lParam);
}
void SubClassNECombo(HWND combo)
{
    POINT pt;
    HWND editWnd;
    pt.x = 5;
    pt.y = 5;
    editWnd = ChildWindowFromPoint(combo, pt);
    SetWindowLong(editWnd, GWL_WNDPROC, (int)neEditComboProc);
}

//-------------------------------------------------------------------------

void RegisterHistoryComboWindow(void)
{
    WNDCLASS wc;
    GetClassInfo(0, "combobox", &wc);
    oldComboProc = wc.lpfnWndProc;
    GetClassInfo(0, "edit", &wc);
    oldEditProc = wc.lpfnWndProc;
}
