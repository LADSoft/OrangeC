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
#include "helpid.h"
#include "header.h"
#include "codecomp.h"
#include "regexp.h"
#include <ctype.h>


#define EDITOR_OFFSET 35
#define LINENO_DIGITS 7

extern PROJECTITEM *activeProject;
extern HWND hwndFindInternal;
extern HWND hwndSrcTab;
extern LOGFONT EditFont;
extern HINSTANCE hInstance;
extern HWND hwndClient, hwndStatus, hwndFrame, hwndASM;
extern HANDLE hMenuMain;
extern char szSourceFilter[];
extern char szNewFileFilter[];
extern enum DebugState uState;
extern char highlightText[256] ;
extern int highlightCaseSensitive;
extern int highlightWholeWord;
extern int finding;
extern SCOPE *activeScope;
extern SCOPE *StackList;
extern THREAD *activeThread, *stoppedThread;

POINT rightclickPos;
HANDLE editHeap;
char szDrawClassName[] = "xccDrawClass";
char szUntitled[] = "New File";

DWINFO *newInfo;

HANDLE ewSem;

DWINFO *editWindows;

HIMAGELIST tagImageList;

static DWORD ccThreadId;
static HANDLE ccThreadExit;
static BOOL stopCCThread;
void recolorize(DWINFO *ptr);
void AsyncOpenFile(DWINFO *newInfo);

void SetTitle(HWND hwnd);
void EditorRundown(void)
{
}
int xstricmpz(char *str1, char *str2)
{
    while (*str2)
        if (toupper(*str1++) != toupper(*str2++))
            return 1;
    return  *str1 !=  *str2;
} 
int xstricmp(char *str1, char *str2)
{
    while (*str1 && *str2)
    {
        if (toupper(*str1) != toupper(*str2))
            break;
        str1++, str2++;
    }
    if (toupper(*str1) != toupper(*str2))
        return toupper(*str1) < toupper(*str2) ? -1 : 1;
    return 0;
}

//-------------------------------------------------------------------------

char *stristr(char *str1, char *str2)
{
    int l = strlen(str2);
    while (*str1)
    {
        char *str3 = str1, *str4 = str2;
        while (*str3 && *str4)
        {
            if (toupper(*str3) != toupper(*str4))
                break;
            str3++, str4++;
        }
        if (!*str4)
            return str1;
        str1++;
    }
    return 0;
}
void ResetEditTitles(void )
{
    DWINFO *ptr = editWindows;
    while (ptr)
    {
        SetTitle(ptr->self);
        ptr = ptr->next;
    }
}

void rehighlight(char *text, int whole, int casesensitive)
{
    DWINFO *ptr = editWindows;
    strcpy(highlightText, text);
    highlightWholeWord = whole;
    highlightCaseSensitive = casesensitive;
    while (ptr)
    {
        recolorize(ptr);
        ptr = ptr->next;
    }
}
BOOL CALLBACK doSysMenu(HWND hwnd, LPARAM hide)
{
    if (hide)
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) &~WS_SYSMENU);
    else
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_SYSMENU);
    return TRUE;
}
void doMaximize(void)
{
    BOOL state;
    HWND hwnd = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&state);
    if (state != PropGetInt(NULL, "TABBED_WINDOWS"))
    {
        EnumChildWindows(hwndClient, doSysMenu, PropGetInt(NULL, "TABBED_WINDOWS"));
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_MAXIMIZEBOX);
        SendMessage(hwndClient, PropGetInt(NULL, "TABBED_WINDOWS") ? WM_MDIMAXIMIZE : WM_MDIRESTORE, (WPARAM)hwnd, 0);
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) &~WS_MAXIMIZEBOX);
    }
}
//-------------------------------------------------------------------------
void ApplyEditorSettings(void)
{
    DWINFO *ptr = editWindows;
    LOGFONT lf;
    int maximized = PropGetInt(NULL, "TABBED_WINDOWS");
    int tabs = PropGetInt(NULL, "TAB_INDENT") * 4;
    doMaximize();
    memcpy(&lf, &EditFont, sizeof(lf));
    PropGetFont(NULL, "FONT", &lf);	
    LoadColors();
    while (ptr)
    {
        HFONT fnt = CreateFontIndirect(&lf);
        PostMessage(ptr->dwHandle, WM_SETFONT, (WPARAM)
            fnt, 1);
        PostMessage(ptr->dwHandle, EM_SETTABSTOPS, 0, tabs);
        PostMessage(ptr->dwHandle, WM_SETEDITORSETTINGS,
            0, 0);
        PostMessage(ptr->self, WM_SETLINENUMBERMODE, 0, 0);
        ptr = ptr->next;
    }
}

//-------------------------------------------------------------------------

void InvalidateByName(char *name)
{
    DWINFO *ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, name);
    while (ptr)
    {
        if (SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)
            &info))
        {
            InvalidateRect(ptr->dwHandle, 0, 0);
            InvalidateRect(ptr->self, 0, 0);
        }
        ptr = ptr->next;
    }
}
DWINFO *GetFileInfo(char *name)
{
    DWINFO *ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, name);
    while (ptr)
    {
        if (SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)
            &info))
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}
void EditRenameFile(char *oldName, char *newName)
{
    DWINFO *ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, oldName);
    while (ptr)
    {
        if (SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)
            &info))
        {
            char *p;
            SendMessage(hwndSrcTab, TABM_RENAME, (WPARAM)newName, (LPARAM)ptr->self);
            strcpy(ptr->dwName, newName);
            p=strrchr(newName, '\\');
            if (p)
                p++;
            else 
                p = newName;
            strcpy(ptr->dwTitle, p);
            SetTitle(ptr->self);
        }
        ptr = ptr->next;
    }
}

//-------------------------------------------------------------------------

int ApplyBreakAddress(char *module, int linenum)
{
    char nmodule[260];
    nmodule[0] = 0;
    TagBreakpoint(module, linenum);
    if (linenum)
    {
        char *p;
        static DWINFO x;
        // there was a call to FindModuleName
        if (module[0])
        {
            strcpy(x.dwName, module);
            p = strrchr(module, '\\');
            if (p)
                strcpy(x.dwTitle, p + 1);
            else
                strcpy(x.dwTitle, module);
            x.dwLineNo = BPLine(x.dwName);
            x.logMRU = TRUE;
            x.newFile = FALSE ;
            CreateDrawWindow(&x, TRUE);
        }
    }
}


//-------------------------------------------------------------------------

void SaveDrawAll(void)
{
    DWINFO *ptr = editWindows;
    while (ptr)
    {
        if (SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0))
            SendMessage(ptr->self, WM_COMMAND, IDM_SAVE, 0);
        ptr = ptr->next;
    }
}

//-------------------------------------------------------------------------

int AnyModified(void)
{
    DWINFO *ptr = editWindows;
    int rv = 0;
    while (ptr)
    {
        rv |= SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0);
        ptr = ptr->next;
    }
    return rv;
}

//-------------------------------------------------------------------------

void CloseAll(void)
{
    
    DWINFO *ptr;
    MSG msg;
    ptr = editWindows;
    while (ptr)
    {
        ShowWindow(ptr->self, SW_HIDE);
        ptr = ptr->next;
    }
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        ProcessMessage(&msg);
    ptr = editWindows;
    while (ptr)
    {
        HANDLE xx = ptr->self;
        ptr = ptr->next;
        PostMessage(xx, WM_CLOSE, 0, 0);
    }
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        ProcessMessage(&msg);
}

//-------------------------------------------------------------------------

void RedrawAllBreakpoints(void)
{
    DWINFO *ptr = editWindows;
    while (ptr)
    {
        InvalidateRect(ptr->self, 0, 0);
        ptr = ptr->next;
    }
}

//-------------------------------------------------------------------------

char *GetEditData(HWND hwnd)
{
    int l;
    char *buf;
    l = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
//	buf = HeapAlloc(editHeap, HEAP_ZERO_MEMORY, l + 1);
    buf = calloc(l+1, 1);
    if (!buf)
    {
        return 0;
    }
    SendMessage(hwnd, WM_GETTEXT, l+1, (LPARAM)buf);
    
    return buf;
}

void FreeEditData(char *buf)
{
    free(buf);
//	HeapFree(editHeap, 0, buf);
//	HeapCompact(editHeap, 0);
}
//-------------------------------------------------------------------------

int SetEditData(HWND hwnd, char *buf, BOOL savepos)
{
    SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), WM_SETTEXT, savepos, (LPARAM)buf);
    FreeEditData(buf);
    return TRUE;
}

//-------------------------------------------------------------------------

static void backup(char *name)
{
    char newname[256], buffer[512];
    char *s;
    HANDLE in, out;
    BY_HANDLE_FILE_INFORMATION info;
    strcpy(newname, name);
    s = strrchr(newname, '.');
    strcat(newname, ".bak");

    in = CreateFile(name, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (in == INVALID_HANDLE_VALUE)
        return ;
    if (!GetFileInformationByHandle(in, &info))
    {
        CloseHandle(in);
        return ;
    }
    out = CreateFile(newname, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (out == INVALID_HANDLE_VALUE)
    {
        ExtendedMessageBox("File Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, 
            "Backup file is not writeable");
        CloseHandle(in);
        return ;
    }
    while (1)
    {
        DWORD read, written;
        if (!ReadFile(in, buffer, 512, &read, 0) || !read)
            break;
        WriteFile(out, buffer, read, &written, 0);
    }
    SetFileTime(out, &info.ftCreationTime, &info.ftLastAccessTime,
        &info.ftLastWriteTime);
    CloseHandle(out);
    CloseHandle(in);
}

//-------------------------------------------------------------------------

int SaveFile(HWND hwnd, DWINFO *info)
{
    char *buf = GetEditData(GetDlgItem(hwnd, ID_EDITCHILD));
    FILE *out;

    if (PropGetBool(NULL, "BACKUP_FILES"))
        backup(info->dwName);
    if (!buf)
        return FALSE;
    if (info->dosStyle)
        out = fopen(info->dwName, "w");
    else
        out = fopen(info->dwName, "wb");
    if (!out)
    {
        ExtendedMessageBox("File Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, 
            "Output file is not writeable");
        free(buf);
        return FALSE;
    }
    fputs(buf, out);
    fclose(out);
    FreeEditData(buf);
    FileTime(&info->time, info->dwName);
    return TRUE;
}

//-------------------------------------------------------------------------

int LoadFile(HWND hwnd, DWINFO *info, BOOL savepos)
{
    long size;
    char *buf,*p,*q;
    FILE *in = fopen(info->dwName, "rb");
    info->dosStyle = FALSE;
    if (!in)
    {
        info->dosStyle = TRUE;
        ShowWindow(info->dwHandle, SW_SHOW);
        return FALSE;
    }
    fseek(in, 0L, SEEK_END);
    size = ftell(in);
    buf = calloc(size+1, 1);
    if (!buf)
    {
        info->dosStyle = TRUE;
        fclose(in);
        ShowWindow(info->dwHandle, SW_SHOW);
        return FALSE;
    }
    fseek(in, 0L, SEEK_SET);
    size = fread(buf, 1, size, in);
    if (size < 0)
        size = 0;
    buf[size] = 0;
    fclose(in);
    p = q = buf;
    while (*p)
    {
        if (*p != '\r')
        {
            *q++ = *p++;
        }
        else
        {
            info->dosStyle = TRUE;
            p++;
        }
    }
    *q = 0;
    SetEditData(hwnd, buf, savepos);
    SendMessage(info->dwHandle, EM_SETMODIFY, 0, 0);
    recolorize(info);
    if (FileAttributes(info->dwName) &FILE_ATTRIBUTE_READONLY)
        SendMessage(info->dwHandle, EM_SETREADONLY, 1, 0);
    FileTime(&info->time, info->dwName);
    return TRUE;

}

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
void SetTitle(HWND hwnd)
{
    DWINFO *info = (DWINFO *)GetWindowLong(hwnd, 0);
    char buf[MAX_PATH];
    EDITDATA *dt = (EDITDATA *)SendMessage(info->dwHandle, EM_GETEDITDATA, 0, 0);
    int mod = SendMessage(info->dwHandle, EM_GETMODIFY, 0, 0);
    strcpy(buf, info->dwName);
    if (buf[0] == 0)
        strcpy(buf, info->dwTitle);
    if (activeProject)
    {
        strcpy(buf, relpath(buf, activeProject->realName));
    }
    else
    {
        char dir[MAX_PATH];
        dir[0] = 0;
        GetCurrentDirectory(MAX_PATH, dir);
        strcat(dir,"\\hithere");
        strcpy(buf, relpath(buf, dir));
    }
    if (dt->id)
        sprintf(buf + strlen(buf), " (%d)", dt->id + 1);
    if (mod)
        strcat(buf, " *");
    SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)buf);
    SendMessage(hwndSrcTab, TABM_SETMODIFY, mod, (LPARAM)hwnd);
}
void drawParams(DWINFO *info, HWND hwnd)
{
    char buf[512];
    int start, ins, col, sel;
    int readonly = SendMessage(info->dwHandle, EM_GETREADONLY, 0, 0);
    int mod = SendMessage(info->dwHandle, EM_GETMODIFY, 0, 0);
    int maxLines = SendMessage(info->dwHandle, EM_GETLINECOUNT, 0, 0) + 1; 
    int textSize = SendMessage(info->dwHandle, EM_GETSIZE, 0, 0);
    CHARRANGE a;
    SendMessage(info->dwHandle, EM_GETSEL, (WPARAM) &sel, 0);
    SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_EXGETSEL, 0, (LPARAM) &a);
    sel = a.cpMin;
    start = SendMessage(info->dwHandle, EM_EXLINEFROMCHAR, 0, sel);
    ins = SendMessage(info->dwHandle, EM_GETINSERTSTATUS, 0, 0);
    col = SendMessage(info->dwHandle, EM_GETCOLUMN, 0, 0);
    sprintf(buf, "Size: %d", textSize);
    SendMessage(hwndStatus, SB_SETTEXT, 1 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Lines: %d", maxLines);
    SendMessage(hwndStatus, SB_SETTEXT, 2 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Line: %d", start + 1);
    SendMessage(hwndStatus, SB_SETTEXT, 3 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Col: %d", col + 1);
    SendMessage(hwndStatus, SB_SETTEXT, 4 | SBT_NOBORDERS, (LPARAM)buf);
    SendMessage(hwndStatus, SB_SETTEXT, 5 | SBT_NOBORDERS, (LPARAM)(ins ? "INS" : "OVR"));
    if (readonly)
        SendMessage(hwndStatus, SB_SETTEXT, 6 | SBT_NOBORDERS, (LPARAM)("READ-ONLY"));
    else
        SendMessage(hwndStatus, SB_SETTEXT, 6 | SBT_NOBORDERS, (LPARAM)(mod ? "MODIFIED" : 
            "    "));

    SetTitle(hwnd);
    if (info->jumplistLineno != start)
    {
        info->jumplistLineno = start;
        SetJumplistPos(hwnd, start+1);
    }
}

//-------------------------------------------------------------------------

void eraseParams(HWND hwnd)
{
    SendMessage(hwndStatus, SB_SETTEXT, 1 | SBT_NOBORDERS, (LPARAM)"    ");
    SendMessage(hwndStatus, SB_SETTEXT, 2 | SBT_NOBORDERS, (LPARAM)"    ");
    SendMessage(hwndStatus, SB_SETTEXT, 3 | SBT_NOBORDERS, (LPARAM)"    ");
    SendMessage(hwndStatus, SB_SETTEXT, 4 | SBT_NOBORDERS, (LPARAM)"    ");

}
static int GetLog10(int val)
{
    int count = 1;
    if (val > 9999999)
        count = 8;
    else if (val > 999999)
        count = 7;
    else if (val > 99999)
        count = 6;
    else if (val > 9999)
        count = 5;
    else if (val > 999)
        count = 4;
    else if (val > 99)
        count = 3;
    else if (val > 9)
        count = 2;
    return count;
}
//-------------------------------------------------------------------------

int PaintBreakpoints(HWND hwnd, HDC dc, PAINTSTRUCT *paint, RECT *rcl)
{
    int count;
    HBRUSH graybrush, graybrush1;
    RECT r, r1;
    int i;
    DWINFO *ptr = (DWINFO*)GetWindowLong(hwnd, 0);
    int linenum = SendMessage(ptr->dwHandle, EM_GETFIRSTVISIBLELINE, 0, 0) + 1;
    int chpos1 = SendMessage(ptr->dwHandle, EM_LINEINDEX, linenum, 0);
    int maxLines = SendMessage(ptr->dwHandle, EM_GETLINECOUNT, 0, 0); 
    int ypos;
    int lines, offset = 0;
    int height;
    int *lt;
    int lc;
    int oldbk = SetBkColor(dc, RetrieveSysColor(COLOR_BTNFACE));
    int oldfg = SetTextColor(dc, RetrieveSysColor(COLOR_BTNTEXT));
    HPEN linePen;
    HFONT xfont;
    POINTL pt;
    int bpline = BPLine(ptr->dwName);
    HDC drawDC;
    HBITMAP drawBitmap;
    SendMessage(ptr->dwHandle, EM_POSFROMCHAR, (WPARAM) &pt, chpos1);
    ypos = pt.y;

    SendMessage(ptr->dwHandle, EM_GETRECT, 0, (LPARAM) &r1);
    drawDC = CreateCompatibleDC(dc);
    drawBitmap = CreateCompatibleBitmap(dc, rcl->right, rcl->bottom);

    drawBitmap = SelectObject(drawDC, drawBitmap);
    lines = r1.bottom / (height = SendMessage(ptr->dwHandle, EM_GETTEXTHEIGHT,
        0, 0));

    count = GetLog10(linenum + lines);
    if (count > ptr->lineNumberDigits)
    {
        PostMessage(hwnd, WM_SETLINENUMBERMODE, count, 0);
        // will invalidate the rect and force it to repaint
        return 0;
    }

    if (ypos < height)
        offset = ypos - height;

    graybrush = CreateSolidBrush(RetrieveSysColor(COLOR_BTNFACE));
    FillRect(drawDC, rcl, graybrush);
    DeleteObject(graybrush);
    linePen = CreatePen(PS_SOLID, 0, RetrieveSysColor(COLOR_BTNSHADOW));
    linePen = SelectObject(drawDC, linePen);
    MoveToEx(drawDC, ptr->editorOffset - 1, 0, 0);
    LineTo(drawDC, ptr->editorOffset - 1, rcl->bottom);
    linePen = SelectObject(drawDC, linePen);
    DeleteObject(linePen);
    if (uState != notDebugging)
    {
        lt = GetLineTable(ptr->dwName, linenum, &lc);
        if (lt)
        {
            int j = 0;
            graybrush1 = RetrieveSysBrush(COLOR_HIGHLIGHT);
            r.left = ptr->editorOffset - 5;
            r.right = ptr->editorOffset - 1;
            for (i = linenum; i <= linenum + lines; i++)
            {
                int k;
                int oldline = TagOldLine(ptr->dwName, i);
                while (lt[j] < oldline)
                    j++;
                for (k=j; k < lc; k++)
                    if (lt[k] == oldline)
                    {
                        r.top = offset + (i - linenum) *height;
                        r.bottom = r.top + height;
                        FillRect(drawDC, &r, graybrush1);
                        break;
                    }
            }
            DeleteObject(graybrush1);
            free(lt);
        }
    }
    xfont = (HFONT)SendMessage(ptr->dwHandle, WM_GETFONT, 0, 0);
    xfont = SelectObject(drawDC, xfont);
   for (i = linenum; i <= linenum + lines; i++)
    {
        int obj = -1;
        int type = IsTagged(ptr->dwName, i);
        switch (type)
        {
            case TAG_BP:
                if (bpline == i)
                {
                    obj = IML_STOPBP;
                }
                else
                {
                    obj = IML_BP;
                }
                break;
            case TAG_FIF1:
                obj = IML_FIF1;
                break;
            case TAG_FIF2:
                obj = IML_FIF2;
                break;
            case TAG_BOOKMARK:
                obj = IML_BOOKMARK;
                break;
            case TAG_BPGRAYED:
                obj = IML_BPGRAYED;
                break;
            case -1:            
                if (bpline == i)
                    obj = IML_STOP;
                else if (activeScope && !stricmp(activeScope->fileName, ptr->dwName) && 
                         i == TagNewLine(activeScope->fileName, activeScope->lineno))
                    if (StackList != activeScope)
                        obj = IML_CONTINUATION;
                break;
        }
        if (obj != -1)
        {
            ImageList_Draw(tagImageList, obj, drawDC, 12, (i - linenum) *height + offset, ILD_NORMAL);
        }
        if (ptr->editorOffset != EDITOR_OFFSET && i <= maxLines+1)
        {
            char buf[256];
            sprintf(buf, "%d", i);
            TextOut(drawDC, EDITOR_OFFSET - 4 + (ptr->lineNumberDigits - GetLog10(i)) * 
                    ((EDITDATA *)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0))->cd->txtFontWidth , 
                    (i - linenum) *height + offset, buf, strlen(buf));
        }
    }
    xfont = SelectObject(drawDC, xfont);
    SetTextColor(drawDC, oldfg);
    SetBkColor(drawDC, oldbk);


    BitBlt(dc, rcl->left, rcl->top, rcl->right - rcl->left, rcl->bottom - rcl->top,
           drawDC, rcl->left, rcl->top, SRCCOPY);


    drawBitmap = SelectObject(drawDC, drawBitmap);
    DeleteObject(drawBitmap);
    DeleteDC(drawDC);

    return 0;
}

//-------------------------------------------------------------------------

LRESULT CALLBACK gotoProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    char buf[3];
    switch (iMessage)
    {
        case WM_COMMAND:
            if (wParam == IDOK)
            {
                int i = GetEditFieldValue(hwnd, IDC_GOTO);
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
            EndDialog(hwnd, 0);
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            SetEditField(hwnd, IDC_GOTO, "");
            DisableControl(hwnd, IDOK, 1);
            return 1;
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

void recolorize(DWINFO *ptr)
{
    int language = LANGUAGE_NONE;
    if (stristr(ptr->dwName, ".c") == ptr->dwName + strlen(ptr->dwName) - 2 ||
        stristr(ptr->dwName, ".cpp") == ptr->dwName + strlen(ptr->dwName) - 4 
        || stristr(ptr->dwName, ".h") == ptr->dwName + strlen(ptr->dwName) - 2)
        language = LANGUAGE_C;
    else if (stristr(ptr->dwName, ".asm") == ptr->dwName + strlen(ptr->dwName) 
        - 4 || stristr(ptr->dwName, ".asi") == ptr->dwName + strlen(ptr->dwName)
        - 4 || stristr(ptr->dwName, ".inc") == ptr->dwName + strlen(ptr->dwName)
        - 4 || stristr(ptr->dwName, ".nas") == ptr->dwName + strlen(ptr->dwName)
        - 4)
        language = LANGUAGE_ASM;
    else if (stristr(ptr->dwName, ".rc") == ptr->dwName + strlen(ptr->dwName) - 3)
        language = LANGUAGE_RC;
    SendMessage(ptr->dwHandle, EM_LANGUAGE, 0, language);
}

//-------------------------------------------------------------------------

void asyncLoadFile(DWINFO *ptr)
{
    recolorize(ptr);
    if (ptr->dwName[0])
        LoadFile(ptr->self, ptr, FALSE);
    else
        ShowWindow(ptr->dwHandle, SW_SHOW);
    if (ptr->dwLineNo !=  - 1)
    {
        PostMessage(ptr->self, WM_COMMAND, IDM_SETLINE, ptr->dwLineNo);
    }
}
DWORD MsgWait(HANDLE event, DWORD timeout)
{
    while (1)
    {
        switch (MsgWaitForMultipleObjects(1, &event, FALSE, timeout, QS_ALLINPUT))
        {
            case WAIT_OBJECT_0:
            {
                return WAIT_OBJECT_0;
            }
            case WAIT_OBJECT_0 + 1:
            {
                MSG msg;
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                    ProcessMessage(&msg);
            }
                break;
            default:
                return WAIT_TIMEOUT;
        }
    }
}
static void installparse(char *name, BOOL remove)
{
    char *p = (char *)calloc(1, strlen(name) + 1);
    if (p)
    {
        strcpy(p, name);
        PostThreadMessage(ccThreadId, WM_USER, remove, (LPARAM)p);
    }
}
void InstallForParse(HWND hwnd)
{
    if (PropGetInt(NULL, "CODE_COMPLETION") != 0)
    {
        DWINFO *info = (DWINFO *)GetWindowLong(hwnd, 0);
        char *name = info->dwName;
        int len = strlen(name);
        if (name[len-2] == '.')
        {
            if (tolower(name[len - 1]) == 'c')
                installparse(name, FALSE);
            else if (tolower(name[len-1]) == 'h')
            {
                DWINFO *ptr = editWindows;
                while (ptr)
                {
                    if (IsWindow(ptr->self))
                    {
                        name = ptr->dwName;
                        len = strlen(name);
                        if (name[len -2] == '.' && tolower(name[len-1]) == 'c')
                            installparse(name, FALSE);
                    }
                    ptr = ptr->next;
                }
            }
        }
    }
}
void InstallAllForParse(void)
{
    DWINFO *ptr = editWindows;
    while (ptr)
    {
        InstallForParse(ptr->self);
        ptr = ptr->next;
    }
}
static void deleteParseData(char *name)
{
    int len = strlen(name);
    if (name[len-2] == '.')
    {
        if (tolower(name[len - 1]) == 'c')
        {
            installparse(name, TRUE);
        }
    }
}
void ScanParse(void)
{
    while (1)
    {
        MSG msg;
        GetMessage(&msg, NULL, 0, 0);
        if (msg.message == WM_USER)
        {
            if (msg.wParam)
                deleteFileData((char *)msg.lParam);
            else
                DoParse((char *)msg.lParam);
            free((void *)msg.lParam);
        }
        if (stopCCThread)
            break;
        Sleep(50);
    }
    SetEvent(ccThreadExit);
}
//-------------------------------------------------------------------------

LRESULT CALLBACK DrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    LRESULT rv;
    DWINFO *ptr,  *ptr1;
    EDITDATA *ed;
    OPENFILENAME ofn;
    HDC dc;
    RECT r;
    PAINTSTRUCT paint;
    LPCREATESTRUCT createStruct;
    int startpos, endpos, flag, i;
    NMHDR *nm;
    CHARRANGE s;
    char buf[256];
    switch (iMessage)
    {
        case WM_LBUTTONDOWN:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            if (!ptr->timing)
            {
                POINT pt;
                int linenum;
                RECT r;
                GetCursorPos(&pt);
                GetWindowRect(ptr->dwHandle, &r);
                pt.x = 0;
                pt.y -= r.top;
                GetClientRect(ptr->dwHandle, &r);
                if (pt.y < r.bottom - r.top)
                {
                    linenum = SendMessage(ptr->dwHandle, EM_CHARFROMPOS, 0, (LPARAM)&pt);
/*
                    if (uState == notDebugging)
                        ToggleBookMark(linenum);
                    else
*/
                    {
                        linenum = SendMessage(ptr->dwHandle, EM_EXLINEFROMCHAR, 0,
                            linenum) + 1;
                        Tag(TAG_BP, ptr->dwName, linenum, 0, 0, 0, 0);
                    }
                }
            }
            break;
        case EN_LINECHANGE:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            TagLineChange(ptr->dwName, wParam + 1, lParam);
            FileBrowseLineChange(ptr, wParam + 1, lParam);
            FindSetDirty();
            GetClientRect(hwnd, &r);
            r.right = ptr->editorOffset;
            InvalidateRect(hwnd, &r, 0);
            break;
        case WM_NOTIFY:
            nm = (NMHDR*)lParam;
            if (nm->code == NM_RCLICK)
            {
                HMENU menu = LoadMenuGeneric(hInstance, "EDITMENU");
                HMENU popup = GetSubMenu(menu, 0);
                POINT pos, pos1;
                RECT rect;
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                SendMessage(hwnd, EN_SETCURSOR, 0, 0);
                if (!SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0))
                    EnableMenuItem(menu, IDM_SAVE, MF_GRAYED);
                if (uState != atBreakpoint && uState != atException)
                {
                    EnableMenuItem(menu, IDM_RUNTO, MF_GRAYED);
                    EnableMenuItem(menu, IDM_ADDWATCHINDIRECT, MF_GRAYED);
                    EnableMenuItem(menu, IDM_DATABREAKPOINTINDIRECT, MF_GRAYED);
                }
                GetCursorPos(&pos);
                pos1.x = pos.x;
                pos1.y = pos.y;
                rightclickPos = pos;
                GetWindowRect(ptr->dwHandle, &rect);
                rightclickPos.x -= rect.left;
                rightclickPos.y -= rect.top;
                InsertBitmapsInMenu(popup);
                TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x,
                    pos.y, hwndFrame, NULL);
                DestroyMenu(menu);
                return 0;
            }
            break;
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                SendMessage(hwnd, WM_COMMAND, IDM_CLOSE, 0);
                return 0;
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDM_CLOSEFIND:
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                SendMessage(ptr->dwHandle, WM_COMMAND, wParam, lParam);
                break;
            case IDM_COPYWINDOW:
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                newInfo = ptr ;
                openfile(ptr, TRUE, TRUE);
                break ;
            case IDM_SPECIFIEDHELP:
            case IDM_RTLHELP:
            case IDM_LANGUAGEHELP:
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                i = SendMessage(ptr->dwHandle, WM_COMMAND, wParam, lParam);
                break;
            case IDM_GOTO:
                lParam = DialogBox(hInstance, "GOTODIALOG", hwnd, (DLGPROC)
                    gotoProc);
                if (lParam == 0)
                    break;
                // fall through
            case IDM_SETLINE:
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                SendMessage(ptr->dwHandle, EM_GETLINECOUNT, 0, 0); 
                    // force update of vertical scroll range
                i = SendMessage(ptr->dwHandle, EM_LINEINDEX, lParam - 1, 0);
                s.cpMin = i;
                s.cpMax = i;
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_HIDESELECTION, 1, 0);
                SendMessage(ptr->dwHandle, EM_EXSETSEL, 0, (LPARAM) &s);
                SendMessage(ptr->dwHandle, EM_SCROLLCARET, 0, 1);
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_HIDESELECTION, 0, 0);
                drawParams(ptr, hwnd);
                InvalidateRect(hwnd, 0, 0);
                break;
            case ID_REDRAWSTATUS:
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                drawParams(ptr, hwnd);
                InvalidateRect(hwnd, 0, 0);
                break;
            case ID_QUERYHASFILE:
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                ptr1 = (DWINFO*)lParam;
                if (!xstricmpz(ptr->dwName, ptr1->dwName))
                {
                    return SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0);
                }
                return 0 ;
            case ID_QUERYSAVE:
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                rv = SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0);
                if (rv)
                {
                    return ExtendedMessageBox("File Changed",
                        MB_YESNOCANCEL, 
                        "File %s has changed.  Do you wish to save it?", ptr
                        ->dwTitle);
                }
                else
                    return IDNO;
            case IDM_SAVEAS:
                dialog: ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                if (!SaveFileDialog(&ofn, ptr->dwName, hwnd, TRUE,
                    szSourceFilter, 0))
                    break;
                if (activeProject && ptr->dwName[0] == 0)
                {
                    if (ExtendedMessageBox("Project Query", MB_TASKMODAL |
                        MB_YESNO, "Add file to active project?") == IDYES)
                    {
                            AddFile(activeProject, ofn.lpstrFile, TRUE);
                            
                    }
                }
                EditRenameFile(ptr->dwName, ofn.lpstrFile);
                strcpy(ptr->dwTitle, ofn.lpstrFileTitle);
                strcpy(ptr->dwName, ofn.lpstrFile);
                recolorize(ptr);
                SendMessage(ptr->dwHandle, EM_SETMODIFY, 1, 0);
            case IDM_SAVE:
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                if (LOWORD(wParam) == IDM_SAVE)
                {
                    rv = SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0);
                    if (!rv)
                        break;
                }
                if (ptr->dwName[0] == 0)
                    goto dialog;
                rv = SaveFile(hwnd, (char*)GetWindowLong(hwnd, 0));
                TagLinesAdjust(ptr->dwName, TAGM_SAVEFILE);
                SendMessage(ptr->dwHandle, EM_SETMODIFY, 0, 0);
                drawParams(ptr, hwnd);
                CalculateFileAutoDepends(ptr->dwName);
                return rv;
            case IDM_CLOSE:
                {
                    rv = SendMessage(hwnd, WM_COMMAND, ID_QUERYSAVE, 0);
                    switch (rv)
                    {
                    case IDYES:
                        if (SendMessage(hwnd, WM_COMMAND, IDM_SAVE, 0))
                           SendMessage(hwnd, WM_CLOSE, 0, 0);
                        break;
                    case IDNO:
                        ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                        TagLinesAdjust(ptr->dwName, TAGM_DISCARDFILE);
                        SendMessage(hwnd, WM_CLOSE, 0, 0);
                        break;
                    case IDCANCEL:
                        break;
                    }
                    return rv;
                }
                break;
            case IDM_UNDO:
                FindSetDirty();
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), WM_UNDO, 0, 0);
                break;
            case IDM_REDO:
                FindSetDirty();
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), WM_REDO, 0, 0);
                break;
            case IDM_CUT:
                FindSetDirty();
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), WM_CUT, 0, 0);
                break;
            case IDM_COPY:
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), WM_COPY, 0, 0);
                break;
            case IDM_PASTE:
                FindSetDirty();
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), WM_PASTE, 0, 0);
                break;
            case IDM_SELECTALL:
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_SETSEL, 0,  - 1);
                break;
            case IDM_FIND:
                OpenFindDialog();
                break;
            case IDM_REPLACE:
                OpenReplaceDialog();
                break;
            case IDM_FINDNEXT:
            {
                DWORD id;
                CloseHandle(CreateThread(NULL,0,
                                         (LPTHREAD_START_ROUTINE)(finding ? DoFindNext : DoReplaceNext), 
                                         hwndFindInternal, 0, &id));
                break;
            }
            case IDM_TOUPPER:
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_TOUPPER, 0, 0);
                break;
            case IDM_TOLOWER:
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_TOLOWER, 0, 0);
                break;
            case IDM_COMMENT:
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_SELECTCOMMENT, 0, 1);
                break;
            case IDM_UNCOMMENT:
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_SELECTCOMMENT, 0, 0);
                break;
            case IDM_INDENT:
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_SELECTINDENT, 0,
                    1);
                break;
            case IDM_UNINDENT:
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_SELECTINDENT, 0,
                    0);
                break;
            case EN_SETFOCUS:
                break;
            case EN_NEEDFOCUS:
                if ((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) == hwnd)
                    SendMessage(hwnd, WM_SETFOCUS, 0, 0);
                else
                    SendMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)hwnd, NULL);
                break;
            case EN_CHANGE:
            {
                CHARRANGE xx;
                int lineno;
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_EXGETSEL, 0, (LPARAM)&xx);
                lineno = SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_LINEFROMCHAR, xx.cpMin, 0);
                FileBrowseLineChange(ptr, lineno + 1, 0);
                FindSetDirty();
                break;
            }
            default:
                return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
            }
            
            break;
        case EM_CANREDO:
            return SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_CANREDO, 0, 0)
                ;
        case EM_CANUNDO:
            return SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_CANUNDO, 0, 0)
                ;
        case EN_SETCURSOR:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            drawParams(ptr, hwnd);
            GetClientRect(hwnd, &r);
            r.right = EDITOR_OFFSET - 3;
            InvalidateRect(hwnd, &r, 0);
            break;
        case WM_PAINT:
            GetClientRect(hwnd, &r);
            dc = BeginPaint(hwnd, &paint);
            /*
            hpen = CreatePen(PS_SOLID, 1, 0xcccccc);
            */
            /*
            oldpen = SelectObject(dc, hpen);
            MoveToEx(dc, EDITOR_OFFSET - 2, 0, 0);
            LineTo(dc, EDITOR_OFFSET - 2, r.bottom);
            SelectObject(dc, oldpen);
            DeleteObject(hpen);
            */
            PaintBreakpoints(hwnd, dc, &paint, &r);
            EndPaint(hwnd, &paint);
            return 0;
        case WM_CREATE:
            //         maximized = TRUE ;			
            rv = DefWindowProc(hwnd, iMessage, wParam, lParam);
            if (rv)
                return rv;
            createStruct = (LPCREATESTRUCT)lParam;
            ed = (EDITDATA *)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            ptr = calloc(1, sizeof(DWINFO));
            SetWindowLong(hwnd, 0, (int)ptr);
            SetWindowLong(hwnd, 4, (int)EDITSIG);
            if (ed != -1)
            {
                strcpy(ptr->dwTitle, newInfo->dwTitle);
                SetWindowText(hwnd, newInfo->dwTitle);
                strcpy(ptr->dwName, newInfo->dwName);
            }
            else
            {
                strcpy(ptr->dwTitle, szUntitled);
                SetWindowText(hwnd, ptr->dwTitle);
            }
            if (!ptr->newFile)
                FileTime(&ptr->time, ptr->dwName);
            ptr->dwHandle = CreateWindowEx(0, "xedit", 0, WS_CHILD +
                WS_CLIPSIBLINGS + WS_CLIPCHILDREN + WS_HSCROLL + WS_VSCROLL + ES_LEFT + WS_VISIBLE +
                ES_MULTILINE + ES_NOHIDESEL + ES_AUTOVSCROLL + ES_AUTOHSCROLL,
                EDITOR_OFFSET, 0, 0, 0, hwnd, (HMENU)ID_EDITCHILD, hInstance, 
                (ed && ed != (DWINFO *)-1) ? (void *)ed : NULL)
                ;
            ptr->self = hwnd;
            ptr->dwLineNo =  - 1;
            if (newInfo && !newInfo->newFile && newInfo->dwLineNo !=  -
                1)
            {
                ptr->dwLineNo = newInfo->dwLineNo;
                newInfo->dwLineNo =  - 1;
            }
            if (!ed && newInfo->logMRU)
            {
                InsertMRU(ptr, 0);
                MRUToMenu(0);
            }
            //         CloseHandle(CreateThread(0,0,(LPTHREAD_START_ROUTINE)asyncLoadFile,(LPVOID)ptr,0,&threadhand)) ;
            if (!ed)
            {
                if (ptr->dwName[0] && (!newInfo || !newInfo->newFile))
                    LoadFile(ptr->self, ptr, FALSE);
                if (ptr->dwLineNo !=  - 1)
                {
                    PostMessage(ptr->self, WM_COMMAND, IDM_SETLINE, ptr->dwLineNo);
                }
                recolorize(ptr);
            }
            else
            {
                ptr->dosStyle = TRUE;
            }
            MsgWait(ewSem, INFINITE);
            if (editWindows)
                editWindows->prev = ptr;
            ptr->next = editWindows;
            ptr->prev = NULL;
            editWindows = ptr;
            SetEvent(ewSem);
            SendMessage(hwnd, WM_SETLINENUMBERMODE, 2, 0);
            ptr->editData = (EDITDATA *)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0);
            strcpy(buf, ptr->dwName);
            if (ptr->editData->id)
                sprintf(buf + strlen(buf), " (%d)", ptr->editData->id + 1);
            SendMessage(hwndSrcTab, TABM_ADD, (WPARAM)buf, (LPARAM)hwnd);
            return rv;

        case WM_DEFERREDCLOSE:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            ptr->deferClose = FALSE;
            if (ptr->shouldClose)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        case WM_CLOSE:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            if (ptr->deferClose)
            {
                ptr->shouldClose = TRUE;
                return 0;
            }
            EndFind();
            FileBrowseClose(ptr);
            deleteParseData(ptr->dwName);
            eraseParams(ptr->dwHandle);
            MsgWait(ewSem, INFINITE);
            if (ptr->prev)
                ptr->prev->next = ptr->next;
            if (ptr->next)
                ptr->next->prev = ptr->prev;
            if (ptr == editWindows)
                editWindows = ptr->next;
            SetEvent(ewSem);
            break;
        case WM_SETSIBLINGID:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            SetTitle(hwnd);
            strcpy(buf, ptr->dwName);
            if (ptr->editData->id)
                sprintf(buf + strlen(buf), " (%d)", ptr->editData->id + 1);
            SendMessage(hwndSrcTab, TABM_RENAME, (WPARAM)buf, (LPARAM)ptr->self);
            break;
        case WM_DESTROY:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            PostMessage(hwndSrcTab, TABM_REMOVE, 0, (LPARAM)hwnd);
            free(ptr);
            PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
            break;
        case WM_SIZE:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            MoveWindow(GetDlgItem(hwnd, ID_EDITCHILD), ptr->editorOffset, 0, 
                (lParam &65535) - ptr->editorOffset, lParam >> 16, 1);
            break;
        // timer being used to prevent a click in the margin which activates
        // the window from setting a breakpoint...
        // it also is used to re-establish the focus as some instances with
        // using an OPENFILEDIALOG create weird timing conditions that don't
        // change the focus...
        case WM_TIMER:
            KillTimer(hwnd, 1000);
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            ptr->timing = FALSE;
            if ((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) == hwnd)
                    PostMessage(hwnd, WM_COMMAND, EN_NEEDFOCUS, 0);
            break;
        case WM_MDIACTIVATE:
            if ((HWND)lParam != hwnd)
            {
                break;
            }
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            ptr->jumplistLineno = -1;
            MsgWait(ewSem, INFINITE);
            if (ptr->prev)
                ptr->prev->next = ptr->next;
            else
                editWindows = ptr->next;
            if (ptr->next)
                ptr->next->prev = ptr->prev;
            if (editWindows)
                editWindows->prev = ptr;
            ptr->next = editWindows;
            ptr->prev = NULL;
            editWindows = ptr;
            SetEvent(ewSem);
            ptr->timing = TRUE;
            SetTimer(hwnd, 1000, 100, NULL);
            doMaximize();
            break;
        case WM_SETFOCUS:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            drawParams(ptr, hwnd);
            InvalidateRect(hwnd, 0, 0);
            PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
            PostMessage(hwndSrcTab, TABM_SELECT, 0, (LPARAM)hwnd);
            SetFocus(GetDlgItem(hwnd, ID_EDITCHILD));
            SetResourceProperties(NULL, NULL);
            return 0;
        case WM_KILLFOCUS:
//			SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), iMessage, wParam, lParam);
            break;
            return 0;
        case WM_SETLINENUMBERMODE:
        {
            BOOL lineNumbers = PropGetBool(NULL, "LINE_NUMBERS");
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            if (wParam)
            {
                    ptr->lineNumberDigits = wParam; // never goes down while the window is open...
            }
            ptr->editorOffset = EDITOR_OFFSET +  
                lineNumbers * (ptr->lineNumberDigits * ((EDITDATA *)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0))->cd->txtFontWidth + 4);
            GetClientRect(hwnd, &r);
            MoveWindow(GetDlgItem(hwnd, ID_EDITCHILD), ptr->editorOffset, r.top, 
                r.right - ptr->editorOffset, r.bottom - r.top, 1);
            InvalidateRect(hwnd, 0, 1);
            break;
        }
        case WM_INITMENUPOPUP:
            SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_GETSEL, (WPARAM)
                &startpos, (LPARAM) &endpos);
            flag = startpos < endpos;
            EnableMenuItem(hMenuMain, IDM_CUT, flag);
            EnableMenuItem(hMenuMain, IDM_COPY, flag);
            EnableMenuItem(hMenuMain, IDM_PASTE, 1);
            EnableMenuItem(hMenuMain, IDM_UNDO, SendMessage(GetDlgItem(hwnd,
                ID_EDITCHILD), EM_CANUNDO, 0, 0));
            EnableMenuItem(hMenuMain, IDM_BROWSE, flag);
            //EnableMenuItem(hMenuMain,IDM_BROWSEBACK,flag) ;
            EnableMenuItem(hMenuMain, IDM_BOOKMARK, flag);
            //EnableMenuItem(hMenuMain,IDM_NEXTBOOKMARK,flag) ;
            //EnableMenuItem(hMenuMain,IDM_PREVBOOKMARK,flag) ;
            return 0;
        case WM_WORDUNDERCURSOR:
        case WM_WORDUNDERPOINT:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            {
                int rv = SendMessage(ptr->dwHandle, iMessage, wParam, lParam);
                return rv;
            }
        case WM_FILETITLE:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            return ptr->dwTitle;
        case WM_FILENAME:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            return ptr->dwName;
        default:
            if (iMessage >= WM_USER)
                return SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), iMessage, wParam, lParam);
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterDrawWindow(void)
{
    HBITMAP bitmap;
    WNDCLASS wc;
//	editHeap = HeapCreate(0, 2 * 1024 * 1024, 128  * 1024 * 1024);
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = &DrawProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(void*) * 2;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szDrawClassName;
    RegisterClass(&wc);
    bitmap = LoadBitmap(hInstance, "ID_TAG");
    ChangeBitmapColor(bitmap, 0xc0c0c0, RetrieveSysColor(COLOR_BTNFACE));
    tagImageList = ImageList_Create(16, 16, ILC_COLOR24, ILEDIT_IMAGECOUNT, 0);
    ImageList_Add(tagImageList, bitmap, NULL);
    DeleteObject(bitmap);
    
    ccThreadExit = CreateEvent(0,0,0,0);
    ewSem = CreateEvent(NULL, FALSE, TRUE, NULL);
    CloseHandle(CreateThread(0,0, (LPTHREAD_START_ROUTINE)ScanParse, 0, 0, &ccThreadId));
}

//-------------------------------------------------------------------------

HWND openfile(DWINFO *newInfo, int newwindow, int visible)
{
    HWND rv ;
    void *extra = newInfo == (DWINFO *)-1 ? newInfo : NULL ;
    MSG msg;
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        ProcessMessage(&msg);
    if (newInfo && newInfo != (DWINFO*) - 1)
    {
        DWINFO *ptr = editWindows;
        while (ptr)
        {
            if (SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, 
                (LPARAM)newInfo))
            {
                if (newwindow)
                {
                    extra = (EDITDATA *)SendMessage(ptr->self, EM_GETEDITDATA, 0, 0);
                }
                else
                {
                    HWND active = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                    if (GetParent(hwndASM) == active || ptr->self == active)
                    {
                        SetFocus(active);
                    }
                    else
                    {
                        PostMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)ptr->self, 0);
                    }
                    if (newInfo->dwLineNo !=  - 1)
                            PostMessage(ptr->self, WM_COMMAND, IDM_SETLINE, newInfo
                                ->dwLineNo);
                    return ptr->self;
                }
            }
            ptr = ptr->next;
        }
    }
    rv = CreateMDIWindow(szDrawClassName, szUntitled, (visible ? WS_VISIBLE : 0) |
           WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | MDIS_ALLCHILDSTYLES | 
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
        WS_SIZEBOX | (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU),
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndClient, hInstance, 
        (LPARAM)extra); 
    return rv;
}
//-------------------------------------------------------------------------

HWND CreateDrawWindow(DWINFO *baseinfo, int visible)
{
    static DWINFO temp;
    OPENFILENAME ofn;

    newInfo = baseinfo;
    if (!newInfo)
    {
        newInfo = &temp;
        newInfo->dwLineNo =  - 1;
        newInfo->logMRU = TRUE;
        newInfo->newFile = FALSE;
        if (OpenFileDialog(&ofn, 0, 0, FALSE, TRUE, szSourceFilter, 0))
        {
            char *q = ofn.lpstrFile, path[256];
            strcpy(path, ofn.lpstrFile);
            q += strlen(q) + 1;
            if (! *q)
            {
                strcpy(newInfo->dwTitle, ofn.lpstrFileTitle);
                strcpy(newInfo->dwName, ofn.lpstrFile);
            }
            else
            {
                while (*q)
                {
                    strcpy(newInfo->dwTitle, q);
                    sprintf(newInfo->dwName, "%s\\%s", path, q);
                    openfile(newInfo, FALSE, visible);
                    q += strlen(q) + 1;
                }
                return 0;
            }
        }
        else
        {

            //               ExtendedMessageBox("File Open",MB_SETFOREGROUND | MB_SYSTEMMODAL,"Could not open file %s %d",newInfo->dwName,GetLastError()) ;
            return 0;
        }
    } else if (newInfo == (DWINFO *)-1)
    {
        newInfo = &temp;
        newInfo->dwLineNo =  - 1;
        newInfo->logMRU = TRUE;
        newInfo->newFile = TRUE;
        if (SaveFileDialog(&ofn, 0, 0, TRUE, szNewFileFilter, "Open New File"))
        {
            char *q = ofn.lpstrFile, path[256];
            strcpy(path, ofn.lpstrFile);
            q += strlen(q) + 1;
            if (! *q)
            {
                strcpy(newInfo->dwTitle, ofn.lpstrFileTitle);
                strcpy(newInfo->dwName, ofn.lpstrFile);
            }
            else
            {
                while (*q)
                {
                    strcpy(newInfo->dwTitle, q);
                    sprintf(newInfo->dwName, "%s\\%s", path, q);
                    openfile(newInfo, FALSE, visible);
                    q += strlen(q) + 1;
                }
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    return openfile(newInfo, FALSE, visible);
}
