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
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>

#include "header.h"
#include "operands.h"
#include "opcodes.h"
#include <ctype.h>

#define ASM_OFFSET 35

#define LINES 512
#define LINELEN 564

extern HINSTANCE hInstance;
extern HWND hwndClient, hwndStatus, hwndFrame, hwndSrcTab;
extern enum DebugState uState;
extern THREAD *activeThread;
extern PROJECTITEM *activeProject;
extern PROCESS *activeProcess;
extern HIMAGELIST tagImageList;

HWND hwndASM;

static char **lines;
static int *addrs;
static int *linenos;
static int curline, shownLine;

static char szASMClassName[] = "xccASMClass";
static char szASMBlankClassName[] = "xccASMClass2";
static char *szASMTitle = "Disassembly Window";
static int asmAddress = 0x410000, asmIP;
static DWORD threadID = 0;
static int rePosition = 0;
static LOGFONT fontdata = 
{
    16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH |
        FF_DONTCARE, "Courier New"
};
static HFONT asmFont;
static int caretX, caretY;
static int charwidth;

extern long code_address;
BYTE *code_ptr;
void GetCodeLine(char *s)
{
    BOOL isNewLine = TRUE;
    BYTE *oldposition = code_ptr;
    char *put;
    OPTABLE *theTable;
    s[0] = 0;
    sprintf(s, "%08x: ", (int)(code_address));
    put = TabTo(s, 11);
    code_ptr = ReadOverrides(code_ptr, VAL_386 | VAL_FLOAT);
    if (!(theTable = FindOpcode(code_ptr, VAL_386 | VAL_FLOAT)))
    {
        strcpy(put, "db");
        put = TabTo(put, TAB_ARGPOS);
        sprintf(put, "%02x",  *code_ptr++);
    }
    else
    {
        code_ptr = DispatchOperand(code_ptr, theTable, TRUE);
        FormatDissassembly(put);
    }
    code_address += code_ptr - oldposition;
}

//-------------------------------------------------------------------------

static char *getline(char *module, int lineno)
{
    static char line[256];
    static char oldmodule[256];
    static int oldline;
    static FILE *infile;
    char buffer[1024],  *rline;
    int l;

    line[0] = 0;
    if (infile && (lineno == 0 || strcmp(module, oldmodule) || lineno <=
        oldline))
    {
        fclose(infile);
        infile = 0;
        oldline = 0;
    }
    strcpy(oldmodule, module);
    if (lineno == 0)
        return NULL;
    if (!infile)
        infile = fopen(module, "r");
    if (!infile)
        return line;

    do
    {
        buffer[0] = 0;
        fgets(buffer, 1024, infile);
    }
    while (!feof(infile) && ++oldline < lineno);
    if (!feof(infile))
        strncpy(line, buffer, 255);
    rline = line;
//    while (isspace(*rline))
//        rline++;
    l = strlen(rline) - 1;
    if (l >= 0 && rline[l] < 32)
        rline[l] = 0;
    return rline;
}

//-------------------------------------------------------------------------

void CalculateDisassembly(int moving)
{
    int lastlineno = -1;
    RECT r;
    int rv = 0;
    int lineno;
    char buffer[256];
    char buf1[256];
    BYTE dbuf[32];
    int i;
    int linecount;
    GetClientRect(hwndASM, &r);
    linecount = (r.bottom - r.top) / 16;
    code_ptr = dbuf;

	if (!activeProcess)
		return;
    i = 0;
    code_address = asmAddress - LINES / 2;
    while (code_address < asmAddress + LINES / 2 && i < LINES)
    {
        code_ptr = dbuf;
        if (code_address >= asmAddress && !rv)
            rv = i;
        if (!ReadProcessMemory(activeProcess->hProcess, (LPVOID)(code_address),
            dbuf, 32, 0))
        {
            addrs[i] = code_address;
            sprintf(lines[i++], "%08X: ??", code_address++);
        }
        else
        {
            DEBUG_INFO *dbg;
            char *types,  *syms;
            if (GetBreakpointLine(code_address, buf1, &lineno, FALSE) == code_address)
            {
                if (lastlineno == -1)
                {
                    linenos[i] = lineno;
                    addrs[i] = code_address;
                    sprintf(lines[i++], "%s", getline(buf1, lineno));
                }
                else
                {
                    int j;
                    for (j= lastlineno + 1; j <= lineno && i < LINES; j++)
                    {
                        linenos[i] = j;
                        addrs[i] = code_address;
                        sprintf(lines[i++], "%s", getline(buf1, j));
                    }
                    if (i >= LINES)
                        break;
                }
                lastlineno = lineno;
            }
            /*
            if (FindGlobalSymbol(&dbg, code_address, buffer, NULL))
            {
                addrs[i] = code_address;
                strcat(buffer, ":");
                strcpy(lines[i++], buffer);
            }
            */
            addrs[i] = code_address;
            GetCodeLine(lines[i++]);
        }
    }
    if (!moving)
    {
        shownLine = rv - linecount / 2;
    }
    else
    {
        shownLine = rv;
        if (moving > 0)
            while (addrs[shownLine] == addrs[shownLine + 1])
                shownLine++;
        shownLine += moving;
    }
    while (shownLine && addrs[shownLine - 1] == addrs[shownLine])
        shownLine--;
    asmAddress = addrs[shownLine];
    getline("", 0);
}

//-------------------------------------------------------------------------

static void CopyText(HWND hwnd)
{
    RECT r;
    int linecnt ;
    char *p;
    GetClientRect(hwnd, &r);
    linecnt = (r.bottom - r.top) / 16;
    p = malloc(linecnt * 100);
    if (p)
    {
        int i;
        p[0] = 0;
        for (i = 0; i < linecnt; i++)
        {
            int c=-1, lc;
            if ((lc = strlen(lines[i + shownLine])) > 97)
            {
                c = lines[i + shownLine][97];
                lines[i + shownLine][97] = 0;
            }
            strcat(p, lines[i+shownLine]);
            strcat(p, "\n");
            if (c !=- -1)
                lines[i + shownLine][97] = c;
        }
        TextToClipBoard(hwnd, p);
        free(p);
    }
}
int indent(int n)
{
    char *p = lines[n];
    if (p[8] == ':')
    {
        int i;
        for (i=0; i < 8; i++)
            if (!isxdigit(p[i]))
                break;
        if (i == 8)
            return 150;
    }
    return 0;
}
void DoDisassembly(HDC dc, RECT *r)
{
    int i;
    COLORREF v = 0;
    int linecnt = (r->bottom - r->top) / 16;
    for (i = 0; i < linecnt; i++)
    {
        int n, indnt = indent(i+shownLine);
        RECT r1;
        n = addrs[i + shownLine];
        if (indnt)
        {
            v = SetTextColor(dc, 0xaaaaaa);
        }
        TextOut(dc, ASM_OFFSET + 5 + indnt, i *16+r->top, lines[i + shownLine], strlen
            (lines[i + shownLine]));
        if (indnt)
        {
            SetTextColor(dc, v);
        }
        if (indnt)
        {
            if (n == asmIP)
            {
                if (isBreakPoint(n))
                {
                    ImageList_Draw(tagImageList, IML_STOPBP, dc, 16, i*16, ILD_NORMAL);
                }
                else if (uState != Running)
                {
                    ImageList_Draw(tagImageList, IML_STOP, dc, 16, i*16, ILD_NORMAL);
                }
            }
            else if (isBreakPoint(n))
            {
                ImageList_Draw(tagImageList, IML_BP, dc, 16, i*16, ILD_NORMAL);
            }
        }
    }
}

//-------------------------------------------------------------------------

LRESULT CALLBACK gotoProc2(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    char buf[256];
    int i;
    VARINFO *info;
    DEBUG_INFO *dbg;
    switch (iMessage)
    {
        case WM_COMMAND:
            if (wParam == IDOK)
            {
                GetEditField(hwnd, IDC_GOTO, buf);
                info = EvalExpr(&dbg, NULL, buf, TRUE);
                if (info)
                {
                    if (info->constant)
                        i = info->ival;
                    else
                        i = info->address;
                    FreeVarInfo(info);
                }
                else
                    i = 0xffffffff;
                EndDialog(hwnd, i);
                break;
            }
            if (HIWORD(wParam) == EN_CHANGE)
            {
                DisableControl(hwnd, IDOK, !GetWindowText((HWND)lParam, buf, 2))
                    ;
                break;
            }
            if (wParam != IDCANCEL)
                break;
        case WM_CLOSE:
            EndDialog(hwnd, 0xffffffff);
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            SetEditField(hwnd, IDC_GOTO, "");
            DisableControl(hwnd, IDOK, 1);
            break;
    }
    return 0;
}
//-------------------------------------------------------------------------

void asmDoPaint(HWND hwnd)
{
    HDC dc;
    HPEN hpen, oldpen;
    RECT rect;
    HBRUSH graybrush;
    PAINTSTRUCT paint;
    HFONT oldFont;
    HDC hdouble;
    HBITMAP bitmap;
    int oldcolor;
    GetClientRect(hwnd, &rect);
    dc = BeginPaint(hwnd, &paint);
    hdouble = CreateCompatibleDC(dc);
    bitmap = CreateCompatibleBitmap(dc, rect.right, rect.bottom);
    SelectObject(hdouble, bitmap);
    FillRect(hdouble, &rect, (HBRUSH)(COLOR_WINDOW + 1));
    hpen = CreatePen(PS_SOLID, 0, RetrieveSysColor(COLOR_BTNSHADOW));
    graybrush = CreateSolidBrush(RetrieveSysColor(COLOR_BTNFACE));
    rect.right = ASM_OFFSET - 1;
//    MoveToEx(hdouble, ASM_OFFSET - 1, 0, 0);
//    LineTo(hdouble, ASM_OFFSET - 1, r.bottom);
    FillRect(hdouble, &rect, graybrush);
    oldpen = SelectObject(hdouble, hpen);
    MoveToEx(hdouble, ASM_OFFSET - 1, 0, 0);
    LineTo(hdouble, ASM_OFFSET - 1, rect.bottom);
    SelectObject(hdouble, oldpen);
    DeleteObject(hpen);
    DeleteObject(graybrush);
    
    if (activeProcess && activeProcess->hProcess)
    {
        oldFont = SelectObject(hdouble, asmFont);
        oldcolor = SetTextColor(hdouble, 0x6a6b6c);
        DoDisassembly(hdouble, &rect);
        SetTextColor(hdouble, oldcolor);
        SelectObject(hdouble, oldFont);
    }
    GetClientRect(hwnd, &rect);
    BitBlt(dc, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
    DeleteObject(bitmap);
    DeleteObject(hdouble);
    EndPaint(hwnd, &paint);
}

//-------------------------------------------------------------------------

LRESULT CALLBACK ASMProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    int i;
    static int offset;
    HDC dc;
    HFONT oldFont;
    TEXTMETRIC metric;
    RECT r;
    int xlines;
    switch (iMessage)
    {
        case WM_CREATE:
            lines = (char **)calloc(1,LINES *sizeof(char*));
            if (!lines)
                return  - 1;
            addrs = (int*)calloc(1,LINES *sizeof(int));
            if (!addrs)
                return  - 1;
            linenos = (int *)calloc(1, LINES *sizeof(int));
            if (!linenos)
                return -1;
            for (i = 0; i < LINES; i++)
                if (!(lines[i] = (char*)calloc(1,LINELEN)))
                    return  - 1;
            caretY = 0;
            caretX = ASM_OFFSET + 5;
            asmFont = CreateFontIndirect(&fontdata);
            dc = GetDC(hwnd);
            oldFont = SelectObject(dc, asmFont);
            GetTextMetrics(dc, &metric);
            SelectObject(dc, oldFont);
            ReleaseDC(hwnd, dc);
            charwidth = metric.tmMaxCharWidth;
            SetScrollRange(hwnd, SB_VERT, 0, 64000, FALSE);
            SetScrollPos(hwnd, SB_VERT, 32000, TRUE);
            SendMessage(hwndSrcTab, TABM_ADD, (WPARAM)"Disassembly", (LPARAM)hwnd);
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            asmDoPaint(hwnd);
            return 0;
        case WM_SETFOCUS:
            SendMessage(GetParent(hwnd), WM_ACTIVATEME, 0, 0);
            CreateCaret(hwnd, 0, 2, 16);
            SetCaretPos(caretX, caretY);
            ShowCaret(hwnd);
            break;
        case WM_KILLFOCUS:
            DestroyCaret();
            break;
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            caretX = LOWORD(lParam);
            caretY = HIWORD(lParam);
            if (LOWORD(lParam) >= ASM_OFFSET + 5)
            {
                caretY &= ~15;
                if (caretX < ASM_OFFSET + 5)
                {
                    caretX = 0;
                }
                else
                {
                    caretX -= ASM_OFFSET + 5;
                }
                caretX /= charwidth;
                caretX *= charwidth;
                caretX += ASM_OFFSET + 5;
                SetCaretPos(caretX, caretY);
            }
            SetFocus(hwnd);
            if (iMessage == WM_LBUTTONDOWN)
            {
                int n ;
setBreakPoint:
                if (shownLine + caretY / 16 >= 4096)
                    return 0;
                n = addrs[shownLine + caretY/ 16];
                if (indent(shownLine + caretY/16))
                {
                    if (isBreakPoint(n))
                    {
                        dbgClearBreakPoint(NULL, n);
                    }		
                    else
                    {
                        dbgSetBreakPoint(NULL, n, NULL);
                    }
                }
                else
                {
                    int lineno;
                    DWINFO info;
                    memset(&info, 0, sizeof(info));
                    GetBreakpointLine(addrs[shownLine + caretY/16], info.dwName, &lineno, FALSE);
                    info.dwLineNo =  linenos[shownLine + caretY/16];
                    info.logMRU = FALSE;
                    info.newFile = FALSE;
                    CreateDrawWindow(&info, TRUE);
                }
                    
                InvalidateRect(hwndASM, 0, 1);
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDM_BREAKPOINT:
                    goto setBreakPoint;
                case IDM_GOTO:
                    asmAddress = DialogBox(hInstance, "GOTODIALOG2", hwnd, (DLGPROC)
                        gotoProc2);
                    if (asmAddress != 0xffffffff)
                    {
                        CalculateDisassembly(0);
                        InvalidateRect(hwndASM, 0, 1);
                    }
                    break;
                case ID_SETADDRESS:
                    asmAddress = asmIP = lParam;
                    threadID = activeThread->idThread;
                    CalculateDisassembly(0);
                    InvalidateRect(hwndASM, 0, 1);
                    break;
            }
            break;
        case WM_GETCURSORADDRESS:
            if (shownLine + caretY / 16 >= 4096)
                return 0;
            sscanf(lines[shownLine + caretY / 16], "%x", &i);
            return i;
        case WM_KEYDOWN:
            switch (wParam)
            {
            case 'C':
                if (GetKeyState(VK_CONTROL) &0x80000000)
                {
                    CopyText(hwnd);
                }
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
            }
            break;
        case WM_DESTROY:
            PostMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            DeleteObject(asmFont);
            for (i = 0; i < LINES; i++)
                free(lines[i]);
            free(lines);
            free(addrs);
            hwndASM = 0;
            break;
        case WM_SIZE:
            InvalidateRect(hwnd, 0, 1);
            break;
        case WM_VSCROLL:
            GetClientRect(hwnd, &r);
            xlines = r.bottom / 16;
            switch (LOWORD(wParam))
            {
            case SB_BOTTOM:
            case SB_TOP:
                asmAddress = asmIP;
                CalculateDisassembly(0);
                break;
            case SB_ENDSCROLL:
                return 0;
            case SB_LINEDOWN:
                CalculateDisassembly(1);
                break;
            case SB_LINEUP:
                CalculateDisassembly( - 1);
                break;
            case SB_PAGEDOWN:
                CalculateDisassembly(xlines - 1);
                break;
            case SB_PAGEUP:
                CalculateDisassembly(1-xlines);
                break;
            case SB_THUMBPOSITION:
                offset = 0;
                break;
            case SB_THUMBTRACK:

                xlines = (HIWORD(wParam) - 32000) / 320;
                if (xlines - offset)
                    CalculateDisassembly(xlines - offset);
                offset = xlines;
                InvalidateRect(hwnd, 0, 1);
                return 0;

            }
            InvalidateRect(hwnd, 0, 1);
            SetScrollPos(hwnd, SB_VERT, 32000, TRUE);
            return 0;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterASMWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &ASMProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szASMClassName;
    RegisterClass(&wc);

}

void StopASMWindow(void)
{
    if (hwndASM)
    {
        CloseWindow(hwndASM);
        DestroyWindow(hwndASM);
    }
}
//-------------------------------------------------------------------------

HWND CreateASMWindow(void)
{
    if (hwndASM)
    {
        SendMessage(hwndASM, WM_SETFOCUS, 0, 0);
    }
    else
    {
        hwndASM = CreateMDIWindow(szASMClassName, szASMTitle, WS_VISIBLE |
           WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | MDIS_ALLCHILDSTYLES | 
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
        WS_SIZEBOX | (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndClient, hInstance, 
        NULL); 
    }
    return hwndASM;
}
