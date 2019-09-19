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
#include "helpid.h"
#include "header.h"
#include "regexp.h"
#include <ctype.h>
#include <process.h>

#define EDITOR_OFFSET 24
#define LINENO_DIGITS 7

extern PROJECTITEM* activeProject;
extern HWND hwndFindInternal;
extern HWND hwndSrcTab;
extern LOGFONT EditFont;
extern HINSTANCE hInstance;
extern HWND hwndClient, hwndStatus, hwndFrame;
extern HANDLE hMenuMain;
extern char szSourceFilter[];
extern char szNewFileFilter[];
extern enum DebugState uState;
extern char highlightText[256];
extern int highlightCaseSensitive;
extern int highlightWholeWord;
extern int finding;
extern SCOPE* activeScope;
extern SCOPE* StackList;
extern THREAD *activeThread, *stoppedThread;
extern HWND hwndeditPopup;
extern PROCESS* activeProcess;

POINT rightclickPos;
HANDLE editHeap;
char szDrawClassName[] = "xccDrawClass";
char szUntitled[] = "New File";

DWINFO* newInfo;

HANDLE ewSem;

DWINFO* editWindows;

HIMAGELIST tagImageList;

struct _ccList
{
    struct _ccList* next;
    char* name;
    int remove;
};

struct _ccList* codeCompList;

static unsigned int ccThreadId;
static HANDLE ccThreadExit, ccThreadGuard;
static DWINFO* ewQueue;
BOOL stopCCThread;
void recolorize(DWINFO* ptr);
void AsyncOpenFile(DWINFO* newInfo);

void SetTitle(HWND hwnd);
void EditorRundown(void) {}
int xstricmpz(const char* str1, const char* str2)
{
    while (*str2)
        if (toupper(*str1++) != toupper(*str2++))
            return 1;
    return *str1 != *str2;
}
int xstricmp(const char* str1, const char* str2)
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

char* stristr(char* str1, char* str2)
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
DWINFO* DeQueueEditWindow(void)
{
    // these are never freed but will be reused...
    // we don't free them because these items are used
    // across threads and we don't want the memory going away or being
    // reused for other purposes while it is being used
    DWINFO* rv = NULL;
    if (ewQueue)
    {
        rv = ewQueue;
        ewQueue = ewQueue->next;
    }
    else
    {
        rv = malloc(sizeof(DWINFO));
    }
    memset(rv, 0, sizeof(DWINFO));
    return rv;
}
void EnQueueEditWindow(DWINFO* ptr)
{
    ptr->active = FALSE;
    ptr->next = ewQueue;
    ewQueue = ptr;
}
void ResetEditTitles(void)
{
    DWINFO* ptr = editWindows;
    while (ptr)
    {
        if (ptr->active)
            SetTitle(ptr->self);
        ptr = ptr->next;
    }
}

void rehighlight(char* text, int whole, int casesensitive)
{
    DWINFO* ptr = editWindows;
    strcpy(highlightText, text);
    highlightWholeWord = whole;
    highlightCaseSensitive = casesensitive;
    while (ptr)
    {
        if (ptr->active)
            recolorize(ptr);
        ptr = ptr->next;
    }
}
BOOL CALLBACK doSysMenu(HWND hwnd, LPARAM hide)
{
    if (hide)
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SYSMENU);
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
        //        EnumChildWindows(hwndClient, doSysMenu, PropGetInt(NULL, "TABBED_WINDOWS"));
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_MAXIMIZEBOX);
        SendMessage(hwndClient, PropGetInt(NULL, "TABBED_WINDOWS") ? WM_MDIMAXIMIZE : WM_MDIRESTORE, (WPARAM)hwnd, 0);
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    }
}
//-------------------------------------------------------------------------
void ApplyEditorSettings(void)
{
    DWINFO* ptr = editWindows;
    LOGFONT lf;
    int maximized = PropGetInt(NULL, "TABBED_WINDOWS");
    int tabs = PropGetInt(NULL, "TAB_INDENT") * 4;
    doMaximize();
    memcpy(&lf, &EditFont, sizeof(lf));
    PropGetFont(NULL, "FONT", &lf);
    LoadColors();
    while (ptr)
    {
        if (ptr->active)
        {
            HFONT fnt = CreateFontIndirect(&lf);
            PostMessage(ptr->dwHandle, WM_SETFONT, (WPARAM)fnt, 1);
            PostMessage(ptr->dwHandle, EM_SETTABSTOPS, 0, tabs);
            PostMessage(ptr->dwHandle, WM_SETEDITORSETTINGS, 0, 0);
            PostMessage(ptr->self, WM_SETLINENUMBERMODE, 0, 0);
        }
        ptr = ptr->next;
    }
}

//-------------------------------------------------------------------------

void InvalidateByName(char* name)
{
    DWINFO* ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, name);
    while (ptr)
    {
        if (ptr->active && SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)&info))
        {
            InvalidateRect(ptr->dwHandle, 0, 0);
            InvalidateRect(ptr->self, 0, 0);
        }
        ptr = ptr->next;
    }
}
DWINFO* GetFileInfo(char* name)
{
    DWINFO* ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, name);
    while (ptr)
    {
        if (ptr->active && SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)&info))
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}
void EditRenameFile(char* oldName, char* newName)
{
    DWINFO* ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, oldName);
    while (ptr)
    {
        if (ptr->active && SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)&info))
        {
            char* p;
            SendMessage(hwndSrcTab, TABM_RENAME, (WPARAM)newName, (LPARAM)ptr->self);
            strcpy(ptr->dwName, newName);
            p = strrchr(newName, '\\');
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

void ApplyBreakAddress(char* module, int linenum)
{
    char nmodule[260];
    nmodule[0] = 0;
    TagBreakpoint(module, linenum);
    if (linenum)
    {
        char* p;
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
            x.newFile = FALSE;
            CreateDrawWindow(&x, TRUE);
        }
    }
}

//-------------------------------------------------------------------------

void SaveDrawAll(void)
{
    DWINFO* ptr = editWindows;
    while (ptr)
    {
        if (ptr->active && SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0))
            SendMessage(ptr->self, WM_COMMAND, IDM_SAVE, 0);
        ptr = ptr->next;
    }
}

//-------------------------------------------------------------------------

int AnyModified(void)
{
    DWINFO* ptr = editWindows;
    int rv = 0;
    while (ptr)
    {
        if (ptr->active)
            rv |= SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0);
        ptr = ptr->next;
    }
    return rv;
}

//-------------------------------------------------------------------------

void CloseAll(void)
{

    DWINFO* ptr;
    MSG msg;
    ptr = editWindows;
    while (ptr)
    {
        if (ptr->active)
            ShowWindow(ptr->self, SW_HIDE);
        ptr = ptr->next;
    }
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        ProcessMessage(&msg);
        if (msg.message == WM_QUIT)
            break;
    }
    ptr = editWindows;
    while (ptr)
    {
        HANDLE xx = ptr->self;
        BOOL active = ptr->active;
        ptr = ptr->next;
        if (active)
            PostMessage(xx, WM_CLOSE, 0, 0);
    }
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        ProcessMessage(&msg);
        if (msg.message == WM_QUIT)
            break;
    }
}

//-------------------------------------------------------------------------

void RedrawAllBreakpoints(void)
{
    DWINFO* ptr = editWindows;
    while (ptr)
    {
        if (ptr->active)
            InvalidateRect(ptr->self, 0, 0);
        ptr = ptr->next;
    }
}

//-------------------------------------------------------------------------

char* GetEditData(HWND hwnd)
{
    int l;
    char* buf;
    l = SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
    //	buf = HeapAlloc(editHeap, HEAP_ZERO_MEMORY, l + 1);
    buf = calloc(l + 1, 1);
    if (!buf)
    {
        return 0;
    }
    SendMessage(hwnd, WM_GETTEXT, l + 1, (LPARAM)buf);

    return buf;
}

void FreeEditData(char* buf)
{
    free(buf);
    //	HeapFree(editHeap, 0, buf);
    //	HeapCompact(editHeap, 0);
}
//-------------------------------------------------------------------------

int SetEditData(HWND hwnd, char* buf, BOOL savepos)
{
    SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), WM_SETTEXT, savepos, (LPARAM)buf);
    FreeEditData(buf);
    return TRUE;
}

//-------------------------------------------------------------------------

void backupFile(char* name)
{
    char newname[256], buffer[512];
    char* s;
    HANDLE in, out;
    BY_HANDLE_FILE_INFORMATION info;
    strcpy(newname, name);
    s = strrchr(newname, '.');
    strcat(newname, ".bak");

    in = CreateFile(name, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (in == INVALID_HANDLE_VALUE)
        return;
    if (!GetFileInformationByHandle(in, &info))
    {
        CloseHandle(in);
        return;
    }
    out = CreateFile(newname, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (out == INVALID_HANDLE_VALUE)
    {
        ExtendedMessageBox("File Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Backup file is not writeable");
        CloseHandle(in);
        return;
    }
    while (1)
    {
        DWORD read, written;
        if (!ReadFile(in, buffer, 512, &read, 0) || !read)
            break;
        WriteFile(out, buffer, read, &written, 0);
    }
    SetFileTime(out, &info.ftCreationTime, &info.ftLastAccessTime, &info.ftLastWriteTime);
    CloseHandle(out);
    CloseHandle(in);
}

//-------------------------------------------------------------------------
    static unsigned char BOM[] = { 0xef, 0xbb, 0xbf };
    static unsigned char BOM2[] = { 0xff, 0xfe }; // only LE version at this time...

int WriteFileData(char *name, char *buf, int flags)
{
    FILE *out;
    if (flags & FD_DOS)
        out = fopen(name, "w");
    else
        out = fopen(name, "wb");
    if (out == NULL)
        return FALSE;
    if (flags & FD_UTF8)
    {
         fwrite(BOM, 1, 3, out);
         fputs(buf, out);
    }
    else if (flags & FD_UCS2LE)
    {
         fwrite(BOM2, 1, 2, out);
         int len = strlen(buf);
         for (int i=0; i < len; i+=4096)
         {
             WCHAR wbuf[4096], *p = wbuf;
             for (int j=0; j < 4096 && j < len -i; j++)
                 *p++ = buf[i+j];
             fwrite(wbuf, 2, p - wbuf, out);
         }
    }
    else
    {
        fputs(buf, out);
    }
    fclose(out);
    return TRUE;
}

char* ReadFileData(char *name, int*flags)
{
    *flags = FD_DOS;

    FILE* in = fopen(name, "rb");
    if (!in)
        return NULL;
    fseek(in, 0L, SEEK_END);
    int size = ftell(in);
    char* buf = calloc(size + 1, 1);
    if (!buf)
    {
        fclose(in);
        return NULL;
    }
    fseek(in, 0L, SEEK_SET);
    size = fread(buf, 1, size, in);
    if (size < 0)
        size = 0;
    buf[size] = 0;
    fclose(in);
    char *p, *q;
    p = q = buf;
    if (*p)
        *flags = 0;
    while (*p)
    {
        if (*p != '\r')
        {
            *q++ = *p++;
        }
        else
        {
            *flags |= FD_DOS;
            p++;
        }
    }
    *q = 0;
    if (!memcmp(buf, BOM, 3))
    {
        *flags |= FD_UTF8;
        strcpy(buf, buf + 3);
    }
    else if (!memcmp(buf, BOM2, 2))
    {
        *flags |= FD_UCS2LE;
        WCHAR *p = (WCHAR *)(buf +2);
        char *d = buf;
        while (*p)
            *d++ = (char)*p++;
        *d = 0;
    }
    return buf;
}
int SaveFile(HWND hwnd, DWINFO* info)
{
    char* buf = GetEditData(GetDlgItem(hwnd, ID_EDITCHILD));

    if (PropGetBool(NULL, "BACKUP_FILES"))
        backupFile(info->dwName);
    if (!buf)
        return FALSE;
    if (!WriteFileData(info->dwName, buf, info->fileFlags))
    {
        ExtendedMessageBox("File Error", MB_SETFOREGROUND | MB_SYSTEMMODAL, "Output file is not writeable");
        FreeEditData(buf);
        return FALSE;
    }
    FreeEditData(buf);
    FileTime(&info->time, info->dwName);
    return TRUE;
}

//-------------------------------------------------------------------------

int LoadFile(HWND hwnd, DWINFO* info, BOOL savepos)
{
    char *buf = ReadFileData(info->dwName, &info->fileFlags);
    if (!buf)
    {
        ShowWindow(info->dwHandle, SW_SHOW);
        return FALSE;
    }
    SetEditData(hwnd, buf, savepos);
    SendMessage(info->dwHandle, EM_SETMODIFY, 0, 0);
    recolorize(info);
    if (FileAttributes(info->dwName) & FILE_ATTRIBUTE_READONLY)
        SendMessage(info->dwHandle, EM_SETREADONLY, 1, 0);
    FileTime(&info->time, info->dwName);
    return TRUE;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void SetTitle(HWND hwnd)
{
    DWINFO* info = (DWINFO*)GetWindowLong(hwnd, 0);
    char buf[MAX_PATH];
    EDITDATA* dt = (EDITDATA*)SendMessage(info->dwHandle, EM_GETEDITDATA, 0, 0);
    int mod = SendMessage(info->dwHandle, EM_GETMODIFY, 0, 0);
    strcpy(buf, info->dwName);
    if (buf[0] == 0)
        strcpy(buf, info->dwTitle);
    if (activeProject)
    {
        strcpy(buf, relpath2(buf, activeProject->realName));
    }
    else
    {
        char dir[MAX_PATH];
        dir[0] = 0;
        GetCurrentDirectory(MAX_PATH, dir);
        strcat(dir, "\\hithere");
        strcpy(buf, relpath2(buf, dir));
    }
    if (dt && dt->id)
        sprintf(buf + strlen(buf), " (%d)", dt->id + 1);
    if (mod)
        strcat(buf, " *");
    SetWindowText(hwnd, buf);
    SendMessage(hwnd, WM_NCPAINT, 1, 0);
    SendMessage(hwndSrcTab, TABM_SETMODIFY, mod, (LPARAM)hwnd);
}
void drawParams(DWINFO* info, HWND hwnd)
{
    char buf[512];
    HWND child = hwndeditPopup ? hwndeditPopup : info->dwHandle;
    int start, ins, col, sel;
    int readonly = SendMessage(child, EM_GETREADONLY, 0, 0);
    int mod = SendMessage(child, EM_GETMODIFY, 0, 0);
    int maxLines = SendMessage(child, EM_GETLINECOUNT, 0, 0) + 1;
    int textSize = SendMessage(child, EM_GETSIZE, 0, 0);
    CHARRANGE a;
    SendMessage(child, EM_GETSEL, (WPARAM)&sel, 0);
    SendMessage(child, EM_EXGETSEL, 0, (LPARAM)&a);
    sel = a.cpMin;
    start = SendMessage(child, EM_EXLINEFROMCHAR, 0, sel);
    ins = SendMessage(child, EM_GETINSERTSTATUS, 0, 0);
    col = SendMessage(child, EM_GETCOLUMN, 0, 0);
    sprintf(buf, "Size: %d", textSize);
    SendMessage(hwndStatus, SB_SETTEXT, 2 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Lines: %d", maxLines);
    SendMessage(hwndStatus, SB_SETTEXT, 3 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Line: %d", start + 1);
    SendMessage(hwndStatus, SB_SETTEXT, 4 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Col: %d", col + 1);
    SendMessage(hwndStatus, SB_SETTEXT, 5 | SBT_NOBORDERS, (LPARAM)buf);
    SendMessage(hwndStatus, SB_SETTEXT, 6 | SBT_NOBORDERS, (LPARAM)(ins ? "INS" : "OVR"));
    if (readonly)
        SendMessage(hwndStatus, SB_SETTEXT, 7 | SBT_NOBORDERS, (LPARAM)("READ-ONLY"));
    else
        SendMessage(hwndStatus, SB_SETTEXT, 7 | SBT_NOBORDERS, (LPARAM)(mod ? "MODIFIED" : "    "));

    SetTitle(hwnd);
    if (info->jumplistLineno != start)
    {
        info->jumplistLineno = start;
        SetJumplistPos(hwnd, start + 1);
    }
}

//-------------------------------------------------------------------------

void eraseParams(HWND hwnd)
{
    SendMessage(hwndStatus, SB_SETTEXT, 2 | SBT_NOBORDERS, (LPARAM) "    ");
    SendMessage(hwndStatus, SB_SETTEXT, 3 | SBT_NOBORDERS, (LPARAM) "    ");
    SendMessage(hwndStatus, SB_SETTEXT, 4 | SBT_NOBORDERS, (LPARAM) "    ");
    SendMessage(hwndStatus, SB_SETTEXT, 5 | SBT_NOBORDERS, (LPARAM) "    ");
    SendMessage(hwndStatus, SB_SETTEXT, 6 | SBT_NOBORDERS, (LPARAM) "    ");
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

int PaintBreakpoints(HWND hwnd, HDC dc, PAINTSTRUCT* paint, RECT* rcl)
{
    int count;
    HBRUSH graybrush, graybrush1;
    RECT r, r1;
    int i;
    DWINFO* ptr = (DWINFO*)GetWindowLong(hwnd, 0);
    int linenum = SendMessage(ptr->dwHandle, EM_GETFIRSTVISIBLELINE, 0, 0) + 1;
    int chpos1 = SendMessage(ptr->dwHandle, EM_LINEINDEX, linenum, 0);
    int maxLines = SendMessage(ptr->dwHandle, EM_GETLINECOUNT, 0, 0);
    int ypos;
    int lines, offset = 0;
    int height;
    int* lt;
    int lc;
    int oldbk = SetBkColor(dc, RetrieveSysColor(COLOR_BTNFACE));
    int oldfg = SetTextColor(dc, RetrieveSysColor(COLOR_BTNTEXT));
    HPEN linePen;
    HFONT xfont;
    POINTL pt;
    int bpline = BPLine(ptr->dwName);
    HDC drawDC;
    HBITMAP drawBitmap;
    SendMessage(ptr->dwHandle, EM_POSFROMCHAR, (WPARAM)&pt, chpos1);
    ypos = pt.y;

    SendMessage(ptr->dwHandle, EM_GETRECT, 0, (LPARAM)&r1);
    drawDC = CreateCompatibleDC(dc);
    drawBitmap = CreateCompatibleBitmap(dc, rcl->right, rcl->bottom);

    drawBitmap = SelectObject(drawDC, drawBitmap);
    lines = r1.bottom / (height = SendMessage(ptr->dwHandle, EM_GETTEXTHEIGHT, 0, 0));

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
                for (k = j; k < lc; k++)
                    if (lt[k] == oldline)
                    {
                        r.top = offset + (i - linenum) * height;
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
            case TAG_DISABLEDBP:
                obj = IML_DISABLEDBP;
                break;
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
            ImageList_Draw(tagImageList, obj, drawDC, 1, (i - linenum) * height + offset, ILD_NORMAL);
        }
        if (ptr->editorOffset != EDITOR_OFFSET && i <= maxLines + 1)
        {
            char buf[256];
            sprintf(buf, "%d", i);
            SetBkMode(drawDC, TRANSPARENT);
            TextOut(drawDC,
                    EDITOR_OFFSET - 4 +
                        (ptr->lineNumberDigits - GetLog10(i)) *
                            ((EDITDATA*)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0))->cd->txtFontWidth,
                    (i - linenum) * height + offset, buf, strlen(buf));
            SetBkMode(drawDC, OPAQUE);
        }
    }
    xfont = SelectObject(drawDC, xfont);
    SetTextColor(drawDC, oldfg);
    SetBkColor(drawDC, oldbk);

    BitBlt(dc, rcl->left, rcl->top, rcl->right - rcl->left, rcl->bottom - rcl->top, drawDC, rcl->left, rcl->top, SRCCOPY);

    drawBitmap = SelectObject(drawDC, drawBitmap);
    DeleteObject(drawBitmap);
    DeleteDC(drawDC);

    return 0;
}

//-------------------------------------------------------------------------

LRESULT CALLBACK gotoProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
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
                DisableControl(hwnd, IDOK, !GetWindowText((HWND)lParam, buf, 2));
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

void recolorize(DWINFO* ptr)
{
    int language = LANGUAGE_NONE;
    if (stristr(ptr->dwName, ".c") == ptr->dwName + strlen(ptr->dwName) - 2 ||
        stristr(ptr->dwName, ".h") == ptr->dwName + strlen(ptr->dwName) - 2)
        language = LANGUAGE_C;
    if (stristr(ptr->dwName, ".cpp") == ptr->dwName + strlen(ptr->dwName) - 4 ||
        stristr(ptr->dwName, ".cxx") == ptr->dwName + strlen(ptr->dwName) - 4 ||
        stristr(ptr->dwName, ".cc") == ptr->dwName + strlen(ptr->dwName) - 3)
        language = LANGUAGE_CPP;
    else if (stristr(ptr->dwName, ".asm") == ptr->dwName + strlen(ptr->dwName) - 4 ||
             stristr(ptr->dwName, ".asi") == ptr->dwName + strlen(ptr->dwName) - 4 ||
             stristr(ptr->dwName, ".inc") == ptr->dwName + strlen(ptr->dwName) - 4 ||
             stristr(ptr->dwName, ".nas") == ptr->dwName + strlen(ptr->dwName) - 4 ||
             stristr(ptr->dwName, ".s") == ptr->dwName + strlen(ptr->dwName) - 2)
        language = LANGUAGE_ASM;
    else if (stristr(ptr->dwName, ".rc") == ptr->dwName + strlen(ptr->dwName) - 3)
        language = LANGUAGE_RC;
    SendMessage(ptr->dwHandle, EM_LANGUAGE, 0, language);
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
                {
                    ProcessMessage(&msg);
                    if (msg.message == WM_QUIT)
                        return WAIT_TIMEOUT;
                }
            }
            break;
            default:
                return WAIT_TIMEOUT;
        }
    }
}
void RemoveAllParse()
{
    WaitForSingleObject(ccThreadGuard, INFINITE);
    while (codeCompList)
    {
        struct _ccList* list = codeCompList->next;
        free(codeCompList->name);
        free(codeCompList);
        codeCompList = list;
    }
    SetEvent(ccThreadGuard);
}
static void installparse(char* name, BOOL remove)
{
    char* p = (char*)calloc(1, strlen(name) + 1);
    if (p)
    {
        struct _ccList** v = &codeCompList;
        struct _ccList* list = (struct _ccList*)calloc(1, sizeof(struct _ccList));
        strcpy(p, name);
        list->remove = remove;
        list->name = p;
        WaitForSingleObject(ccThreadGuard, INFINITE);
        while (*v)
            v = &(*v)->next;
        *v = list;
        SetEvent(ccThreadGuard);
        PostThreadMessage(ccThreadId, WM_USER, remove, (LPARAM)p);
    }
}
void InstallForParse(HWND hwnd)
{
    if (PropGetInt(NULL, "CODE_COMPLETION") != 0)
    {
        DWINFO* info = (DWINFO*)GetWindowLong(hwnd, 0);
        char* name = info->dwName;
        int len = strlen(name);
        if (!xstricmp(name + len - 2, ".c") || !xstricmp(name + len - 4, ".cpp") || !xstricmp(name + len - 3, ".cc") ||
            !xstricmp(name + len - 4, ".cxx") || !xstricmp(name + len - 2, ".h"))
            installparse(name, FALSE);
        else if (!xstricmp(name + len - 2, ".h"))
        {
            DWINFO* ptr = editWindows;
            while (ptr)
            {
                if (ptr->active && IsWindow(ptr->self))
                {
                    name = ptr->dwName;
                    len = strlen(name);
                    if (!xstricmp(name + len - 2, ".c") || !xstricmp(name + len - 4, ".cpp") || !xstricmp(name + len - 3, ".cc") ||
                        xstricmp(name + len - 4, ".cxx"))
                        installparse(name, FALSE);
                }
                ptr = ptr->next;
            }
        }
    }
}
void InstallAllForParse(void)
{
    DWINFO* ptr = editWindows;
    while (ptr)
    {
        if (ptr->active)
            InstallForParse(ptr->self);
        ptr = ptr->next;
    }
}
static void deleteParseData(char* name)
{
    int len = strlen(name);
    if (name[len - 2] == '.')
    {
        if (tolower(name[len - 1]) == 'c')
        {
            installparse(name, TRUE);
        }
    }
}
unsigned __stdcall ScanParse(void* aa)
{
    while (1)
    {
        if (codeCompList)
        {
            struct _ccList* list = NULL;
            WaitForSingleObject(ccThreadGuard, INFINITE);
            if (codeCompList)
            {
                list = codeCompList;
                codeCompList = codeCompList->next;
            }
            SetEvent(ccThreadGuard);
            if (list)
            {
                if (list->remove)
                    deleteFileData(list->name);
                else
                    DoParse(list->name);
                free(list->name);
                free(list);
            }
        }
        if (stopCCThread)
            break;
        Sleep(100);
        if (stopCCThread)
            break;
    }
    SetEvent(ccThreadExit);
    return 0;
}
//-------------------------------------------------------------------------

LRESULT CALLBACK DrawProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    LRESULT rv;
    DWINFO *ptr, *ptr1;
    EDITDATA* ed;
    OPENFILENAME ofn;
    RECT r;
    PAINTSTRUCT paint;
    LPCREATESTRUCT createStruct;
    int startpos, endpos, flag, i;
    NMHDR* nm;
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
                        linenum = SendMessage(ptr->dwHandle, EM_EXLINEFROMCHAR, 0, linenum) + 1;
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
                CHARRANGE range;
                HMENU menu = LoadMenuGeneric(hInstance, "EDITMENU");
                HMENU popup = GetSubMenu(menu, 0);
                POINT pos, pos1;
                POINTL epos;
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
                MapWindowPoints(HWND_DESKTOP, ptr->dwHandle, &pos1, 1);
                epos.x = pos1.x;
                epos.y = pos1.y;
                range.cpMin = range.cpMax = SendMessage(ptr->dwHandle, EM_CHARFROMPOS, 0, (LPARAM)&epos);
                SendMessage(ptr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&range);
                TrackPopupMenuEx(popup, TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pos.x, pos.y, hwndFrame, NULL);
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
                    newInfo = ptr;
                    openfile(ptr, TRUE, TRUE);
                    break;
                case IDM_RTLHELP:
                case IDM_LANGUAGEHELP:
                case IDM_SPECIFIEDHELP:
                    ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                    i = SendMessage(ptr->dwHandle, WM_COMMAND, wParam, lParam);
                    break;
                case IDM_GOTO:
                    lParam = DialogBox(hInstance, "GOTODIALOG", hwnd, (DLGPROC)gotoProc);
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
                    SendMessage(ptr->dwHandle, EM_EXSETSEL, 0, (LPARAM)&s);
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
                    return 0;
                case ID_QUERYSAVE:
                    ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                    rv = SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0);
                    if (rv)
                    {
                        return ExtendedMessageBox("File Changed", MB_YESNOCANCEL, "File %s has changed.  Do you wish to save it?",
                                                  ptr->dwTitle);
                    }
                    else
                        return IDNO;
                case IDM_SAVEAS:
                dialog:
                    ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                    if (!SaveFileDialog(&ofn, ptr->dwName, hwnd, TRUE, szSourceFilter, 0))
                        break;
                    if (activeProject && ptr->dwName[0] == 0)
                    {
                        if (ExtendedMessageBox("Project Query", MB_TASKMODAL | MB_YESNO, "Add file to active project?") == IDYES)
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
                    sprintf(buf, "Saving %s...", ptr->dwTitle);
                    SetStatusMessage(buf, FALSE);
                    if (LOWORD(wParam) == IDM_SAVE)
                    {
                        rv = SendMessage(ptr->dwHandle, EM_GETMODIFY, 0, 0);
                        if (!rv)
                            break;
                    }
                    if (ptr->dwName[0] == 0)
                        goto dialog;
                    rv = SaveFile(hwnd, (DWINFO*)GetWindowLong(hwnd, 0));
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
                    SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_SETSEL, 0, -1);
                    break;
                case IDM_FIND:
                    OpenFindDialog();
                    break;
                case IDM_REPLACE:
                    OpenReplaceDialog();
                    break;
                case IDM_FINDNEXT:
                {
                    _beginthread((BEGINTHREAD_FUNC)(finding ? DoFindNext : DoReplaceNext), 0, (LPVOID)hwndFindInternal);
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
                    SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_SELECTINDENT, 0, 1);
                    break;
                case IDM_UNINDENT:
                    SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_SELECTINDENT, 0, 0);
                    break;
                case EN_SETFOCUS:
                    break;
                case EN_NEEDFOCUS:
                    if ((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) == hwnd)
                        SendMessage(hwnd, WM_SETFOCUS, 0, 0);
                    else
                        SendMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)hwnd, 0);
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
            return SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_CANREDO, 0, 0);
        case EM_CANUNDO:
            return SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_CANUNDO, 0, 0);
        case EN_SETCURSOR:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            drawParams(ptr, hwnd);
            GetClientRect(hwnd, &r);
            r.right = EDITOR_OFFSET - 3;
            InvalidateRect(hwnd, &r, 0);
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_NCACTIVATE:
            PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
            return TRUE;
        case WM_NCPAINT:
            return PaintMDITitleBar(hwnd, iMessage, wParam, lParam);
        case WM_PAINT:
        {
            RECT rect;
            HDC hDC, hdouble;
            HBITMAP bitmap;
            PAINTSTRUCT ps;
            GetClientRect(hwnd, &rect);
            hDC = BeginPaint(hwnd, &ps);
            hdouble = CreateCompatibleDC(hDC);
            bitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
            SelectObject(hdouble, bitmap);
            PaintBreakpoints(hwnd, hdouble, &paint, &rect);
            BitBlt(hDC, 0, 0, rect.right, rect.bottom, hdouble, 0, 0, SRCCOPY);
            DeleteObject(bitmap);
            DeleteObject(hdouble);
            EndPaint(hwnd, &ps);
        }
            return 0;
        case WM_CREATE:
            //         maximized = TRUE ;
            rv = DefMDIChildProc(hwnd, iMessage, wParam, lParam);
            if (rv)
                return rv;
            //            SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
            createStruct = (LPCREATESTRUCT)lParam;
            ed = (EDITDATA*)((LPMDICREATESTRUCT)(createStruct->lpCreateParams))->lParam;
            ptr = DeQueueEditWindow();
            SetWindowLong(hwnd, 0, (int)ptr);
            SetWindowLong(hwnd, 4, (int)EDITSIG);
            if (ed != (EDITDATA*)-1)
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
            ptr->dwHandle = CreateWindowEx(0, "xedit", 0,
                                           WS_CHILD + WS_CLIPSIBLINGS + WS_CLIPCHILDREN + WS_HSCROLL + WS_VSCROLL + ES_LEFT +
                                               WS_VISIBLE + ES_MULTILINE + ES_NOHIDESEL + ES_AUTOVSCROLL + ES_AUTOHSCROLL,
                                           EDITOR_OFFSET, 0, 0, 0, hwnd, (HMENU)ID_EDITCHILD, hInstance,
                                           (ed && ed != (EDITDATA*)-1) ? (void*)ed : NULL);

            ptr->self = hwnd;
            ptr->dwLineNo = -1;
            if (newInfo && !newInfo->newFile && newInfo->dwLineNo != -1)
            {
                ptr->dwLineNo = newInfo->dwLineNo;
                newInfo->dwLineNo = -1;
            }
            if (!ed && newInfo->logMRU)
            {
                InsertMRU(ptr, 0);
                MRUToMenu(0);
            }
            if (!ed)
            {
                if (ptr->dwName[0] && (!newInfo || !newInfo->newFile))
                    LoadFile(ptr->self, ptr, FALSE);
                else
                    ptr->fileFlags = FD_DOS;
                if (ptr->dwLineNo != -1)
                {
                    PostMessage(ptr->self, WM_COMMAND, IDM_SETLINE, ptr->dwLineNo);
                }
                recolorize(ptr);
            }
            else
            {
                ptr->fileFlags = FD_DOS;
            }
            MsgWait(ewSem, INFINITE);
            if (editWindows)
                editWindows->prev = ptr;
            ptr->next = editWindows;
            ptr->prev = NULL;
            ptr->active = TRUE;
            editWindows = ptr;
            SetEvent(ewSem);
            SendMessage(hwnd, WM_SETLINENUMBERMODE, 2, 0);
            ptr->editData = (EDITDATA*)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0);
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
            EnQueueEditWindow(ptr);
            PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
            break;
        case WM_SIZE:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            MoveWindow(GetDlgItem(hwnd, ID_EDITCHILD), ptr->editorOffset, 0, (lParam & 65535) - ptr->editorOffset, lParam >> 16, 0);
            InvalidateRect(hwnd, 0, 0);  // for line numbers
            break;
        // timer being used to prevent a click in the margin which activates
        // the window from setting a breakpoint...
        // it also is used to re-establish the focus as some instances with
        // using an OPENFILEDIALOG create weird timing conditions that don't
        // change the focus...
        case WM_TIMER:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            KillTimer(hwnd, ptr->timerId);
            ptr->timing = FALSE;
            if ((HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0) == hwnd)
                PostMessage(hwnd, WM_COMMAND, EN_NEEDFOCUS, 0);
            break;
        case WM_MDIACTIVATE:
            if ((HWND)lParam != hwnd)
            {
                break;
            }
            SendMessage((HWND)lParam, WM_NCPAINT, 1, 0);
            SendMessage((HWND)wParam, WM_NCPAINT, 1, 0);
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
            ptr->timerId = SetTimer(hwnd, 1000, 100, NULL);
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
                ptr->lineNumberDigits = wParam;  // never goes down while the window is open...
            }
            ptr->editorOffset =
                EDITOR_OFFSET +
                lineNumbers *
                    (ptr->lineNumberDigits * ((EDITDATA*)SendMessage(ptr->dwHandle, EM_GETEDITDATA, 0, 0))->cd->txtFontWidth + 4);
            GetClientRect(hwnd, &r);
            MoveWindow(GetDlgItem(hwnd, ID_EDITCHILD), ptr->editorOffset, r.top, r.right - ptr->editorOffset, r.bottom - r.top, 0);
            InvalidateRect(hwnd, 0, 0);
            break;
        }
        case WM_INITMENUPOPUP:
            SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_GETSEL, (WPARAM)&startpos, (LPARAM)&endpos);
            flag = startpos < endpos;
            EnableMenuItem(hMenuMain, IDM_CUT, flag);
            EnableMenuItem(hMenuMain, IDM_COPY, flag);
            EnableMenuItem(hMenuMain, IDM_PASTE, 1);
            EnableMenuItem(hMenuMain, IDM_UNDO, SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), EM_CANUNDO, 0, 0));
            EnableMenuItem(hMenuMain, IDM_BROWSETODEFINITION, flag);
            EnableMenuItem(hMenuMain, IDM_BROWSETODECLARATION, flag);
            // EnableMenuItem(hMenuMain,IDM_BROWSEBACK,flag) ;
            EnableMenuItem(hMenuMain, IDM_BOOKMARK, flag);
            // EnableMenuItem(hMenuMain,IDM_NEXTBOOKMARK,flag) ;
            // EnableMenuItem(hMenuMain,IDM_PREVBOOKMARK,flag) ;
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
            return (LRESULT)ptr->dwTitle;
        case WM_FILENAME:
            ptr = (DWINFO*)GetWindowLong(hwnd, 0);
            return (LRESULT)ptr->dwName;
        default:
            if (iMessage >= WM_USER)
                return SendMessage(GetDlgItem(hwnd, ID_EDITCHILD), iMessage, wParam, lParam);
            break;
    }
    return DefMDIChildProc(hwnd, iMessage, wParam, lParam);
}

//-------------------------------------------------------------------------

void RegisterDrawWindow(HINSTANCE hInstance)
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

    ccThreadExit = CreateEvent(0, 0, 0, 0);
    ccThreadGuard = CreateEvent(0, 0, TRUE, 0);
    ewSem = CreateEvent(NULL, FALSE, TRUE, NULL);
    _beginthreadex(NULL, 0, ScanParse, NULL, 0, &ccThreadId);
}

//-------------------------------------------------------------------------

HWND openfile(DWINFO* newInfo, int newwindow, int visible)
{
    RECT r;
    HWND rv;
    void* extra = newInfo == (DWINFO*)-1 ? newInfo : NULL;
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        ProcessMessage(&msg);
        if (msg.message == WM_QUIT)
            break;
    }
    if (msg.message == WM_QUIT)
        return NULL;
    if (newInfo && newInfo != (DWINFO*)-1)
    {
        DWINFO* ptr = editWindows;
        while (ptr)
        {
            if (ptr->active && SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)newInfo))
            {
                if (newwindow)
                {
                    extra = (EDITDATA*)SendMessage(ptr->self, EM_GETEDITDATA, 0, 0);
                }
                else
                {
                    HWND active = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
                    if (ptr->self == active)
                    {
                        SetFocus(GetDlgItem(active, ID_EDITCHILD));
                    }
                    else
                    {
                        PostMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)ptr->self, 0);
                    }
                    if (newInfo->dwLineNo != -1)
                        PostMessage(ptr->self, WM_COMMAND, IDM_SETLINE, newInfo->dwLineNo);
                    return ptr->self;
                }
            }
            ptr = ptr->next;
        }
    }
    GetClientRect(hwndClient, &r);
    if (r.right > 820 * 5 / 4)
        r.right = 820;
    else
        r.right = r.right * 5 / 6;
    rv = CreateMDIWindow(szDrawClassName, szUntitled,
                         (visible ? WS_VISIBLE : 0) | WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | MDIS_ALLCHILDSTYLES |
                             WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SIZEBOX |
                             (PropGetInt(NULL, "TABBED_WINDOWS") ? WS_MAXIMIZE : WS_SYSMENU),
                         CW_USEDEFAULT, CW_USEDEFAULT, r.right, CW_USEDEFAULT, hwndClient, hInstance, (LPARAM)extra);
    return rv;
}
//-------------------------------------------------------------------------

HWND CreateDrawWindow(DWINFO* baseinfo, int visible)
{
    static DWINFO temp;
    OPENFILENAME ofn;

    newInfo = baseinfo;
    if (!newInfo)
    {
        newInfo = &temp;
        newInfo->dwLineNo = -1;
        newInfo->logMRU = TRUE;
        newInfo->newFile = FALSE;
        if (OpenFileDialog(&ofn, 0, 0, FALSE, TRUE, szSourceFilter, 0))
        {
            char *q = ofn.lpstrFile, path[256];
            strcpy(path, ofn.lpstrFile);
            q += strlen(q) + 1;
            if (!*q)
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

            //               ExtendedMessageBox("File Open",MB_SETFOREGROUND | MB_SYSTEMMODAL,"Could not open file %s
            //               %d",newInfo->dwName,GetLastError()) ;
            return 0;
        }
    }
    else if (newInfo == (DWINFO*)-1)
    {
        newInfo = &temp;
        newInfo->dwLineNo = -1;
        newInfo->logMRU = TRUE;
        newInfo->newFile = TRUE;
        newInfo->fileFlags = FD_DOS;
        if (SaveFileDialog(&ofn, 0, 0, TRUE, szNewFileFilter, "Open New File"))
        {
            char *q = ofn.lpstrFile, path[256];
            strcpy(path, ofn.lpstrFile);
            q += strlen(q) + 1;
            if (!*q)
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
