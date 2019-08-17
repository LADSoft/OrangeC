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
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>

#include "header.h"

extern HANDLE hInstance;
extern HWND hwndClient, hwndFrame;
extern LOGFONT systemDialogFont;
extern TBBUTTON infoButtons[];
extern char* infoHints[];
extern BOOL latch;

static HWND hwndFind1, hwndFind2;
static char szFindClassName[] = "xccFindClass";
static WNDPROC oldproc;
static int wheelIncrement;

typedef struct findData
{
    HWND toolbar;
    HWND editor;
} FINDDATA;
//-------------------------------------------------------------------------

BOOL Findbump(HWND hwnd, char* buffer)
{
    DWINFO info;
    char *t, *q;
    int lineno;
    lineno = -1;
    t = info.dwName;
    q = buffer;
    *t++ = *q++;
    *t++ = *q++;  // past the ':'
    while (*q && *q != ':')
    {
        *t++ = *q++;
    }
    *t = 0;
    q++;
    while (isspace(*q))
        q++;
    if (!isdigit(*q))
        return FALSE;
    lineno = atoi(q);
    q = strrchr(info.dwName, '\\');
    if (q)
        strcpy(info.dwTitle, q + 1);
    else
        strcpy(info.dwTitle, info.dwName);
    if (lineno != -1)
    {
        lineno = TagNewLine(info.dwName, lineno);
        info.dwLineNo = lineno;
        info.logMRU = FALSE;
        info.newFile = FALSE;
        CreateDrawWindow(&info, TRUE);
        return TRUE;
    }
    return FALSE;
}
static void FindPrev(HWND hwnd)
{
    char buffer[512];
    int lineno;
    int start;
    int end = 0;
    BOOL found = FALSE;
    SendMessage(hwnd, EM_GETSEL, (WPARAM)&start, 0);
    lineno = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, start);
    while (lineno--)
    {
        *(short*)buffer = 512;
        buffer[SendMessage(hwnd, EM_GETLINE, lineno, (LPARAM)buffer)] = 0;
        if ((found = Findbump(hwnd, buffer)))
            break;
    }
    if (found)
    {
        CHARRANGE a;
        a.cpMin = SendMessage(hwnd, EM_LINEINDEX, lineno, 0);
        a.cpMax = a.cpMin;
        SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&a);
        SendMessage(hwnd, EM_SCROLLCARET, 0, 0);
        a.cpMax = a.cpMin + strlen(buffer);
        SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&a);
    }
}
static void FindNext(HWND hwnd)
{
    char buffer[512];
    int lineno;
    int start;
    int end = SendMessage(hwnd, EM_GETLINECOUNT, 0, 0);
    BOOL found = FALSE;
    SendMessage(hwnd, EM_GETSEL, (WPARAM)&start, 0);
    lineno = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, start);
    while (++lineno < end)
    {
        *(short*)buffer = 512;
        buffer[SendMessage(hwnd, EM_GETLINE, lineno, (LPARAM)buffer)] = 0;
        if ((found = Findbump(hwnd, buffer)))
            break;
    }
    if (found)
    {
        CHARRANGE a;
        a.cpMin = SendMessage(hwnd, EM_LINEINDEX, lineno, 0);
        a.cpMax = a.cpMin;
        SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&a);
        SendMessage(hwnd, EM_SCROLLCARET, 0, 0);
        a.cpMax = a.cpMin + strlen(buffer);
        SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&a);
    }
}
static void FindBumpToEditor(HWND hwnd)
{
    char buffer[512];
    int lineno;
    int start;
    SendMessage(hwnd, EM_GETSEL, (WPARAM)&start, 0);
    lineno = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, start);
    *(short*)buffer = 512;
    lineno = SendMessage(hwnd, EM_GETLINE, lineno, (LPARAM)buffer);
    buffer[lineno] = 0;
    Findbump(hwnd, buffer);
}

//-------------------------------------------------------------------------

LRESULT CALLBACK findEditProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)

{
    char buffer[256];
    int pos, chars, line, i;
    LRESULT rv;
    POINTL point;

    CHARRANGE s;

    switch (iMessage)
    {
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
        case WM_CREATE:
            rv = CallWindowProc(oldproc, hwnd, iMessage, wParam, lParam);
            //         SendMessage(hwnd,EM_NOCOLORIZE,0,0) ;
            return rv;
        case WM_LBUTTONDOWN:
            SetFocus(hwnd);
            *(short*)buffer = 256;
            point.x = LOWORD(lParam);
            point.y = HIWORD(lParam);
            pos = SendMessage(hwnd, EM_CHARFROMPOS, 0, (LPARAM)&point);
            line = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, pos);
            i = SendMessage(hwnd, EM_GETLINE, line, (LPARAM)&buffer);
            chars = 0;  // richedit 1.0 was tacking things on the end...
            while (i-- && buffer[chars] > 31)
                chars++;
            pos = SendMessage(hwnd, EM_LINEINDEX, line, 0);
            s.cpMin = pos;
            s.cpMax = pos + chars;
            SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&s);
            SetFocus(hwnd);
            return 0;
        case WM_LBUTTONDBLCLK:
            FindBumpToEditor(hwnd);
            return 0;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
            return 0;

        case WM_KEYDOWN:
            switch (wParam)
            {
                case 'C':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        EditCopyText(hwnd);
                    }
                    break;
                case VK_CONTROL:
                    break;
                case VK_UP:
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    break;
                case VK_DOWN:
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                    break;
                case VK_PRIOR:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
                    break;
                case VK_NEXT:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
                    break;
                case VK_HOME:
                    SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
                    break;
                case VK_END:
                    SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                    break;
                default:
                    break;
            }
            return 0;
        case WM_MOUSEWHEEL:
            i = PropGetInt(NULL, "MOUSEWHEEL_SCROLL");
            if (i <= 0)
                i = 1;
            wheelIncrement -= GET_WHEEL_DELTA_WPARAM(wParam);
            if (wheelIncrement < 0)
            {
                while (wheelIncrement <= -WHEEL_DELTA)
                {
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    wheelIncrement += WHEEL_DELTA / i;
                }
            }
            else
            {
                while (wheelIncrement >= WHEEL_DELTA)
                {
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                    wheelIncrement -= WHEEL_DELTA / i;
                }
            }
            return 0;
        case WM_VSCROLL:
        {
            static int xlatch;
            // all this is meant to unlatch the scroll to bottom if you scroll up,
            // and relatch it if you scroll to the bottom
            switch (LOWORD(wParam))
            {
                case SB_ENDSCROLL:
                    xlatch = latch;
                    break;
                case SB_LINEUP:
                case SB_LINEDOWN:
                case SB_PAGEUP:
                case SB_PAGEDOWN:
                    xlatch = latch;
                case SB_THUMBTRACK:
                {
                    if (xlatch == latch)
                    {
                        SCROLLINFO scroll_info = {sizeof scroll_info, SIF_ALL};

                        GetScrollInfo(hwnd, SB_VERT, &scroll_info);
                        if (LOWORD(wParam) == SB_THUMBTRACK)
                            latch = scroll_info.nTrackPos + scroll_info.nPage >= scroll_info.nMax;
                        else
                            latch = scroll_info.nPos + scroll_info.nPage >= scroll_info.nMax;
                        if (latch != xlatch)
                        {
                            if (latch)
                            {
                                CallWindowProc(oldproc, hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                            }
                            else if (LOWORD(wParam) == SB_THUMBTRACK)
                            {
                                CallWindowProc(oldproc, hwnd, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, scroll_info.nTrackPos), 0);
                            }
                        }
                    }
                }
                break;
            }
        }
        break;
    }
    return CallWindowProc(oldproc, hwnd, iMessage, wParam, lParam);
}
//-------------------------------------------------------------------------

LRESULT CALLBACK findProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    FINDDATA* ptr;
    RECT r;
    CHARRANGE s;
    HDC dc;
    PAINTSTRUCT ps;
    switch (iMessage)
    {
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        case WM_LBUTTONDOWN:
            ptr = (FINDDATA*)GetWindowLong(hwnd, 0);
            return SendMessage(ptr->editor, iMessage, wParam, lParam);
        case WM_COMMAND:
            ptr = (FINDDATA*)GetWindowLong(hwnd, 0);
            switch (LOWORD(wParam))
            {
                case IDM_FIND:
                    SimpleSearchInTextBox(ptr->editor);
                    break;
                case IDM_GOTO:
                    FindBumpToEditor(ptr->editor);
                    break;
                case IDM_PREVBOOKMARK:
                    FindPrev(ptr->editor);
                    break;
                case IDM_NEXTBOOKMARK:
                    FindNext(ptr->editor);
                    break;
                case IDM_CLEAR:
                    if (hwnd == hwndFind1)
                        TagRemoveAll(TAG_FIF1);
                    else
                        TagRemoveAll(TAG_FIF2);
                    SendMessage(ptr->editor, WM_SETTEXT, 0, (LPARAM) "");
                    break;
            }
            break;
        case WM_VSCROLL:
            break;
        case WM_SETTEXT2:
            ptr = (FINDDATA*)GetWindowLong(hwnd, 0);
            if (!lParam)
            {
                SendMessage(ptr->editor, WM_SETTEXT, 0, (LPARAM) "");
                latch = TRUE;
            }
            else
            {
                int thumb = SendMessage(ptr->editor, EM_GETTHUMB, 0, 0);
                int lindex = SendMessage(ptr->editor, EM_LINEINDEX, 0x7fffffff, 0);
                s.cpMin = s.cpMax = lindex;
                SendMessage(ptr->editor, EM_EXSETSEL, 0, (LPARAM)&s);
                SendMessage(ptr->editor, EM_REPLACESEL, FALSE, lParam);
                SendMessage(ptr->editor, EM_SETSEL, -1, 0);
                if (!latch)
                {
                    SendMessage(ptr->editor, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, thumb), 0);
                }
            }
            SelectWindow(wParam);
            return 0;
        case WM_SETFOCUS:
            ptr = (FINDDATA*)GetWindowLong(hwnd, 0);
            PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            SetFocus(ptr->editor);
            return 0;
        case WM_CREATE:
            ptr = (FINDDATA*)calloc(sizeof(FINDDATA), 1);
            SetWindowLong(hwnd, 0, (long)ptr);
            GetClientRect(hwnd, &r);
            ptr->toolbar = CreateToolBarWindow(-1, hwnd, hwnd, ID_INFOTB, 5, infoButtons, infoHints, "Find Tools",
                                               0 /*IDH_ help hint */, TRUE);
            SendMessage(ptr->toolbar, LCF_FLOATINGTOOL, 0, 0);
            SetWindowPos(ptr->toolbar, NULL, 0, 3, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
            r.top += 25;
            ptr->editor = CreateWindowEx(
                0, "XFINDEDIT", 0, WS_CHILD + WS_VISIBLE + WS_VSCROLL + ES_NOHIDESEL + ES_LEFT + ES_MULTILINE + ES_READONLY, r.left,
                r.top, r.right - r.left, r.bottom - r.top, hwnd, (HMENU)ID_EDITCHILD, hInstance, 0);
            ApplyDialogFont(ptr->editor);

            SendMessage(ptr->editor, EM_LIMITTEXT, 4 * 65536, 0);
            return 0;
        case WM_CLOSE:
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
        {
            HPEN pen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            r.bottom = 30;
            pen = SelectObject(dc, pen);
            FillRect(dc, &r, (HBRUSH)(COLOR_BTNFACE + 1));
            MoveToEx(dc, r.left, r.bottom, NULL);
            LineTo(dc, r.right, r.bottom);
            pen = SelectObject(dc, pen);
            DeleteObject(pen);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            ptr = (FINDDATA*)GetWindowLong(hwnd, 0);
            DestroyWindow(ptr->toolbar);
            DestroyWindow(ptr->editor);
            break;
        case WM_SIZE:
            ptr = (FINDDATA*)GetWindowLong(hwnd, 0);
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 31;
            r.bottom = HIWORD(lParam);
            MoveWindow(ptr->editor, r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
            return 0;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterFindWindow(HINSTANCE hInstance)
{
    static BOOL registered;
    if (!registered)
    {
        WNDCLASS wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = 0;
        wc.lpfnWndProc = &findProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof(DWORD);
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        wc.lpszClassName = szFindClassName;
        RegisterClass(&wc);

        GetClassInfo(0, "richedit20a", &wc);
        oldproc = wc.lpfnWndProc;
        wc.lpfnWndProc = &findEditProc;
        wc.lpszClassName = "XFINDEDIT";
        wc.hInstance = hInstance;
        wc.hbrBackground = 0;
        RegisterClass(&wc);

        registered = TRUE;
    }
}

//-------------------------------------------------------------------------

HWND CreateFind1Window(void) { return hwndFind1 = CreateInternalWindow(DID_FINDWND, szFindClassName, "Find Results 1"); }
HWND CreateFind2Window(void) { return hwndFind2 = CreateInternalWindow(DID_FINDWND + 1, szFindClassName, "Find Results 2"); }
void SendFindMessage(BOOL second, char* buf)
{
    char* p;
    if (buf)
        p = strdup(buf);
    else
        p = buf;
    PostMessage(second ? hwndFind2 : hwndFind1, WM_SETTEXT2, second ? DID_FINDWND + 1 : DID_FINDWND, (LPARAM)p);
}
