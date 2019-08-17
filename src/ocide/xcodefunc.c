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

#define TRANSPARENT_COLOR 0x872395

extern HWND hwndFrame;
HWND hwndShowFunc;

static int GetFuncCount(CCFUNCDATA* funcs)
{
    int n = 0;
    while (funcs)
        n++, funcs = funcs->next;
    return n;
}
void GetArgData(char* proto, struct _ProtoData* arg)
{
    sprintf(proto, "%s", arg->fieldType);
    if (arg->fieldName[0] != '{' && strcmp(arg->fieldName, " "))  // unnamed...
    {
        strcat(proto, " ");
        unmangle(proto + strlen(proto), arg->fieldName);
    }
}
static BOOL GetFragments(CCFUNCDATA* funcs, int funcpos, int curarg, char** fragments)
{
    char buf[2048], *last = funcs->fullname, *p;
    int n = GetFuncCount(funcs);
    int i, nesting = 0;
    for (i = 0; i < funcpos && funcs; i++)
        funcs = funcs->next;
    if (!funcs)
        return FALSE;
    p = funcs->fullname;
    while (*p)
    {
        switch (*p)
        {
            case '@':
                if (!nesting)
                    last = p;
                break;
            case '#':
                nesting++;
                break;
            case '~':
                if (nesting)
                    nesting--;
                break;
        }
        p++;
    }
    if (last != p)
    {
        while (*last && *last != '$')
            last++;
        *last = 0;
    }
    sprintf(buf, "%d of %d", funcpos + 1, n);
    fragments[0] = strdup(buf);
    unmangle(buf, funcs->fullname);
    if (last != p)
        *last = '$';
    strcat(buf, "(");
    for (i = 0; i < curarg && i < funcs->args->argCount; i++)
    {
        GetArgData(buf + strlen(buf), &funcs->args->data[i]);
        if (i < funcs->args->argCount - 1)
            strcat(buf, ", ");
    }
    fragments[1] = strdup(buf);
    fragments[2] = NULL;
    if (i < funcs->args->argCount)
    {
        GetArgData(buf, &funcs->args->data[i]);
        if (i < funcs->args->argCount - 1)
            strcat(buf, ", ");
        i++;
        fragments[2] = strdup(buf);
    }
    fragments[3] = NULL;
    buf[0] = 0;
    for (; i < funcs->args->argCount; i++)
    {
        GetArgData(buf + strlen(buf), &funcs->args->data[i]);
        if (i < funcs->args->argCount - 1)
            strcat(buf, ", ");
    }
    strcat(buf, ")");
    fragments[3] = strdup(buf);
    return TRUE;
}
static void FreeFragments(char** fragments)
{
    int i;
    for (i = 0; i < 4; i++)
        free(fragments[i]);
}
static void SetFuncWindowSize(HWND hwnd, HFONT normal, HFONT bold, POINT origin, CCFUNCDATA* funcs, int funcpos, int curarg)
{
    char* fragments[4];
    if (GetFragments(funcs, funcpos, curarg, fragments))
    {
        HDC dc = GetDC(hwnd);
        int x = 0, y = 0;
        SIZE sz;
        normal = SelectObject(dc, normal);
        GetTextExtentPoint32(dc, fragments[0], strlen(fragments[0]), &sz);
        x += sz.cx;
        y = sz.cy;
        GetTextExtentPoint32(dc, fragments[1], strlen(fragments[1]), &sz);
        x += sz.cx;
        if (fragments[2])
        {
            normal = SelectObject(dc, normal);
            bold = SelectObject(dc, bold);
            GetTextExtentPoint32(dc, fragments[2], strlen(fragments[2]), &sz);
            x += sz.cx;
            y = sz.cy;
            bold = SelectObject(dc, bold);
            normal = SelectObject(dc, normal);
        }
        GetTextExtentPoint32(dc, fragments[3], strlen(fragments[3]), &sz);
        x += sz.cx;
        normal = SelectObject(dc, normal);
        ReleaseDC(hwnd, dc);

        x += 3 + 12 + 12 + 4 + 3 + GetSystemMetrics(SM_CXBORDER) * 2;
        y += GetSystemMetrics(SM_CYBORDER) * 2 + 3;
        MoveWindow(hwnd, origin.x, origin.y, x, y, TRUE);
        FreeFragments(fragments);
        InvalidateRect(hwnd, 0, 0);
    }
}
LRESULT CALLBACK funcProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static EDITDATA* ptr;
    static CCFUNCDATA** funcs[100];
    static int funcpos[100];
    static int nesting[100];
    static int curarg[100];
    static int index;
    static HFONT normal, bold, oldfont;
    static POINT origin[100];
    static HBITMAP leftarrow, rightarrow;
    int oldbk, oldfg;
    LOGFONT lf;
    PAINTSTRUCT ps;
    HPEN pen;
    HBRUSH brush;
    HDC dc;
    RECT r;
    char* fragments[4];
    switch (iMessage)
    {
        case WM_CREATE:
            leftarrow = LoadBitmap(hInstance, "ID_CCLEFTARR");
            rightarrow = LoadBitmap(hInstance, "ID_CCRIGHTARR");
            lf = systemDialogFont;
            normal = (HFONT)CreateFontIndirect(&lf);
            lf.lfWeight = FW_BOLD;
            bold = CreateFontIndirect(&lf);
            SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xff, LWA_COLORKEY);
            break;
        case WM_USER:
            funcs[index] = (CCFUNCDATA*)lParam;
            funcpos[index] = 0;
            nesting[index] = 0;
            curarg[index] = 0;
            index++;
            if (IsWindowVisible(hwnd))
                SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1], funcpos[index - 1], curarg[index - 1]);

            break;
        case WM_USER + 1:
        {
            POINTL pt;
            ptr = (EDITDATA*)lParam;
            posfromchar((HWND)wParam, ptr, &pt, ptr->selendcharpos);
            origin[index - 1].x = pt.x;
            origin[index - 1].y = pt.y;
            MapWindowPoints((HWND)wParam, hwndFrame, &origin[index - 1], 1);
            origin[index - 1].y += ptr->cd->txtFontHeight;
        }
        break;
        case WM_USER + 2:
            curarg[index - 1] = wParam;
            SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1], funcpos[index - 1], curarg[index - 1]);
            break;

        case WM_SHOWWINDOW:
        {
            if (wParam)
            {
                SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1], funcpos[index - 1], curarg[index - 1]);
            }
            else
            {
                ccFreeFunctionList(funcs[--index]);
                if (index)
                    ShowWindow(hwnd, SW_SHOW);
            }
        }
        case WM_USER + 3:
            if (IsWindowVisible(hwnd))
            {
                switch (wParam)
                {
                    case VK_DOWN:
                        if (GetFuncCount(funcs[index - 1]) > 1)
                        {
                            funcpos[index - 1]++;
                            if (funcpos[index - 1] >= GetFuncCount(funcs[index - 1]))
                                funcpos[index - 1] = 0;
                            SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1], funcpos[index - 1],
                                              curarg[index - 1]);

                            return 0;
                        }
                    case VK_UP:
                        if (GetFuncCount(funcs[index - 1]) > 1)
                        {
                            funcpos[index - 1]--;
                            if (funcpos[index - 1] < 0)
                                funcpos[index - 1] = GetFuncCount(funcs[index - 1]);
                            SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1], funcpos[index - 1],
                                              curarg[index - 1]);
                            return 0;
                        }
                    case VK_HOME:
                    case VK_END:
                    case VK_PRIOR:
                    case VK_NEXT:
                        if (index == 1)
                            ShowWindow(hwnd, SW_HIDE);
                        else
                        {
                            ccFreeFunctionList(funcs[--index]);
                            SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1], funcpos[index - 1],
                                              curarg[index - 1]);
                        }
                        return 1;
                    case VK_BACK:
                        if (ptr->selendcharpos)
                            switch (ptr->cd->text[ptr->selendcharpos - 1].ch)
                            {
                                case ',':
                                    --curarg[index - 1];
                                    SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1], funcpos[index - 1],
                                                      curarg[index - 1]);
                                    return 1;
                                case '(':
                                    if (--nesting[index - 1] < 0)
                                    {
                                        if (index == 1)
                                            ShowWindow(hwnd, SW_HIDE);
                                        else
                                        {
                                            ccFreeFunctionList(funcs[--index]);
                                            SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1],
                                                              funcpos[index - 1], curarg[index - 1]);
                                        }
                                    }
                                    break;
                                case ')':
                                    ++nesting[index - 1];
                                    break;
                            }
                        return 1;
                        break;
                    default:
                    {
                        int key = KeyboardToAscii(wParam, lParam, TRUE);
                        switch (key)
                        {
                            case ',':
                                ++curarg[index - 1];
                                SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1], funcpos[index - 1],
                                                  curarg[index - 1]);
                                break;
                            case '(':
                                ++nesting[index - 1];
                                break;
                            case ')':
                                if (--nesting[index - 1] < 0)
                                {
                                    if (index == 1)
                                        ShowWindow(hwnd, SW_HIDE);
                                    else
                                    {
                                        ccFreeFunctionList(funcs[--index]);
                                        SetFuncWindowSize(hwnd, normal, bold, origin[index - 1], funcs[index - 1],
                                                          funcpos[index - 1], curarg[index - 1]);
                                    }
                                }
                                break;
                        }
                        return 1;
                        break;
                    }
                }
            }
            break;
        case WM_PAINT:
            dc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &r);
            brush = CreateSolidBrush(TRANSPARENT_COLOR);
            FillRect(dc, &r, brush);
            DeleteObject(brush);

            brush = CreateSolidBrush(RetrieveSysColor(COLOR_INFOBK));
            brush = SelectObject(dc, brush);
            pen = CreatePen(PS_SOLID, 0, 0);
            pen = SelectObject(dc, pen);
            RoundRect(dc, r.left, r.top, r.right, r.bottom, 5, 5);
            brush = SelectObject(dc, brush);
            DeleteObject(brush);
            pen = SelectObject(dc, pen);
            DeleteObject(pen);

            if (GetFragments(funcs[index - 1], funcpos[index - 1], curarg[index - 1], fragments))
            {
                SIZE sz;
                HDC* memDC = CreateCompatibleDC(dc);
                int offset = 3 + GetSystemMetrics(SM_CXBORDER);
                normal = SelectObject(dc, normal);
                oldbk = SetBkColor(dc, RetrieveSysColor(COLOR_INFOBK));
                oldfg = SetTextColor(dc, RetrieveSysColor(COLOR_INFOTEXT));
                GetTextExtentPoint32(dc, fragments[0], strlen(fragments[0]), &sz);
                TextOut(dc, offset + 12, 2, fragments[0], strlen(fragments[0]));

                leftarrow = SelectObject(memDC, leftarrow);
                TransparentBlt(dc, offset + 2, ((sz.cy + 3) - 10) / 2, 10, 10, memDC, 0, 0, 10, 10, 0xc0c0c0);
                offset += 12 + sz.cx;
                leftarrow = SelectObject(memDC, leftarrow);
                rightarrow = SelectObject(memDC, rightarrow);
                TransparentBlt(dc, offset + 2, ((sz.cy + 3) - 10) / 2, 10, 10, memDC, 0, 0, 10, 10, 0xc0c0c0);
                rightarrow = SelectObject(memDC, rightarrow);
                offset += 12 + 4;
                GetTextExtentPoint32(dc, fragments[1], strlen(fragments[1]), &sz);
                TextOut(dc, offset, 2, fragments[1], strlen(fragments[1]));
                offset += sz.cx;
                if (fragments[2])
                {
                    normal = SelectObject(dc, normal);
                    bold = SelectObject(dc, bold);
                    GetTextExtentPoint32(dc, fragments[2], strlen(fragments[2]), &sz);
                    TextOut(dc, offset, 2, fragments[2], strlen(fragments[2]));
                    offset += sz.cx;
                    bold = SelectObject(dc, bold);
                    normal = SelectObject(dc, normal);
                }
                GetTextExtentPoint32(dc, fragments[3], strlen(fragments[3]), &sz);
                TextOut(dc, offset, 2, fragments[3], strlen(fragments[3]));
                offset += sz.cx;

                DeleteDC(memDC);
                normal = SelectObject(dc, normal);
                oldbk = SetBkColor(dc, oldbk);
                oldfg = SetTextColor(dc, oldfg);
                FreeFragments(fragments);
            }
            EndPaint(hwnd, &ps);
            return 0;
        case WM_CLOSE:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
void showFunction(HWND hwnd, EDITDATA* p, int ch)
{
    CCFUNCDATA* functionData = NULL;
    char name[2046], *q = name;
    int end = p->selstartcharpos;
    int pos = p->selstartcharpos - 1;
    int curArg = 0;
    if (instring(p->cd->text, &p->cd->text[p->selstartcharpos]))
        return;
    p->cd->selecting = FALSE;

    if (pos <= 0 || PropGetInt(NULL, "CODE_COMPLETION") == 0)
        return;
    if (ch == '(' || (ch == ',' && !IsWindowVisible(hwndShowFunc)))
    {
        char name[512], *p1;
        int pos;
        int i;
        int commaCount = 0;
        DWINFO* info = (DWINFO*)GetWindowLong(GetParent(hwnd), 0);
        int lineno = SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, p->selstartcharpos) + 1;
        CCFUNCDATA* functionData = NULL;
        BOOL parsed = FALSE;
        CHARRANGE range;
        if (ch == ',')
        {
            int nesting = 1;
            pos = p->selendcharpos - 1;
            while (nesting > 0 && pos > 0 && p->cd->text[pos].ch != '{' && p->cd->text[pos].ch != '}')
            {
                if (p->cd->text[pos].ch == ')')
                    nesting++;
                else if (p->cd->text[pos].ch == '(')
                    nesting--;
                else if (p->cd->text[pos].ch == ',' && nesting == 1)
                    commaCount++;
                pos--;
            }
            if (nesting)
                return;
            if (!pos || (!isalnum(p->cd->text[pos - 1].ch) && p->cd->text[pos - 1].ch != '_'))
                return;
            range.cpMin = pos;
            range.cpMax = pos;
        }
        else
        {
            range.cpMin = p->selendcharpos - 1;
            range.cpMax = p->selendcharpos - 1;
        }
        GetWordSpan(p, &range);  // undefined in local context
        if (range.cpMin == range.cpMax)
            return;
        pos = range.cpMin;
        while (pos && isspace(p->cd->text[pos - 1].ch))
        {
            pos--;
        }
        if (pos && p->cd->text[pos - 1].ch == ':')
        {
            pos = CPPScanBackward(p, range.cpMax, TRUE);  // undefined in local context
        }
        else
        {
            pos = range.cpMin;
        }
        for (i = pos; i < range.cpMax; i++)
            name[i - pos] = p->cd->text[i].ch;
        name[i - pos] = 0;
        p1 = name;
        GetQualifiedName(name + i - pos + 1, &p1, FALSE, FALSE);
        strcpy(name, name + i - pos + 1);
        while (pos && isspace(p->cd->text[pos - 1].ch))
        {
            pos--;
        }
        if (pos && (p->cd->text[pos - 1].ch == '.' || p->cd->text[pos - 1].ch == '>'))
        {
            int start = 0;
            start = CPPScanBackward(p, p->selendcharpos, TRUE);
            if (start != pos)
            {
                char qual[2048];
                for (i = start; i < pos; i++)
                    qual[i - start] = p->cd->text[i].ch;
                qual[i - start] = 0;
                parsed = TRUE;
                sprintf(qual + strlen(qual), "@%s", name);
                functionData = ccLookupFunctionList(lineno, info->dwName, qual);
            }
        }
        if (!parsed)
        {
            CCFUNCDATA** scan;
            char funcbase[2048], nsbase[512], abase[512];
            abase[0] = 0;
            GetContainerData(lineno, info->dwName, nsbase, funcbase);
            if (strrchr(name, '@') != name)
            {
                char* p = strrchr(name, '@');
                strncpy(abase, name, p - name);
                abase[p - name] = 0;
                strcpy(name, p);
                *p = 0;
            }
            functionData = ccLookupFunctionList(lineno, info->dwName, name);
            // try again if not found, looking for 'C' version
            if (!functionData)
            {
                name[0] = '_';
                functionData = ccLookupFunctionList(lineno, info->dwName, name);
                name[0] = '@';
            }
            // one last try, looking for 'stdcall' version
            if (!functionData)
            {
                functionData = ccLookupFunctionList(lineno, info->dwName, name + 1);
            }
            scan = &functionData;
            while (*scan)
            {
                char nsbase2[2048];
                if ((*scan)->args->member)
                {
                    if (!funcbase[0] || strncmp(funcbase, (*scan)->fullname, strlen(funcbase)))
                    {
                        CCFUNCDATA* remove = *scan;
                        *scan = (*scan)->next;
                        remove->next = NULL;
                        ccFreeFunctionList(remove);
                    }
                    else
                    {
                        scan = &(*scan)->next;
                    }
                }
                else
                {
                    char *last, *p;
                    int nesting = 0;
                    p = last = (*scan)->fullname;
                    while (*p)
                    {
                        switch (*p)
                        {
                            case '@':
                                if (!nesting)
                                    last = p;
                                break;
                            case '#':
                                nesting++;
                                break;
                            case '~':
                                if (nesting)
                                    nesting--;
                                break;
                        }
                        p++;
                    }
                    if (last != (*scan)->fullname &&
                        (strncmp((*scan)->fullname, funcbase, last - (*scan)->fullname) ||
                         (funcbase[last - (*scan)->fullname] != '@' && funcbase[last - (*scan)->fullname] != 0)))
                    {
                        BOOL found = FALSE;
                        char* p = nsbase;
                        while (*p)
                        {
                            char* q = nsbase2;
                            while (*p && *p != ';')
                                *q++ = *p++;
                            if (*p)
                                p++;
                            *q = 0;
                            if (!strncmp((*scan)->fullname, nsbase2, last - (*scan)->fullname) &&
                                (nsbase2[last - (*scan)->fullname] == '@' || nsbase2[last - (*scan)->fullname] == 0))
                            {
                                found = TRUE;
                                break;
                            }
                        }
                        if (!found)
                        {
                            strcpy(nsbase2, abase);
                            if (!strncmp((*scan)->fullname, nsbase2, last - (*scan)->fullname) &&
                                nsbase2[last - (*scan)->fullname] == 0)
                            {
                                found = TRUE;
                            }
                        }
                        if (!found)
                        {
                            CCFUNCDATA* remove = *scan;
                            *scan = (*scan)->next;
                            remove->next = NULL;
                            ccFreeFunctionList(remove);
                        }
                        else
                            scan = &(*scan)->next;
                    }
                    else
                    {
                        BOOL found = abase[0] == '\0';

                        strcpy(nsbase2, abase);
                        if (!strncmp((*scan)->fullname, nsbase2, last - (*scan)->fullname) &&
                            nsbase2[last - (*scan)->fullname] == 0)
                        {
                            found = TRUE;
                        }
                        if (!found)
                        {
                            CCFUNCDATA* remove = *scan;
                            *scan = (*scan)->next;
                            remove->next = NULL;
                            ccFreeFunctionList(remove);
                        }
                        else
                        {
                            scan = &(*scan)->next;
                        }
                    }
                }
            }
        }
        if (functionData)
        {
            if (!hwndShowFunc)
            {
                hwndShowFunc =
                    CreateWindowEx(0 | /*WS_EX_TOPMOST | WS_EX_LAYERED |*/ WS_EX_NOACTIVATE, "xccfuncclass", "", (WS_CHILD),
                                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndFrame, 0, GetModuleHandle(0), 0);
                // done this way to associate the popup with the application rather than the desktop
                SetWindowLong(hwndShowFunc, GWL_STYLE, (GetWindowLong(hwndShowFunc, GWL_STYLE) & ~WS_CHILD) | WS_POPUP);
            }
            SendMessage(hwndShowFunc, WM_USER, 0, (LPARAM)functionData);
            SendMessage(hwndShowFunc, WM_USER + 1, (WPARAM)hwnd, (LPARAM)p);
            SendMessage(hwndShowFunc, WM_USER + 2, commaCount, 0);
            ShowWindow(hwndShowFunc, SW_SHOW);
            SetFocus(hwnd);
        }
    }
}
