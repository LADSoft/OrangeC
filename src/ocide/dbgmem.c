/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
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
extern HWND hwndClient, hwndStatus, hwndFrame;
extern PROCESS *activeProcess;
extern enum DebugState uState;
extern THREAD *activeThread;
extern PROJECTITEM *workArea;
extern SCOPE *activeScope;

char *memhist[MAX_COMBO_HISTORY];
HWND hwndMem;

int memoryWordSize = 1;

static char szMemClassName[] = "xccMemoryClass";
static char szMemInternalClass[] = "xccInternalMemoryClass";
static HWND hwndCombo, hwndEdit;
static HBRUSH hbrBackground;
static WNDPROC oldproc;

static int oldMemAddress[4];
static int MemAddress[4];
static int cursrow[4], curscol[4];
static char currentSel[4][256];
static HWND hwndMemInternal[4];

static int curpos;
static char cursmod[32];
static int modifying;
static HFONT tabNormalFont;
static HFONT staticFont;
static HWND hwndTabCtrl;
static int index;

static char *nameTags[4] = { "Memory 1", "Memory 2", "Memory 3", "Memory 4" };
static LOGFONT fontdata = 
{
    -13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH |
        FF_DONTCARE,
        CONTROL_FONT
};
static LOGFONT Normalfontdata = 
{
    -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN |
        FF_DONTCARE,
        CONTROL_FONT
};
static LOGFONT tabFontData = 
{
    -13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN |
        FF_DONTCARE,
        "Arial"
};

HFONT MemFont;
static char *szMemTitle = "Memory Window";
LRESULT CALLBACK historyComboProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);

void SetCursMode(void)
{
        switch(memoryWordSize)
        {
                case 1:
                        strcpy(cursmod, "  ");
                        break;
                case 2:
                        strcpy(cursmod, "    ");
                        break;
                case 4:
                        strcpy(cursmod, "        ");
                        break;
        }
}
int GetMemAddress(char *buf, BOOL error)
{
    int addr, newaddr;
    int stars = 0;
    while (*buf == ' ')
        buf++;
    {
        int l;
        DEBUG_INFO *dbg;
        VARINFO *var;
        var = EvalExpr(&dbg, activeScope, buf, FALSE);
        if (var)
        {
            if (var->constant)
                addr = var->ival;
            else if (var->address < 0x1000)
            {
                char data[20];
                //if (!var->explicitreg)
                    //ExtendedMessageBox("Address Error", MB_SETFOREGROUND |
                        //MB_SYSTEMMODAL, 
                        //"Address is a register.  Using its value as the address.");
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
    //   newaddr = addr ;
    //   while (stars) {
    //      if (!ReadProcessMemory(activeProcess->hProcess,(LPVOID)newaddr,(LPVOID)&newaddr, 4, 0) )
    //         break ;
    //      stars-- ;
    //   }
    //   if (!stars)
    //      addr = newaddr ;

    return addr;
}

//-------------------------------------------------------------------------

static void CopyText(HWND hwnd)
{
    int i;
    char buf[1024];
    unsigned char charbuf[1000];
    PAINTSTRUCT ps;
    CONTEXT context;
    HDC dc;
    HFONT oldFont;
    COLORREF oldbk, oldtxt;
    RECT rect;
    int lines;
    int chars;
    char *p;
    memset(charbuf, 0, sizeof(charbuf));
    GetClientRect(hwnd, &rect);
    rect.top += 22;
    lines = (rect.bottom - rect.top)/ 16;
    chars = (rect.right/8 - 10) / 4; 
    
    p = malloc((chars+7) * lines + 1);
    if (p)
    {
        p[0] = 0;
        for (i = 0; i < lines; i++)
        {
            int j;
            sprintf(buf, "%08X: ", MemAddress[index] + i * chars);
            strcat(p, buf);
            for (j = 0; j < chars; j+= memoryWordSize)
            {
                int k;
                if (ReadProcessMemory(activeProcess->hProcess, (LPVOID)(MemAddress[index] +
                    i * chars + j), (LPVOID)(charbuf + j), memoryWordSize, 0))
                {
                    switch(memoryWordSize)
                    {
                            case 1:
                    sprintf(buf, "%02X ", charbuf[j]);
                                    break;
                            case 2:
                    sprintf(buf, "%04X ", (*(short *)(charbuf+j)));
                                    break;
                            case 4:
                    sprintf(buf, "%08X ", (*(int *)(charbuf+j)));
                                    break;
                    }
                    if (i == cursrow[index] && j == curscol[index])
                        if (modifying)
                            strcpy(buf, cursmod);
                }
                else
                {
                    if (i == cursrow[index] && j == curscol[index])
                    {
                        modifying = FALSE;
                        curpos = 0;
                        SetCursMode();
                    }
                    switch(memoryWordSize)
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
                for (k=0; k < memoryWordSize; k++)
                    if (charbuf[j+k] < 32 || charbuf[j+k] > 126)
                        charbuf[j+k] = '.';
            }
            strcat(p, " ");
            strcat(p, charbuf);
            strcat(p, "\n");
        }
        TextToClipBoard(hwnd, p);
        free(p);
    }
}
void MemDoPaint(HWND hwnd, int focussed)
{
    int i;
    char buf[256];
    unsigned char charbuf[1000];
    PAINTSTRUCT ps;
    CONTEXT context;
    HDC dc;
    HFONT oldFont;
    COLORREF oldbk, oldtxt;
    RECT rect;
    int lines;
    int chars;
    memset(charbuf, 0, sizeof(charbuf));
    GetClientRect(hwnd, &rect);
    lines = (rect.bottom - rect.top)/ 16;
    chars = (rect.right/8 - 10) / 4; 
    dc = BeginPaint(hwnd, &ps);
    SelectObject(dc, MemFont);
    for (i = 0; i < lines; i++)
    {
        int j;
        sprintf(buf, "%08X: ", MemAddress[index] + i * chars);
        TextOut(dc, 0, i *16+rect.top, buf, strlen(buf));
        for (j = 0; j < chars; j+= memoryWordSize)
        {
            int k;
            if (i == cursrow[index] && j == curscol[index] && focussed)
            {
                oldbk = GetBkColor(dc);
                oldtxt = SetTextColor(dc, RetrieveSysColor(COLOR_HIGHLIGHTTEXT));
//                if (modifying)
                    SetBkColor(dc, RetrieveSysColor(COLOR_HIGHLIGHT));
//                else
//                    SetBkColor(dc, oldtxt);
            }
            if (ReadProcessMemory(activeProcess->hProcess, (LPVOID)(MemAddress[index] +
                i * chars + j), (LPVOID)(charbuf + j), memoryWordSize, 0))
            {
                switch(memoryWordSize)
                {
                        case 1:
                sprintf(buf, "%02X ", charbuf[j]);
                                break;
                        case 2:
                sprintf(buf, "%04X ", (*(short *)(charbuf+j)));
                                break;
                        case 4:
                sprintf(buf, "%08X ", (*(int *)(charbuf+j)));
                                break;
                }
                if (i == cursrow[index] && j == curscol[index])
                    if (modifying)
                        strcpy(buf, cursmod);
            }
            else
            {
                if (i == cursrow[index] && j == curscol[index])
                {
                    modifying = FALSE;
                    curpos = 0;
                    SetCursMode();
                }
                switch(memoryWordSize)
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
            if (i == cursrow[index] && j == curscol[index] && focussed && modifying)
                TextOut(dc, (10+ (j * 2 + j/memoryWordSize)) *8-4, 
                                                i *16+rect.top, buf, 2 * memoryWordSize);
            else
                TextOut(dc, (10+ (j * 2 + j/memoryWordSize)) *8, 
                                                i *16+rect.top, buf, 2 * memoryWordSize);
                        
            for (k=0; k < memoryWordSize; k++)
                if (charbuf[j+k] < 32 || charbuf[j+k] > 126)
                    charbuf[j+k] = '.';
            if (i == cursrow[index] && j == curscol[index] && focussed)
            {
                SetTextColor(dc, oldtxt);
                SetBkColor(dc, oldbk);
            }
            if (i == cursrow[index] && j == curscol[index] && focussed && modifying)
                TextOut(dc, (10 + memoryWordSize * 2 + 
                            (j * 2 + j/memoryWordSize)) *8-4, 
                             i *16+rect.top, " ", 1);
            else
                TextOut(dc, (10 + memoryWordSize * 2 + 
                            (j * 2 + j/memoryWordSize)) *8, 
                             i *16+rect.top, " ", 1);
        }
        TextOut(dc, (10+ (j * 2 + j/memoryWordSize)) *8, i *16+rect.top, charbuf, strlen(charbuf));
    }

    SelectObject(dc, oldFont);//FIXME oldFont ?

    EndPaint(hwnd, &ps);
}
//-------------------------------------------------------------------------

LRESULT CALLBACK EditHook(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
    lParam)
{
    switch (iMessage)
    {
        case WM_COMMAND:
        {
                char *buf;
                switch(wParam)
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
                        SendMessage(hwnd, WM_SETTEXT, 0 , (LPARAM)buf);
                        SendMessage(hwndMem, WM_COMMAND, IDC_RETURN, 0);
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
            TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x,
                pos.y, hwnd, NULL);
            DestroyMenu(menu);
        }
            return 0;
        case WM_KEYDOWN:
            switch (wParam)
            {
            case VK_RETURN:
                if (GetKeyState(VK_SHIFT) &0x80000000)
                    return SendMessage(hwndMem, iMessage, wParam, lParam);
                else
                    return SendMessage(hwndMem, WM_COMMAND, IDC_RETURN, 0);
            case VK_ESCAPE:
                if (GetKeyState(VK_SHIFT) &0x80000000)
                    return SendMessage(hwndMem, iMessage, wParam, lParam);
                else
                    return SendMessage(hwndMem, WM_COMMAND, IDC_ESCAPE, 0);            
            case VK_LEFT:
            case VK_RIGHT:
            case VK_UP:
            case VK_DOWN:
                if (GetKeyState(VK_SHIFT) &0x80000000)
                    return SendMessage(hwndMem, iMessage, wParam, lParam);
                break;
            case VK_NEXT:
            case VK_PRIOR:
                return SendMessage(hwndMem, iMessage, wParam, lParam);
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

LRESULT CALLBACK MemInternalProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    RECT r;
    int lines, chars;
    char buf[256];
    switch (iMessage)
    {
        case WM_CREATE:
            SetScrollRange(hwnd, SB_VERT, 0, 64000, FALSE);
            SetScrollPos(hwnd, SB_VERT, 32000, TRUE);
            break;
        case WM_DESTROY:
            break;
        case WM_PAINT:
            MemDoPaint(hwnd, GetWindowLong(hwnd, GWL_USERDATA)); // focussed
            return 0;
        case WM_SETFOCUS:
            //         SendMessage(hwndFrame,WM_REDRAWTOOLBAR,0,0) ;
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            SetWindowLong(hwnd, GWL_USERDATA, TRUE);
            InvalidateRect(hwnd, 0, 1);
            break;
        case WM_KILLFOCUS:
            SetWindowLong(hwnd, GWL_USERDATA, FALSE);
            modifying = FALSE;
            SetCursMode();
            InvalidateRect(hwnd, 0, 1);
            break;
        case WM_RBUTTONDOWN:
            {
                HMENU menu = LoadMenuGeneric(hInstance, "MEMORYBYTESIZEMENU");
                                POINT pos;
                HMENU popup = GetSubMenu(menu, 0);
                GetCursorPos(&pos);

                InsertBitmapsInMenu(popup);
                TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x,
                    pos.y, hwnd, NULL);
                DestroyMenu(menu);
            }
            return 0;
        case WM_COMMAND:
            switch(wParam)
            {
                    case IDM_BYTE:
                            memoryWordSize = 1;
                            SavePreferences();
                            break;
                    case IDM_WORD:
                            memoryWordSize = 2;
                            SavePreferences();
                            break;
                    case IDM_DWORD:
                            memoryWordSize = 4;
                            SavePreferences();
                            break;
                    case IDM_GOTO:
                        MemAddress[index] = lParam;
                        oldMemAddress[index] = MemAddress[index];
                        InvalidateRect(hwnd, 0, 1);
                        return 0;
            }
            MarkChanged(workArea, TRUE);
            modifying = FALSE;
            InvalidateRect(hwnd, 0, 1);
            break;
        case WM_LBUTTONDOWN:
            GetClientRect(hwnd, &r);
            lines = (r.bottom - r.top)/ 16;
            chars = (r.right/8 - 10) / 4; 
            SetCursMode();
            curpos = 0;
            modifying = FALSE;
            if (LOWORD(lParam) >= 10 *8 && LOWORD(lParam) < 10*8 + chars *8 * (1 + 2 * memoryWordSize))
            {
                curscol[index] = (LOWORD(lParam)/8 - 10) / (1 + memoryWordSize * 2);
                curscol[index] *= memoryWordSize;
                cursrow[index] = (HIWORD(lParam)) / 16;
                if (GetWindowLong(hwnd, GWL_USERDATA))
                    InvalidateRect(hwnd, 0, 1);
            }
            SetFocus(hwnd);
            break;
        case WM_KEYDOWN:
            GetClientRect(hwnd, &r);
            lines = (r.bottom - r.top)/ 16;
            chars = (r.right/8 - 10) / 4; 
            switch (wParam)
            {
            case VK_BACK:
                if (modifying)
                {
                    cursmod[curpos] = ' ';
                    if (curpos)
                        curpos--;
                    InvalidateRect(hwnd, 0, 1);
                }
                break;
            case VK_LEFT:
                if (modifying)
                {
                    cursmod[curpos] = ' ';
                    if (curpos)
                        curpos--;
                    InvalidateRect(hwnd, 0, 1);
                    break;
                }
                if (curscol[index] != 0)
                {
                    curscol[index]-= memoryWordSize;
                    InvalidateRect(hwnd, 0, 1);
                    break;
                }
                curscol[index] = chars - memoryWordSize;
                // FALL THROUGH
            case VK_UP:
                modifying = FALSE;
                SetCursMode();
                curpos = 0;
                if (cursrow[index] == 0)
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                else
                {
                    cursrow[index]--;
                    InvalidateRect(hwnd, 0, 1);
                }
                break;
            case VK_RIGHT:
                if (modifying)
                {
                    if (curpos < memoryWordSize * 2)
                        curpos++;
                    break;
                }
                if (curscol[index] + memoryWordSize < chars)
                {
                    curscol[index]+=memoryWordSize;
                    InvalidateRect(hwnd, 0, 1);
                    break;
                }
                curscol[index] = 0;
                // FALL THROUGH
            case VK_DOWN:
                GetClientRect(hwnd, &r);
                lines = (r.bottom - r.top)/ 16;
                modifying = FALSE;
                SetCursMode();
                curpos = 0;
                if (cursrow[index] +1 >= lines)
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                else
                {
                    cursrow[index]++;
                    InvalidateRect(hwnd, 0, 1);
                }
                break;
            case VK_RETURN:
                if (modifying)
                {
                    MEMORY_BASIC_INFORMATION mbi;
                    DWORD dwOldProtect;
                    unsigned v = 0;
                    int address = MemAddress[index] + cursrow[index] * chars + curscol[index];
                    sscanf(cursmod, "%x", &v);
                    GetClientRect(hwnd, &r);
                    lines = (r.bottom - r.top)/ 16;
                    chars = (r.right/8 - 10) / 4; 
                    // so it will work on the code page if protected...
                    VirtualQueryEx(activeProcess->hProcess, (LPVOID)address, &mbi,
                        sizeof(mbi));
                    VirtualProtectEx(activeProcess->hProcess, mbi.BaseAddress,
                        mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect);
                    if (!WriteProcessMemory(activeProcess->hProcess, (LPVOID)
                        address, (LPVOID) &v, memoryWordSize, 0))
                        ExtendedMessageBox("Memory Window", MB_SETFOREGROUND |
                            MB_SYSTEMMODAL, 
                            "Could not write process memory with new value");
                    VirtualProtectEx(activeProcess->hProcess, mbi.BaseAddress,
                        mbi.RegionSize, mbi.Protect, &dwOldProtect);
                    FlushInstructionCache(activeProcess->hProcess, (LPVOID)
                        address, 1);
                    modifying = FALSE;
                    curpos = 0;
                    SetCursMode();
                    InvalidateRect(hwnd, 0, 1);
                }
                break;
            case VK_ESCAPE:
                modifying = FALSE;
                curpos = 0;
                SetCursMode();
                InvalidateRect(hwnd, 0, 1);
                break;
            case VK_NEXT:
                SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
                break;
            case VK_PRIOR:
                SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
                break;
            case 'C':
                if (GetKeyState(VK_CONTROL) &0x80000000)
                {
                    CopyText(hwnd);
                }
                break;
            
            }
            break;
        case WM_CHAR:
            if (isxdigit(wParam))
            {
                modifying = TRUE;
                cursmod[curpos] = toupper(wParam);
                if (curpos < memoryWordSize * 2)
                    curpos++;
                InvalidateRect(hwnd, 0, 0);
            }
            break;
        case WM_VSCROLL:
            GetClientRect(hwnd, &r);
            lines = (r.bottom - r.top)/ 16;
            chars = (r.right/8 - 10) / 4; 
            switch (LOWORD(wParam))
            {
            case SB_BOTTOM:
                MemAddress[index] =  - (lines *chars);
                break;
            case SB_TOP:
                MemAddress[index] = 0;
                break;
            case SB_ENDSCROLL:
                return 0;
            case SB_LINEDOWN:
                MemAddress[index] += chars;
                break;
            case SB_LINEUP:
                MemAddress[index] -= chars;
                break;
            case SB_PAGEDOWN:
                MemAddress[index] += chars *(lines - 1);
                break;
            case SB_PAGEUP:
                MemAddress[index] -= chars *(lines - 1);
                break;
            case SB_THUMBPOSITION:
                MemAddress[index] = oldMemAddress[index] + (HIWORD(wParam) - 32000) *16;
                oldMemAddress[index] = MemAddress[index];
                break;
            case SB_THUMBTRACK:
                MemAddress[index] = oldMemAddress[index] + (HIWORD(wParam) - 32000) *16;
                break;
            }
            InvalidateRect(hwnd, 0, 1);
            SetScrollPos(hwnd, SB_VERT, 32000, TRUE);
            return 0;
        case WM_SIZE:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
//-------------------------------------------------------------------------

LRESULT CALLBACK MemProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    RECT r;
    POINT pt;
    HWND hwndStatic;
    int lines, chars;
    static int focussed;
    char buf[256];
    LPNMHDR nmh;
    int i;
    switch (iMessage)
    {
        case WM_NOTIFY:
            nmh = (LPNMHDR)lParam;
            if (nmh->code == TABN_SELECTED)
            {
                LSTABNOTIFY *p = (LSTABNOTIFY *)nmh;
                curpos = 0;
                modifying = FALSE;
                SendMessage(hwndEdit, WM_GETTEXT, 256, (LPARAM)currentSel[index]);
                ShowWindow(hwndMemInternal[index], SW_HIDE);
                for (i=0; i < 4; i++)
                    if (p->lParam == (LPARAM)hwndMemInternal[i])
                        index = i;
                ShowWindow(hwndMemInternal[index], SW_SHOW);
                SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)currentSel[index]);
                break;
            }
            break;
        case WM_ACTIVATEME:
            return SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
        case WM_CREATE:
            index = 0;
            modifying = 0;
            curpos = 0;
            SetCursMode();
            MemFont = CreateFontIndirect(&fontdata);
            staticFont = CreateFontIndirect(&Normalfontdata);
            GetClientRect(hwnd, &r);
            hwndStatic = CreateWindow("STATIC", "Address:", WS_CHILD + WS_CLIPSIBLINGS + WS_VISIBLE,
                r.left + 20, r.top + 6, 60, 18,
                hwnd, 0, hInstance, 0);
            SendMessage(hwndStatic, WM_SETFONT, (LPARAM)staticFont, 1);
            hwndCombo = CreateWindow("COMBOBOX", "", WS_CHILD + WS_CLIPSIBLINGS +
                WS_BORDER + WS_VISIBLE + CBS_DROPDOWN + CBS_AUTOHSCROLL, 
                                r.left + 80, r.top, 200, 100, hwnd, 0, hInstance, 0);
            SendMessage(hwndCombo, WM_SETFONT, (LPARAM)MemFont, 1);
            SubClassHistoryCombo(hwndCombo);
            SendMessage(hwndCombo, WM_SETHISTORY, 0, (LPARAM)memhist);
            pt.x = pt.y = 5;
            hwndEdit = ChildWindowFromPoint(hwndCombo, pt);
            oldproc = (WNDPROC)SetWindowLong(hwndEdit, GWL_WNDPROC, (int)EditHook);
            GetClientRect(hwnd, &r);
            tabNormalFont = CreateFontIndirect(&tabFontData);
            hwndTabCtrl = CreateLsTabWindow(hwnd, TABS_BOTTOM | TABS_HOTTRACK | TABS_FLAT | WS_VISIBLE);
            SendMessage(hwndTabCtrl, WM_SETFONT, (WPARAM)tabNormalFont, 0);
            r.top += 26;
            r.bottom -= 25;
            hwndMemInternal[0] = CreateWindowEx(0, szMemInternalClass, "", WS_CHILD + WS_VISIBLE + WS_BORDER,
                                          r.left, r.top, r.right - r.left, r.bottom - r.top,
                                          hwnd, 0, hInstance, 0);
            hwndMemInternal[1] = CreateWindowEx(0, szMemInternalClass, "", WS_CHILD + WS_BORDER, //FIXME ? 1-3 the same
                                          r.left, r.top, r.right - r.left, r.bottom - r.top,
                                          hwnd, 0, hInstance, 0);
            hwndMemInternal[2] = CreateWindowEx(0, szMemInternalClass, "", WS_CHILD + WS_BORDER,
                                          r.left, r.top, r.right - r.left, r.bottom - r.top,
                                          hwnd, 0, hInstance, 0);
            hwndMemInternal[3] = CreateWindowEx(0, szMemInternalClass, "", WS_CHILD + WS_BORDER,
                                          r.left, r.top, r.right - r.left, r.bottom - r.top,
                                          hwnd, 0, hInstance, 0);
            for (i=3; i >=0 ; i--)
                SendMessage(hwndTabCtrl, TABM_ADD, (WPARAM)nameTags[i], (LPARAM)hwndMemInternal[i]);
            break;
        case WM_DESTROY:
            hwndMem = 0;
            DeleteObject(MemFont);
            DeleteObject(staticFont);
            break;
        case WM_SETFOCUS:
            SetFocus(hwndCombo);
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            InvalidateRect(hwndMemInternal[index], 0, 1);
            break;
        case WM_KILLFOCUS:
            modifying = FALSE;
            SetCursMode();
            InvalidateRect(hwndMemInternal[index], 0, 1);
            break;
        case WM_COMMAND:
            switch(wParam)
            {
                case IDC_RETURN:
                    SendMessage(hwndEdit, WM_GETTEXT, 256, (LPARAM)buf);
                    MemAddress[index] = GetMemAddress(buf, TRUE);
                    oldMemAddress[index] = MemAddress[index];
                    InvalidateRect(hwndMemInternal[index], 0, 1);
                    SendMessage(hwndCombo, WM_SAVEHISTORY, 0, 0);
                    return 0;
                case IDC_ESCAPE:
                    sprintf(buf,"0x%x", MemAddress[index]);
                    SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)buf);
                    return 0;
            }
            MarkChanged(workArea, TRUE);
            modifying = FALSE;
            InvalidateRect(hwndMemInternal[index], 0, 1);
            break;
        case WM_RESTACK:
            SendMessage(hwndEdit, WM_GETTEXT, 256, (LPARAM)buf);
            MemAddress[index] = GetMemAddress(buf, FALSE);
            oldMemAddress[index] = MemAddress[index];
            InvalidateRect(hwndMemInternal[index], 0, 1);
            break;
        case WM_SIZE:
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 0;
            r.bottom = HIWORD(lParam);
            MoveWindow(hwndTabCtrl, r.left, r.bottom - 24, r.right - r.left, 24, 1);
            for (i=0; i < 4; i++)
                MoveWindow(hwndMemInternal[i], r.left, r.top+26, r.right - r.left, r.bottom -
                    r.top - 25 - 26, 1);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}



//-------------------------------------------------------------------------

void RegisterMemWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW + CS_VREDRAW;
    wc.lpfnWndProc = &MemProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    hbrBackground = wc.hbrBackground = (HBRUSH)RetrieveSysBrush(COLOR_BTNFACE);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szMemClassName;
    RegisterClass(&wc);
    
    wc.lpfnWndProc = &MemInternalProc;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)RetrieveSysBrush(COLOR_WINDOW);
    wc.lpszClassName = szMemInternalClass;
    RegisterClass(&wc);

}

//-------------------------------------------------------------------------

HWND CreateMemWindow(void)
{
    HWND rv;
    RECT r;
    POINT pt;
    if (hwndMem)
    {
        SendMessage(hwndMem, WM_SETFOCUS, 0, 0);
    }
    else
    {
        hwndMem = CreateDockableWindow(DID_MEMWND, szMemClassName, szMemTitle, hInstance, 80 * 8, 19 * 8 + 40);
    }
    return hwndMem;
}
