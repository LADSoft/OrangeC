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

int latch;

int simpleFindFlags = FR_DOWN;

static HWND hwndInfo;
static char szInfoClassName[] = "xccInfoClass";
static WNDPROC oldproc;
static HWND hwndCtrl;
static int index;
static int wheelIncrement;
static HWND combo;
static HWND label;
static char* nameTags[] = {"Build", "Extended Build", "Debug"};
TBBUTTON infoButtons[] = {{0, IDM_GOTO, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON},
                          {0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT},
                          {1, IDM_PREVBOOKMARK, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON},
                          {2, IDM_NEXTBOOKMARK, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON},
                          {0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT},
                          {4, IDM_FIND, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON},
                          {0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT},
                          {3, IDM_CLEAR, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON},
                          {0, 0, 0, 0}};
char* infoHints[] = {"Goto info", "", "Previous", "Next", "", "Find In Window", "", "Clear window"};

static HWND toolbar;
static char* szInfoTitle = "Information";

void GetSimpleCharRange(HWND hwnd, int flags, FINDTEXT* findText)
{
    CHARRANGE pos;
    int len = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
    SendMessage(hwnd, EM_EXGETSEL, 0, (LPARAM)&pos);
    if (pos.cpMin == pos.cpMax)
        pos.cpMin = pos.cpMax = 0;
    if (flags & FR_DOWN)
    {
        findText->chrg.cpMin = pos.cpMax;
        findText->chrg.cpMax = len;
    }
    else
    {
        findText->chrg.cpMin = pos.cpMin;
        findText->chrg.cpMax = 0;
    }
}
LRESULT CALLBACK SimpleFindDialogProc(HWND hwndDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndEdit;
    switch (iMessage)
    {
        // IDC_COMBOFINDFIND
        case WM_INITDIALOG:
            CenterWindow(hwndDlg);
            hwndEdit = (HWND)lParam;
            CheckDlgButton(hwndDlg, IDC_CHECKFINDCASE, simpleFindFlags & FR_MATCHCASE ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hwndDlg, IDC_CHECKFINDWHOLE, simpleFindFlags & FR_WHOLEWORD ? BST_CHECKED : BST_UNCHECKED);
            CheckRadioButton(hwndDlg, IDC_CHECKFINDUP, IDC_CHECKFINDDOWN,
                             simpleFindFlags & FR_DOWN ? IDC_CHECKFINDDOWN : IDC_CHECKFINDUP);
            return TRUE;
        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                {
                    int n = 0;
                    FINDTEXT findText;
                    char buf[256];
                    GetDlgItemText(hwndDlg, IDC_COMBOFINDFIND, buf, sizeof(buf));
                    if (IsDlgButtonChecked(hwndDlg, IDC_CHECKFINDCASE) == BST_CHECKED)
                        n |= FR_MATCHCASE;
                    if (IsDlgButtonChecked(hwndDlg, IDC_CHECKFINDWHOLE) == BST_CHECKED)
                        n |= FR_WHOLEWORD;
                    if (IsDlgButtonChecked(hwndDlg, IDC_CHECKFINDDOWN) == BST_CHECKED)
                        n |= FR_DOWN;
                    simpleFindFlags = n;
                    GetSimpleCharRange(hwndEdit, simpleFindFlags, &findText);
                    findText.lpstrText = buf;
                    n = SendMessage(hwndEdit, EM_FINDTEXT, simpleFindFlags, (LPARAM)&findText);
                    if (n == -1)
                    {
                        MessageBeep(0);
                    }
                    else
                    {
                        findText.chrg.cpMin = n;
                        findText.chrg.cpMax = n + strlen(buf);
                        SendMessage(hwndEdit, EM_EXSETSEL, 0, (LPARAM)&findText.chrg);
                        SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0);
                    }
                    break;
                }
                case IDCANCEL:
                    EndDialog(hwndDlg, 0);
                    break;
            }
    }
    return 0;
}
void SimpleSearchInTextBox(HWND hwndEdit)
{
    CreateDialogParam(hInstance, "IDD_SIMPLEFIND", hwndFrame, (DLGPROC)SimpleFindDialogProc, (LPARAM)hwndEdit);
}
int getfile(char* start, char* buffer, char end, DWINFO* info)
{
    char *t = buffer, *q;
    int rv;
    rv = atoi(++buffer);
    while (*buffer && isdigit(*buffer))
        buffer++;
    if (*buffer == end)
    {
        /* complexity is to allow white space in file and path names */
        q = strchr(start, ':');
        if (q && q != start)
        {
            char* r = strchr(start, '.');
            if (r && r < q)
            {
                q = r;
                while (q != start && isalnum(q[-1]))
                    q--;
            }
            else
                q--;
        }
        else
        {
            q = strchr(start, '\\');
            if (!q)
            {
                q = t;
                while (q > start)
                {
                    if (*(q - 1) == ' ')
                        break;
                    q--;
                }
            }
            else
            {
                char* r = strchr(start, '.');
                if (r && r < q)
                {
                    q = r;
                    while (q != start && isalnum(q[-1]))
                        q--;
                }
            }
        }
        if (t - q > sizeof(info->dwName) - 1)
            t = q + sizeof(info->dwName) - 1;
        strncpy(info->dwName, q, t - q);
        info->dwName[t - q] = 0;
        q = strrchr(info->dwName, '\\');
        if (q)
            strcpy(info->dwTitle, q + 1);
        else
            strcpy(info->dwTitle, info->dwName);
        return rv;
    }
    return -1;
}
void TextToClipBoard(HWND hwnd, char* text)
{
    if (OpenClipboard(hwnd))
    {
        int n = 0;
        char* p = text;
        int l = strlen(text);
        HGLOBAL glmem;
        while ((p = strchr(p, '\n')))
            n++, p++;
        glmem = GlobalAlloc(GMEM_DDESHARE + GMEM_MOVEABLE, l + n + 1);
        if (glmem != NULL)
        {
            char *data = GlobalLock(glmem), *q = data;
            for (p = text; *p;)
            {
                if (*p == '\n')
                {
                    *q++ = '\r';
                    *q++ = *p++;
                }
                else if (*p == '\r')
                {
                    *q++ = *p++;
                    *q++ = '\n';
                    if (*p == '\n')
                        p++;
                }
                else
                {
                    *q++ = *p++;
                }
            }
            *q++ = 0;
            GlobalUnlock(data);
            EmptyClipboard();
            SetClipboardData(CF_TEXT, glmem);
        }
        CloseClipboard();
    }
}
//-------------------------------------------------------------------------

static BOOL bump(HWND hwnd, char* buffer)
{
    DWINFO info;
    char *t, *q;
    int lineno;
    int start;
    lineno = -1;
    if ((t = strrchr(buffer, '(')) && isdigit(*(t + 1)))
    {
        lineno = getfile(buffer, t, ')', &info);
    }
    else
    {
        t = buffer;
        while (1)
        {
            if ((t = strchr(t + 1, ':')))
            {
                if (isdigit(*(t + 1)))
                {
                    lineno = getfile(buffer, t, ':', &info);
                    if (lineno != -1)
                        break;
                }
            }
            else
                break;
        }
    }
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
static void Prev(HWND hwnd)
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
        if ((found = bump(hwnd, buffer)))
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
static void Next(HWND hwnd)
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
        if ((found = bump(hwnd, buffer)))
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
static void BumpToEditor(HWND hwnd)
{
    char buffer[1024];
    int lineno;
    int start;
    SendMessage(hwnd, EM_GETSEL, (WPARAM)&start, 0);
    lineno = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, start);
    *(short*)buffer = 512;
    lineno = SendMessage(hwnd, EM_GETLINE, lineno, (LPARAM)buffer);
    buffer[lineno] = 0;
    bump(hwnd, buffer);
}
static void BumpToErrorWnd(char* buffer)
{
    DWINFO info;
    char *t, *q, *err = NULL;
    int lineno = -1;
    int start;
    if ((t = strrchr(buffer, '(')) && isdigit(*(t + 1)))
    {
        lineno = getfile(buffer, t, ')', &info);
        err = strchr(t, ')');
    }
    else
    {
        t = buffer;
        while (1)
        {
            if ((t = strchr(t + 1, ':')))
            {
                if (isdigit(*(t + 1)))
                {
                    lineno = getfile(buffer, t, ':', &info);
                    if (lineno != -1)
                    {
                        err = strchr(t, ':');
                        break;
                    }
                }
            }
            else
                break;
        }
    }
    if (lineno == -1)
    {
        if (strstr(buffer, "in module"))
        {
            strncpy(info.dwName, strstr(buffer, "in module" + 9), sizeof(info.dwName) - 1);
            info.dwName[sizeof(info.dwName) - 1] = 0;
            strstr(buffer, "in module")[0] = 0;
            err = strchr(buffer, ':');
            lineno = -2;
        }
    }
    if (lineno != -1 && err)
    {
        ERRWNDDATA* p = calloc(1, sizeof(ERRWNDDATA));
        p->isWarning = TRUE;
        if (strnicmp(buffer, "error", 5) == 0)
            p->isWarning = FALSE;
        p->lineno = lineno;
        while (*err && !isspace(*err))
            err++;
        while (isspace(*err))
            err++;
        strcpy(p->error, err);
        strcpy(p->file, info.dwName);
        PostDIDMessage(DID_ERRWND, WM_SETERRDATA, 0, (LPARAM)p);
    }
}

//-------------------------------------------------------------------------

void EditCopyText(HWND hwnd)
{
    char* p;
    int l;
    l = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
    p = malloc(l + 1);
    if (p)
    {
        SendMessage(hwnd, WM_GETTEXT, l + 1, (LPARAM)p);
        TextToClipBoard(hwnd, p);
        free(p);
    }
}
LRESULT CALLBACK buildEditProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)

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
        case WM_RBUTTONDOWN:
        {
            SetFocus(hwnd);
            if (index == 0)
            {
                HMENU menu = LoadMenuGeneric(hInstance, "BUILDMENU");
                HMENU popup = GetSubMenu(menu, 0);
                POINT pos;
                GetCursorPos(&pos);
                InsertBitmapsInMenu(popup);
                TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x, pos.y, hwndFrame, NULL);
                DestroyMenu(menu);
            }
        }
        break;
        case WM_LBUTTONDBLCLK:
            BumpToEditor(hwnd);
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

LRESULT CALLBACK infoProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static HWND hsubwnds[3];  // build, extended build, debug
    CHARFORMAT cfm;
    NMHDR* h;
    RECT r;
    CHARRANGE s;
    HDC dc;
    PAINTSTRUCT ps;
    HPEN pen;
    int i;
    static int colors[2];
    static int forecolor;
    switch (iMessage)
    {
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        case WM_LBUTTONDOWN:
            return SendMessage(hsubwnds[index], iMessage, wParam, lParam);
        case WM_COMMAND:
            switch (HIWORD(wParam))
            {
                case CBN_SELCHANGE:
                {
                    int newindex = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
                    if (newindex != CB_ERR)
                    {
                        ShowWindow(hsubwnds[index], SW_HIDE);
                        index = newindex;
                        ShowWindow(hsubwnds[index], SW_SHOW);
                    }
                    return 0;
                    break;
                }
            }
            switch (LOWORD(wParam))
            {
                case IDM_FIND:
                    SimpleSearchInTextBox(hsubwnds[index]);
                    break;
                case IDM_GOTO:
                    BumpToEditor(hsubwnds[index]);
                    break;
                case IDM_PREVBOOKMARK:
                    Prev(hsubwnds[index]);
                    break;
                case IDM_NEXTBOOKMARK:
                    Next(hsubwnds[index]);
                    break;
                case IDM_CLEAR:
                    SendMessage(hsubwnds[index], WM_SETTEXT, 0, (LPARAM) "");
                    break;
            }
            break;
        case WM_VSCROLL:
            break;
        case WM_SETCOLOR:
            colors[wParam] = lParam;
            break;
        case WM_SETTEXT2:
            if (!lParam)
            {
                if (wParam == 0)
                    PostDIDMessage(DID_ERRWND, WM_CLEARERRDATA, 0, 0);
                SendMessage(hsubwnds[wParam], WM_SETTEXT, 0, (LPARAM) "");
                latch = TRUE;
            }
            else
            {
                int thumb = SendMessage(hsubwnds[wParam], EM_GETTHUMB, 0, 0);
                int lindex = SendMessage(hsubwnds[wParam], EM_LINEINDEX, 0x7fffffff, 0);
                s.cpMin = s.cpMax = lindex;
                SendMessage(hsubwnds[wParam], EM_EXSETSEL, 0, (LPARAM)&s);
                memset(&cfm, 0, sizeof(cfm));
                cfm.cbSize = sizeof(cfm);
                cfm.dwMask = CFM_COLOR;
                cfm.crTextColor = colors[wParam];
                SendMessage(hsubwnds[wParam], EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfm);
                SendMessage(hsubwnds[wParam], EM_REPLACESEL, FALSE, lParam);
                SendMessage(hsubwnds[wParam], EM_SETSEL, -1, 0);
                if (!latch)
                {
                    SendMessage(hsubwnds[wParam], WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, thumb), 0);
                }
                if (wParam == 0)
                {
                    BumpToErrorWnd((char*)lParam);
                }
            }
            colors[wParam] = RetrieveSysColor(COLOR_WINDOWTEXT);
            return 0;
        case WM_SETFOCUS:
            PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            SetFocus(hsubwnds[index]);
            return 0;
        case WM_CREATE:
            hwndInfo = hwnd;
            GetClientRect(hwnd, &r);
            label =
                CreateWindow("STATIC", "Select information to view:", WS_CHILD | WS_VISIBLE, 5, 7, 150, 16, hwnd, 0, hInstance, 0);
            combo =
                CreateWindowEx(WS_EX_NOACTIVATE, "COMBOBOX", 0, WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL,
                               160, 4, 100, 100, hwnd, 0, hInstance, 0);
            toolbar = CreateToolBarWindow(-1, hwnd, hwnd, ID_INFOTB, 5, infoButtons, infoHints, "Info Tools", 0 /*IDH_ help hint */,
                                          TRUE);
            SendMessage(toolbar, LCF_FLOATINGTOOL, 0, 0);
            SetWindowPos(toolbar, NULL, 250, 3, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
            SubClassNECombo(combo);
            ApplyDialogFont(label);
            ApplyDialogFont(combo);
            r.top += 25;
            hsubwnds[0] = CreateWindowEx(
                0, "XBUILDEDIT", 0, WS_CHILD + WS_VISIBLE + WS_VSCROLL + ES_NOHIDESEL + ES_LEFT + ES_MULTILINE + ES_READONLY,
                r.left, r.top, r.right - r.left, r.bottom - r.top, hwnd, (HMENU)ID_EDITCHILD, hInstance, 0);
            hsubwnds[1] =
                CreateWindowEx(0, "XBUILDEDIT", 0, WS_CHILD + WS_VSCROLL + ES_NOHIDESEL + ES_LEFT + ES_MULTILINE + ES_READONLY,
                               r.left, r.top, r.right - r.left, r.bottom - r.top, hwnd, (HMENU)ID_EDITCHILD, hInstance, 0);
            hsubwnds[2] =
                CreateWindowEx(0, "XBUILDEDIT", 0, WS_CHILD + WS_VSCROLL + ES_NOHIDESEL + ES_LEFT + ES_MULTILINE + ES_READONLY,
                               r.left, r.top, r.right - r.left, r.bottom - r.top, hwnd, (HMENU)ID_EDITCHILD, hInstance, 0);

            SendMessage(hsubwnds[0], EM_LIMITTEXT, 4 * 65536, 0);
            SendMessage(hsubwnds[1], EM_LIMITTEXT, 4 * 65536, 0);
            ApplyDialogFont(hsubwnds[0]);
            ApplyDialogFont(hsubwnds[1]);
            for (i = 0; i < 3; i++)
                SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)nameTags[i]);
            SendMessage(combo, CB_SETCURSEL, index, 0);
            colors[0] = colors[1] = RetrieveSysColor(COLOR_WINDOWTEXT);
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
            FillRect(dc, &r, (HBRUSH)(COLOR_BTNFACE + 1));
            pen = SelectObject(dc, pen);
            MoveToEx(dc, r.left, r.bottom, NULL);
            LineTo(dc, r.right, r.bottom);
            pen = SelectObject(dc, pen);
            DeleteObject(pen);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            DestroyWindow(combo);
            DestroyWindow(label);
            DestroyWindow(toolbar);
            DestroyWindow(hsubwnds[0]);
            DestroyWindow(hsubwnds[1]);
            DestroyWindow(hsubwnds[2]);
            DestroyWindow(hwndCtrl);
            break;
        case WM_SELERRWINDOW:
            ShowWindow(hsubwnds[index], SW_HIDE);
            index = (int)lParam;
            ShowWindow(hsubwnds[index], SW_SHOW);
            SendMessage(combo, CB_SETCURSEL, index, 0);
            SendMessage(hsubwnds[index], WM_SETTEXT, index, (LPARAM) "");
            if (index == 0)
                PostDIDMessage(DID_ERRWND, WM_CLEARERRDATA, 0, 0);
            latch = TRUE;
            break;
        case WM_SIZE:
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 31;
            r.bottom = HIWORD(lParam);
            MoveWindow(hsubwnds[0], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
            MoveWindow(hsubwnds[1], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
            MoveWindow(hsubwnds[2], r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
            return 0;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterInfoWindow(HINSTANCE hInstance)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = 0;
    wc.lpfnWndProc = &infoProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = szInfoClassName;
    RegisterClass(&wc);

    GetClassInfo(0, "richedit20a", &wc);
    oldproc = wc.lpfnWndProc;
    wc.lpfnWndProc = &buildEditProc;
    wc.lpszClassName = "XBUILDEDIT";
    wc.hInstance = hInstance;
    wc.hbrBackground = 0;
    RegisterClass(&wc);
}

//-------------------------------------------------------------------------

HWND CreateInfoWindow(void)
{
    if (!hwndInfo)
    {
        hwndInfo = CreateInternalWindow(DID_INFOWND, szInfoClassName, szInfoTitle);
    }
    return hwndInfo;
}
void SetInfoColor(int window, DWORD color) { PostMessage(hwndInfo, WM_SETCOLOR, window, color); }
void SelectInfoWindow(int window)
{
    PostMessage(hwndInfo, WM_SELERRWINDOW, 0, window);
    SetFocus(hwndInfo);
    //    PostMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)hwndInfo, 0);
}
void SendInfoMessage(int window, char* buf)
{
    if (window != ERR_NO_WINDOW)
    {
        char* p;
        if (buf)
            p = strdup(buf);
        else
            p = buf;
        PostMessage(hwndInfo, WM_SETTEXT2, window, (LPARAM)p);
    }
}
