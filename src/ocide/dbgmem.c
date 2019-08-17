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

#include "header.h"
#include "operands.h"
#include "opcodes.h"
#include <ctype.h>

extern HINSTANCE hInstance;
extern HWND hwndClient, hwndFrame;
extern PROCESS* activeProcess;
extern enum DebugState uState;
extern THREAD* activeThread;
extern PROJECTITEM* workArea;
extern SCOPE* activeScope;
extern LOGFONT systemDialogFont;

char* memhist[MAX_COMBO_HISTORY];

int memoryWordSize = 1;

static char szMemClassName[] = "xccMemoryClass";
static char szMemInternalClass[] = "xccInternalMemoryClass";
static WNDPROC oldproc;
static HBRUSH hbrBackground;

TBBUTTON memButtons[] = {{0, IDM_GOTO, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON}, {0, 0, 0, 0}};
char* memHints[] = {"Auto Evaluate"};

typedef struct memdata
{
    HWND hwndCombo, hwndEdit, hwndStatic;
    HWND hwnd, hwndChild;
    HWND toolbar;
    BYTE* lastMem;
    int lastMemSize;
    int oldMemAddress;
    int MemAddress;
    int cursrow, curscol;
    char currentSel[256];
    char name[256];
    int curpos;
    int modifying;
    int focussed;
    int length;
    int wordSize;
    int autoeval;
    char cursmod[32];
    BYTE* currentData;
    int currentLen;
    BYTE* lastData;
    int lastLen;
} MEMDATA;

static LOGFONT Normalfontdata = {-12,
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
                                 FF_MODERN | FF_DONTCARE,
                                 CONTROL_FONT};

static HFONT MemFont;
LRESULT CALLBACK historyComboProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

static void SetCursMode(MEMDATA* ptr)
{
    switch ((memoryWordSize & 0xffff))
    {
        case 1:
            strcpy(ptr->cursmod, "  ");
            break;
        case 2:
            strcpy(ptr->cursmod, "    ");
            break;
        case 4:
            strcpy(ptr->cursmod, "        ");
            break;
    }
}
static int GetMemAddress(char* buf, BOOL error)
{
    int addr;
    int stars = 0;
    while (*buf == ' ')
        buf++;
    if (*buf == '&')
        buf++;
    {
        DEBUG_INFO* dbg;
        VARINFO* var;
        var = EvalExpr(&dbg, activeScope, buf, FALSE);
        if (var)
        {
            if (var->constant)
                addr = var->ival;
            else if (var->address < 0x1000)
            {
                char data[20];
                var->thread = activeThread;
                ReadValue(var->address, &data, 4, var);
                addr = *(int*)data;
            }
            else
                addr = var->address;
            FreeVarInfo(var);
        }
        else
        {
            if (error)
                MessageBeep(MB_ICONASTERISK);
            addr = 0;
        }
    }

    return addr;
}

//-------------------------------------------------------------------------

static void MemCopyText(HWND hwnd, MEMDATA* ptr)
{
    int i;
    char buf[1024];
    char charbuf[1000];
    RECT rect;
    int lines;
    int chars;
    char* p;
    memset(charbuf, 0, sizeof(charbuf));
    GetClientRect(hwnd, &rect);
    rect.top += 22;
    lines = (rect.bottom - rect.top) / 16;
    chars = (ptr->wordSize >> 16);
    if (!chars)
        chars = (rect.right / 8 - 10) / 4;

    p = malloc(lines * 2048 + 1);
    if (p)
    {
        p[0] = 0;
        for (i = 0; i < lines; i++)
        {
            int j;
            sprintf(buf, "%08X: ", ptr->MemAddress + i * chars);
            strcat(p, buf);
            for (j = 0; j < chars; j += (ptr->wordSize & 0xffff))
            {
                int k;
                if (ReadProcessMemory(activeProcess->hProcess, (LPVOID)(ptr->MemAddress + i * chars + j), (LPVOID)(charbuf + j),
                                      (ptr->wordSize & 0xffff), 0))
                {
                    switch ((ptr->wordSize & 0xffff))
                    {
                        case 1:
                            sprintf(buf, "%02X ", (unsigned char)charbuf[j]);
                            break;
                        case 2:
                            sprintf(buf, "%04X ", (*(unsigned short*)(charbuf + j)));
                            break;
                        case 4:
                            sprintf(buf, "%08X ", (*(unsigned int*)(charbuf + j)));
                            break;
                    }
                    if (i == ptr->cursrow && j == ptr->curscol)
                        if (ptr->modifying)
                            strcpy(buf, ptr->cursmod);
                }
                else
                {
                    if (i == ptr->cursrow && j == ptr->curscol)
                    {
                        ptr->modifying = FALSE;
                        ptr->curpos = 0;
                        SetCursMode(ptr);
                    }
                    switch ((ptr->wordSize & 0xffff))
                    {
                        case 1:
                            strcpy(buf, "?? ");
                            break;
                        case 2:
                            strcpy(buf, "???? ");
                            break;
                        case 4:
                            strcpy(buf, "???????? ");
                            break;
                    }
                    charbuf[j] = '.';
                }
                strcat(p, buf);
                strcat(p, " ");
                for (k = 0; k < (ptr->wordSize & 0xffff); k++)
                    if (charbuf[j + k] < 32 || charbuf[j + k] > 126)
                        charbuf[j + k] = '.';
            }
            strcat(p, " ");
            strcat(p, &charbuf[0]);
            strcat(p, "\n");
        }
        TextToClipBoard(hwnd, p);
        free(p);
    }
}
void MemDoPaint(HWND hwnd, MEMDATA* ptr, int focussed)
{
    PAINTSTRUCT ps;
    HDC dc;
    dc = BeginPaint(hwnd, &ps);
    if (activeProcess)
    {
        int i;
        char buf[256];
        char charbuf[1000];
        HFONT oldFont;
        COLORREF oldbk, oldtxt;
        RECT rect;
        int lines;
        int chars;
        HDC hdouble;
        HBITMAP bitmap;
        GetClientRect(hwnd, &rect);
        hdouble = CreateCompatibleDC(dc);
        bitmap = CreateCompatibleBitmap(dc, rect.right, rect.bottom);
        SelectObject(hdouble, bitmap);
        FillRect(hdouble, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        memset(charbuf, 0, sizeof(charbuf));
        lines = (rect.bottom - rect.top) / 16;
        chars = (ptr->wordSize >> 16);
        if (!chars)
            chars = (rect.right / 8 - 10) / 4;
        oldFont = SelectObject(hdouble, MemFont);
        if (lines > 0 && chars > 0)
        {
            if (ptr->currentLen < lines * chars)
            {
                free(ptr->currentData);
                ptr->currentLen = lines * chars;
                ptr->currentData = calloc(ptr->currentLen + 16, 1);
            }
            for (i = 0; i < lines; i++)
            {
                int j;
                sprintf(buf, "%08X: ", ptr->MemAddress + i * chars);
                TextOut(hdouble, 0, i * 16 + rect.top, buf, strlen(buf));
                for (j = 0; j < chars; j += (ptr->wordSize & 0xffff))
                {
                    int k;
                    int setbk = FALSE;
                    if (ReadProcessMemory(activeProcess->hProcess, (LPVOID)(ptr->MemAddress + i * chars + j), (LPVOID)(charbuf + j),
                                          (ptr->wordSize & 0xffff), 0))
                    {
                        memcpy(ptr->currentData + chars * i + j, charbuf + j, (ptr->wordSize & 0xffff));
                        switch ((ptr->wordSize & 0xffff))
                        {
                            case 1:
                                sprintf(buf, "%02X ", (unsigned char)charbuf[j]);
                                break;
                            case 2:
                                sprintf(buf, "%04X ", (*(unsigned short*)(charbuf + j)));
                                break;
                            case 4:
                                sprintf(buf, "%08X ", (*(unsigned int*)(charbuf + j)));
                                break;
                        }
                        if (i == ptr->cursrow && j == ptr->curscol)
                            if (ptr->modifying)
                                strcpy(buf, ptr->cursmod);
                    }
                    else
                    {
                        if (i == ptr->cursrow && j == ptr->curscol)
                        {
                            ptr->modifying = FALSE;
                            ptr->curpos = 0;
                            SetCursMode(ptr);
                        }
                        switch ((ptr->wordSize & 0xffff))
                        {
                            case 1:
                                strcpy(buf, "?? ");
                                break;
                            case 2:
                                strcpy(buf, "???? ");
                                break;
                            case 4:
                                strcpy(buf, "???????? ");
                                break;
                        }
                        charbuf[j] = '.';
                    }
                    if (i == ptr->cursrow && j == ptr->curscol && focussed)
                    {
                        oldbk = GetBkColor(hdouble);
                        oldtxt = SetTextColor(hdouble, RetrieveSysColor(COLOR_HIGHLIGHTTEXT));
                        SetBkColor(hdouble, RetrieveSysColor(COLOR_HIGHLIGHT));
                        setbk = TRUE;
                    }
                    else if (ptr->lastData &&
                             memcmp(ptr->lastData + chars * i + j, ptr->currentData + chars * i + j, (ptr->wordSize & 0xffff)))
                    {
                        oldbk = GetBkColor(hdouble);
                        oldtxt = SetTextColor(hdouble, 0xff);
                        setbk = TRUE;
                    }
                    if (i == ptr->cursrow && j == ptr->curscol && focussed && ptr->modifying)
                        TextOut(hdouble, (10 + (j * 2 + j / (ptr->wordSize & 0xffff))) * 8 - 4, i * 16 + rect.top, buf,
                                2 * (ptr->wordSize & 0xffff));
                    else
                        TextOut(hdouble, (10 + (j * 2 + j / (ptr->wordSize & 0xffff))) * 8, i * 16 + rect.top, buf,
                                2 * (ptr->wordSize & 0xffff));

                    if (setbk)
                    {
                        SetTextColor(hdouble, oldtxt);
                        SetBkColor(hdouble, oldbk);
                    }
                    for (k = 0; k < (ptr->wordSize & 0xffff); k++)
                        if (charbuf[j + k] < 32 || charbuf[j + k] > 126)
                            charbuf[j + k] = '.';
                    if (i == ptr->cursrow && j == ptr->curscol && focussed && ptr->modifying)
                        TextOut(hdouble, (10 + (ptr->wordSize & 0xffff) * 2 + (j * 2 + j / (ptr->wordSize & 0xffff))) * 8 - 4,
                                i * 16 + rect.top, " ", 1);
                    else
                        TextOut(hdouble, (10 + (ptr->wordSize & 0xffff) * 2 + (j * 2 + j / (ptr->wordSize & 0xffff))) * 8,
                                i * 16 + rect.top, " ", 1);
                }
                TextOut(hdouble, (10 + (j * 2 + j / (ptr->wordSize & 0xffff))) * 8, i * 16 + rect.top, &charbuf[0],
                        strlen(charbuf));
            }
        }
        SelectObject(hdouble, oldFont);
        BitBlt(dc, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
        DeleteObject(bitmap);
        DeleteObject(hdouble);
    }
    EndPaint(hwnd, &ps);
}
//-------------------------------------------------------------------------

static LRESULT CALLBACK EditHook(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    MEMDATA* ptr;
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
        case WM_COMMAND:
        {
            char* buf;
            switch (wParam)
            {
                case IDM_EAX:
                    buf = "eax";
                    break;
                case IDM_EBX:
                    buf = "ebx";
                    break;
                case IDM_ECX:
                    buf = "ecx";
                    break;
                case IDM_EDX:
                    buf = "edx";
                    break;
                case IDM_ESP:
                    buf = "esp";
                    break;
                case IDM_EBP:
                    buf = "ebp";
                    break;
                case IDM_ESI:
                    buf = "esi";
                    break;
                case IDM_EDI:
                    buf = "edi";
                    break;
            }
            if (buf)
            {
                ptr = (MEMDATA*)GetWindowLong(hwnd, GWL_USERDATA);
                SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)buf);
                SendMessage(ptr->hwnd, WM_COMMAND, IDC_RETURN, 0);
            }
        }
        break;
        case WM_RBUTTONDOWN:
        {
            HMENU menu = LoadMenuGeneric(hInstance, "MEMORYREGISTERMENU");
            POINT pos;
            HMENU popup = GetSubMenu(menu, 0);
            GetCursorPos(&pos);

            InsertBitmapsInMenu(popup);
            TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x, pos.y, hwnd, NULL);
            DestroyMenu(menu);
        }
            return 0;
        case WM_KEYDOWN:
            ptr = (MEMDATA*)GetWindowLong(hwnd, GWL_USERDATA);
            switch (wParam)
            {
                case VK_RETURN:
                    if (GetKeyState(VK_SHIFT) & 0x80000000)
                        return SendMessage(ptr->hwnd, iMessage, wParam, lParam);
                    else
                        return SendMessage(ptr->hwnd, WM_COMMAND, IDC_RETURN, 0);
                case VK_ESCAPE:
                    if (GetKeyState(VK_SHIFT) & 0x80000000)
                        return SendMessage(ptr->hwnd, iMessage, wParam, lParam);
                    else
                        return SendMessage(ptr->hwnd, WM_COMMAND, IDC_ESCAPE, 0);
                case VK_LEFT:
                case VK_RIGHT:
                case VK_UP:
                case VK_DOWN:
                    if (GetKeyState(VK_SHIFT) & 0x80000000)
                        return SendMessage(ptr->hwnd, iMessage, wParam, lParam);
                    break;
                case VK_NEXT:
                case VK_PRIOR:
                    return SendMessage(ptr->hwnd, iMessage, wParam, lParam);
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

LRESULT CALLBACK MemInternalProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    RECT r;
    int lines, chars;
    LPCREATESTRUCT lpCreateStruct;
    MEMDATA* ptr;
    switch (iMessage)
    {
        case WM_CREATE:
            lpCreateStruct = (LPCREATESTRUCT)lParam;
            SetWindowLong(hwnd, 0, (long)lpCreateStruct->lpCreateParams);
            SetScrollRange(hwnd, SB_VERT, 0, 64000, FALSE);
            SetScrollPos(hwnd, SB_VERT, 32000, TRUE);
            break;
        case WM_DESTROY:
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            MemDoPaint(hwnd, ptr, ptr->focussed);  // focussed
            return 0;
        case WM_SETFOCUS:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            //         SendMessage(hwndFrame,WM_REDRAWTOOLBAR,0,0) ;
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            ptr->focussed = TRUE;
            InvalidateRect(hwnd, 0, 1);
            break;
        case WM_KILLFOCUS:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            ptr->focussed = FALSE;
            ptr->modifying = FALSE;
            SetCursMode(ptr);
            InvalidateRect(hwnd, 0, 1);
            break;
        case WM_RBUTTONDOWN:
        {
            HMENU menu = LoadMenuGeneric(hInstance, "MEMORYBYTESIZEMENU");
            POINT pos;
            HMENU popup = GetSubMenu(menu, 0);
            GetCursorPos(&pos);

            InsertBitmapsInMenu(popup);
            TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x, pos.y, hwnd, NULL);
            DestroyMenu(menu);
        }
            return 0;
        case WM_COMMAND:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            switch (wParam)
            {
                case IDM_BYTE:
                    ptr->wordSize = (ptr->wordSize & 0xffff0000) + 1;
                    memoryWordSize = ptr->wordSize;
                    SavePreferences();
                    break;
                case IDM_WORD:
                    ptr->wordSize = (ptr->wordSize & 0xffff0000) + 2;
                    memoryWordSize = ptr->wordSize;
                    SavePreferences();
                    break;
                case IDM_DWORD:
                    ptr->wordSize = (ptr->wordSize & 0xffff0000) + 4;
                    memoryWordSize = ptr->wordSize;
                    SavePreferences();
                    break;
                default:
                    if (wParam >= 16384 && wParam <= 16384 + 64)
                    {
                        ptr->wordSize &= 0xffff;
                        ptr->wordSize |= (wParam - 16384) << 16;
                        memoryWordSize = ptr->wordSize;
                        SavePreferences();
                    }
                    break;
            }
            MarkChanged(workArea, TRUE);
            ptr->modifying = FALSE;
            InvalidateRect(hwnd, 0, 1);
            break;
        case WM_LBUTTONDOWN:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            GetClientRect(hwnd, &r);
            lines = (r.bottom - r.top) / 16;
            chars = (r.right / 8 - 10) / 4;
            SetCursMode(ptr);
            ptr->curpos = 0;
            ptr->modifying = FALSE;
            if (LOWORD(lParam) >= 10 * 8 && LOWORD(lParam) < 10 * 8 + chars * 8 * (1 + 2 * (ptr->wordSize & 0xffff)))
            {
                ptr->curscol = (LOWORD(lParam) / 8 - 10) / (1 + (ptr->wordSize & 0xffff) * 2);
                ptr->curscol *= (ptr->wordSize & 0xffff);
                ptr->cursrow = (HIWORD(lParam)) / 16;
                if (ptr->focussed)
                    InvalidateRect(hwnd, 0, 1);
            }
            SetFocus(hwnd);
            break;
        case WM_KEYDOWN:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            GetClientRect(hwnd, &r);
            lines = (r.bottom - r.top) / 16;
            chars = (r.right / 8 - 10) / 4;
            switch (wParam)
            {
                case VK_BACK:
                    if (ptr->modifying)
                    {
                        ptr->cursmod[ptr->curpos] = ' ';
                        if (ptr->curpos)
                            ptr->curpos--;
                        InvalidateRect(hwnd, 0, 1);
                    }
                    break;
                case VK_LEFT:
                    if (ptr->modifying)
                    {
                        ptr->cursmod[ptr->curpos] = ' ';
                        if (ptr->curpos)
                            ptr->curpos--;
                        InvalidateRect(hwnd, 0, 1);
                        break;
                    }
                    if (ptr->curscol != 0)
                    {
                        ptr->curscol -= (ptr->wordSize & 0xffff);
                        InvalidateRect(hwnd, 0, 1);
                        break;
                    }
                    ptr->curscol = chars - (ptr->wordSize & 0xffff);
                    // FALL THROUGH
                case VK_UP:
                    ptr->modifying = FALSE;
                    SetCursMode(ptr);
                    ptr->curpos = 0;
                    if (ptr->cursrow == 0)
                        SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    else
                    {
                        ptr->cursrow--;
                        InvalidateRect(hwnd, 0, 1);
                    }
                    break;
                case VK_RIGHT:
                    if (ptr->modifying)
                    {
                        if (ptr->curpos < (ptr->wordSize & 0xffff) * 2)
                            ptr->curpos++;
                        break;
                    }
                    if (ptr->curscol + (ptr->wordSize & 0xffff) < chars)
                    {
                        ptr->curscol += (ptr->wordSize & 0xffff);
                        InvalidateRect(hwnd, 0, 1);
                        break;
                    }
                    ptr->curscol = 0;
                    // FALL THROUGH
                case VK_DOWN:
                    GetClientRect(hwnd, &r);
                    lines = (r.bottom - r.top) / 16;
                    ptr->modifying = FALSE;
                    SetCursMode(ptr);
                    ptr->curpos = 0;
                    if (ptr->cursrow + 1 >= lines)
                        SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                    else
                    {
                        ptr->cursrow++;
                        InvalidateRect(hwnd, 0, 1);
                    }
                    break;
                case VK_RETURN:
                    if (ptr->modifying)
                    {
                        MEMORY_BASIC_INFORMATION mbi;
                        DWORD dwOldProtect;
                        unsigned v = 0;
                        int address = ptr->MemAddress + ptr->cursrow * chars + ptr->curscol;
                        sscanf(ptr->cursmod, "%x", &v);
                        GetClientRect(hwnd, &r);
                        lines = (r.bottom - r.top) / 16;
                        chars = (r.right / 8 - 10) / 4;
                        // so it will work on the code page if protected...
                        VirtualQueryEx(activeProcess->hProcess, (LPVOID)address, &mbi, sizeof(mbi));
                        VirtualProtectEx(activeProcess->hProcess, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE,
                                         &mbi.Protect);
                        if (!WriteProcessMemory(activeProcess->hProcess, (LPVOID)address, (LPVOID)&v, (ptr->wordSize & 0xffff), 0))
                            ExtendedMessageBox("Memory Window", MB_SETFOREGROUND | MB_SYSTEMMODAL,
                                               "Could not write process memory with new value");
                        VirtualProtectEx(activeProcess->hProcess, mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);
                        FlushInstructionCache(activeProcess->hProcess, (LPVOID)address, 1);
                        ptr->modifying = FALSE;
                        ptr->curpos = 0;
                        SetCursMode(ptr);
                        InvalidateRect(hwnd, 0, 1);
                    }
                    break;
                case VK_ESCAPE:
                    ptr->modifying = FALSE;
                    ptr->curpos = 0;
                    SetCursMode(ptr);
                    InvalidateRect(hwnd, 0, 1);
                    break;
                case VK_NEXT:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
                    break;
                case VK_PRIOR:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
                    break;
                case 'C':
                    if (GetKeyState(VK_CONTROL) & 0x80000000)
                    {
                        MemCopyText(hwnd, ptr);
                    }
                    break;
            }
            break;
        case WM_CHAR:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            if (isxdigit(wParam))
            {
                ptr->modifying = TRUE;
                ptr->cursmod[ptr->curpos] = toupper(wParam);
                if (ptr->curpos < (ptr->wordSize & 0xffff) * 2)
                    ptr->curpos++;
                InvalidateRect(hwnd, 0, 0);
            }
            break;
        case WM_VSCROLL:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            GetClientRect(hwnd, &r);
            lines = (r.bottom - r.top) / 16;
            chars = (r.right / 8 - 10) / 4;
            switch (LOWORD(wParam))
            {
                case SB_BOTTOM:
                    ptr->MemAddress = -(lines * chars);
                    break;
                case SB_TOP:
                    ptr->MemAddress = 0;
                    break;
                case SB_ENDSCROLL:
                    return 0;
                case SB_LINEDOWN:
                    ptr->MemAddress += chars;
                    break;
                case SB_LINEUP:
                    ptr->MemAddress -= chars;
                    break;
                case SB_PAGEDOWN:
                    ptr->MemAddress += chars * (lines - 1);
                    break;
                case SB_PAGEUP:
                    ptr->MemAddress -= chars * (lines - 1);
                    break;
                case SB_THUMBPOSITION:
                    ptr->MemAddress = ptr->oldMemAddress + (HIWORD(wParam) - 32000) * 16;
                    ptr->oldMemAddress = ptr->MemAddress;
                    break;
                case SB_THUMBTRACK:
                    ptr->MemAddress = ptr->oldMemAddress + (HIWORD(wParam) - 32000) * 16;
                    break;
            }
            free(ptr->lastData);
            ptr->lastData = NULL;
            ptr->lastLen = 0;
            InvalidateRect(hwnd, 0, 1);
            SetScrollPos(hwnd, SB_VERT, 32000, TRUE);
            return 0;
        case WM_SIZE:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
//-------------------------------------------------------------------------

static void SetEditChild(MEMDATA* ptr)
{
    if (ptr->autoeval)
    {
        SendMessage(ptr->hwndEdit, WM_SETTEXT, 0, (LPARAM)ptr->name);
    }
    else
    {
        char buf[256];
        sprintf(buf, "0x%x", ptr->MemAddress);
        SendMessage(ptr->hwndEdit, WM_SETTEXT, 0, (LPARAM)buf);
    }
}
LRESULT CALLBACK MemProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    MEMDATA* ptr;
    RECT r;
    POINT pt;
    static int focussed;
    char buf[256];
    switch (iMessage)
    {
        case WM_ACTIVATEME:
            return SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
        case WM_CREATE:
            ptr = (MEMDATA*)calloc(1, sizeof(MEMDATA));
            ptr->wordSize = memoryWordSize;
            SetWindowLong(hwnd, 0, (long)ptr);
            ptr->modifying = 0;
            ptr->hwnd = hwnd;
            SetCursMode(ptr);
            GetClientRect(hwnd, &r);
            ptr->hwndStatic = CreateWindow("STATIC", "Address:", WS_CHILD + WS_CLIPSIBLINGS + WS_VISIBLE, r.left + 10, r.top + 6,
                                           60, 18, hwnd, 0, hInstance, 0);
            ApplyDialogFont(ptr->hwndStatic);
            ptr->hwndCombo =
                CreateWindow("COMBOBOX", "", WS_CHILD + WS_CLIPSIBLINGS + WS_BORDER + WS_VISIBLE + CBS_DROPDOWN + CBS_AUTOHSCROLL,
                             r.left + 80, r.top, 200, 100, hwnd, 0, hInstance, 0);
            ApplyDialogFont(ptr->hwndCombo);
            SubClassHistoryCombo(ptr->hwndCombo);
            SendMessage(ptr->hwndCombo, WM_SETHISTORY, 0, (LPARAM)memhist);
            pt.x = pt.y = 5;
            ptr->hwndEdit = ChildWindowFromPoint(ptr->hwndCombo, pt);
            SetWindowLong(ptr->hwndEdit, GWL_USERDATA, (long)ptr);
            oldproc = (WNDPROC)SetWindowLong(ptr->hwndEdit, GWL_WNDPROC, (int)EditHook);
            ptr->toolbar =
                CreateToolBarWindow(-1, hwnd, hwnd, ID_MEMTB, 1, memButtons, memHints, "Memory Tools", 0 /*IDH_ help hint */, TRUE);
            SendMessage(ptr->toolbar, LCF_FLOATINGTOOL, 0, 0);
            SetWindowPos(ptr->toolbar, NULL, 275, 1, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
            GetClientRect(hwnd, &r);
            r.top += 26;
            ptr->hwndChild = CreateWindowEx(0, szMemInternalClass, "", WS_CHILD + WS_VISIBLE + WS_BORDER, r.left, r.top,
                                            r.right - r.left, r.bottom - r.top, hwnd, 0, hInstance, ptr);
            break;
        case WM_RESETHISTORY:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            SendMessage(ptr->hwndCombo, WM_SETHISTORY, 0, (LPARAM)memhist);
            break;
        case WM_DESTROY:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            DeleteObject(ptr->hwndStatic);
            DeleteObject(ptr->hwndCombo);
            DeleteObject(ptr->hwndChild);
            break;
        case WM_SETFOCUS:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            InvalidateRect(ptr->hwndChild, 0, 1);
            break;
        case WM_KILLFOCUS:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            ptr->modifying = FALSE;
            SetCursMode(ptr);
            InvalidateRect(ptr->hwndChild, 0, 1);
            break;
        case WM_COMMAND:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            switch (wParam)
            {
                case IDC_RETURN:
                    SendMessage(ptr->hwndEdit, WM_GETTEXT, 256, (LPARAM)ptr->name);
                    ptr->MemAddress = GetMemAddress(ptr->name, TRUE);
                    ptr->oldMemAddress = ptr->MemAddress;
                    InvalidateRect(ptr->hwndChild, 0, 1);
                    SendMessage(ptr->hwndCombo, WM_SAVEHISTORY, 0, 0);
                    SetEditChild(ptr);
                    free(ptr->lastData);
                    ptr->lastData = NULL;
                    ptr->lastLen = 0;
                    return 0;
                case IDC_ESCAPE:
                    SetEditChild(ptr);
                    return 0;
                case IDM_GOTO:
                    ptr->autoeval = !ptr->autoeval;
                    SetEditChild(ptr);
                    return 0;
            }
            if (HIWORD(wParam) == CBN_SELENDOK)
            {
                PostMessage(hwnd, WM_COMMAND, IDC_RETURN, 0);
                return 0;
            }
            MarkChanged(workArea, TRUE);
            ptr->modifying = FALSE;
            InvalidateRect(ptr->hwndChild, 0, 1);
            break;
        case WM_RESTACK:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            SendMessage(ptr->hwndEdit, WM_GETTEXT, 256, (LPARAM)buf);
            if (ptr->autoeval)
            {
                ptr->MemAddress = GetMemAddress(buf, FALSE);
                if (ptr->MemAddress != ptr->oldMemAddress)
                {
                    free(ptr->lastData);
                    ptr->lastData = NULL;
                    ptr->lastLen = 0;
                    ptr->oldMemAddress = ptr->MemAddress;
                    InvalidateRect(ptr->hwndChild, 0, 1);
                    break;
                }
                ptr->oldMemAddress = ptr->MemAddress;
            }
            if (ptr->lastLen < ptr->currentLen)
            {
                free(ptr->lastData);
                ptr->lastLen = ptr->currentLen;
                ptr->lastData = calloc(ptr->lastLen, 1);
            }
            memcpy(ptr->lastData, ptr->currentData, ptr->lastLen);
            InvalidateRect(ptr->hwndChild, 0, 1);
            break;
        case WM_SIZE:
            ptr = (MEMDATA*)GetWindowLong(hwnd, 0);
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 0;
            r.bottom = HIWORD(lParam);
            MoveWindow(ptr->hwndChild, r.left, r.top + 26, r.right - r.left, r.bottom - r.top - 26, 1);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

//-----------------------------------------------------------------------

void RegisterMemWindow(HINSTANCE hInstance)
{
    static BOOL registered;
    if (!registered)
    {
        WNDCLASS wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_HREDRAW + CS_VREDRAW;
        wc.lpfnWndProc = &MemProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof(DWORD);
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        hbrBackground = wc.hbrBackground = (HBRUSH)RetrieveSysBrush(COLOR_BTNFACE);
        wc.lpszMenuName = 0;
        wc.lpszClassName = szMemClassName;
        RegisterClass(&wc);

        MemFont = CreateFontIndirect(&Normalfontdata);

        wc.lpfnWndProc = &MemInternalProc;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)RetrieveSysBrush(COLOR_WINDOW);
        wc.lpszClassName = szMemInternalClass;
        RegisterClass(&wc);

        registered = TRUE;
    }
}

//-------------------------------------------------------------------------

HWND CreateMem1Window(void) { return CreateInternalWindow(DID_MEMWND, szMemClassName, "Memory 1"); }
HWND CreateMem2Window(void) { return CreateInternalWindow(DID_MEMWND + 1, szMemClassName, "Memory 2"); }
HWND CreateMem3Window(void) { return CreateInternalWindow(DID_MEMWND + 2, szMemClassName, "Memory 3"); }
HWND CreateMem4Window(void) { return CreateInternalWindow(DID_MEMWND + 3, szMemClassName, "Memory 4"); }
