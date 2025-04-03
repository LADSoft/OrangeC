/* Software License Agreement
 * 
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 * 
 */

#define Uses_TView
#define Uses_TGroup
#define Uses_TFileDialog
#define Uses_MsgBox
#define Uses_TEvent
#define Uses_TDialog
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TButton
#define Uses_TMenuItem;
#include "tv.h"
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include "infopad.h"
#include "editview.h"
#include "ipconfig.h"
#include <ctype.h>


#define LINENO_DIGITS 7

extern InfoPadCfg cfg;
extern TMenuItem *FileMRUMenu;
extern char szSourceFilter[];
extern char szNewFileFilter[];
extern enum DebugState uState;
extern char highlightText[256] ;
extern int highlightCaseSensitive;
extern int highlightWholeWord;

extern _tabStruct *srcTab;
extern EnhancedStatusLine *statusLine;
extern DWINFO *newInfo;

POINT rightclickPos;
char szUntitled[] = "New File";


DWINFO *editWindows;
MRU WindowMRU;

void recolorize(DWINFO *ptr);

void SetTitle(DWINFO *ptr);
void LoadColors();

void EditorRundown(void)
{
}
int FileTime(FILETIME *timex, char *name)
{
    HANDLE fd;

    memset(timex, 0, sizeof(*timex));
    fd = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (fd == INVALID_HANDLE_VALUE)
        return 0;
    if (!GetFileTime(fd, 0, 0, timex))
    {
        CloseHandle(fd);
        return 0;
    }
    CloseHandle(fd);
    return 1;
}
int FileAttributes(char *name)
{
    int rv = GetFileAttributes(name);
    if (rv ==  - 1)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            return 0;
        return  - 1; // any other error, it is read only file
    }
    else
        return rv;
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
        SetTitle(ptr);
        ptr = ptr->fwd;
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
        ptr = ptr->fwd;
    }
}
//-------------------------------------------------------------------------
void ApplyEditorSettings(void)
{
    DWINFO *ptr = editWindows;
    int tabs = PropGetInt(NULL, "TAB_INDENT") * 4;
    LoadColors();
    while (ptr)
    {
        ptr->child->WinMessage(EM_SETTABSTOPS, 0, (LPARAM)&tabs);
        ptr->child->WinMessage(WM_SETEDITORSETTINGS,
            0, 0);
        ptr->WinMessage(WM_SETLINENUMBERMODE, 0, 0);
        ptr = ptr->fwd;
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
        if (ptr->WinMessage(WM_COMMAND, ID_QUERYHASFILE, (LPARAM)
            &info))
        {
            ptr->drawView();
        }
        ptr = ptr->fwd;
    }
}
DWINFO *GetFileInfo(char *name)
{
    DWINFO *ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, name);
    while (ptr)
    {
        if (ptr->WinMessage(WM_COMMAND, ID_QUERYHASFILE, (LPARAM)
            &info))
            return ptr;
        ptr = ptr->fwd;
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
        if (ptr->WinMessage(WM_COMMAND, ID_QUERYHASFILE, (LPARAM)
            &info))
        {
            char *p;
            srcTab->WinMessage(TABM_RENAME, (WPARAM)newName, (LPARAM)ptr);
            strcpy(ptr->dwName, newName);
            p=strrchr(newName, '\\');
            if (p)
                p++;
            else 
                p = newName;
            strcpy(ptr->dwTitle, p);
            SetTitle(ptr);
        }
        ptr = ptr->fwd;
    }
}


//-------------------------------------------------------------------------

void SaveDrawAll(void)
{
    DWINFO *ptr = editWindows;
    while (ptr)
    {
        if (ptr->child->WinMessage(EM_GETMODIFY, 0, 0))
            ptr->WinMessage(WM_COMMAND, IDM_SAVE, 0);
        ptr = ptr->fwd;
    }
}

//-------------------------------------------------------------------------

int AnyModified(void)
{
    DWINFO *ptr = editWindows;
    int rv = 0;
    while (ptr)
    {
        rv |= ptr->child->WinMessage(EM_GETMODIFY, 0, 0);
        ptr = ptr->fwd;
    }
    return rv;
}

//-------------------------------------------------------------------------

void CloseAll(void)
{
    
    DWINFO *ptr;
    ptr = editWindows;
    while (ptr)
    {
        ptr->setState(sfVisible, False);
        ptr = ptr->fwd;
    }
    ptr = editWindows;
    while (ptr)
    {
        DWINFO *xx = ptr;
        ptr = ptr->fwd;
        xx->owner->remove(xx);
        delete xx;
    }
}


//-------------------------------------------------------------------------

char *GetEditData(EDITDATA *ptr)
{
    int l;
    char *buf;
    l = ptr->WinMessage(WM_GETTEXTLENGTH, 0, 0);
    buf = (char *)calloc(l+1, 1);
    if (!buf)
    {
        return 0;
    }
    ptr->WinMessage(WM_GETTEXT, l+1, (DWORD)buf);
    
    return buf;
}

void FreeEditData(char *buf)
{
    free(buf);
}
//-------------------------------------------------------------------------

int SetEditData(DWINFO *ptr, char *buf, BOOL savepos)
{
    ptr->child->WinMessage(WM_SETTEXT, savepos, (DWORD)buf);
    FreeEditData(buf);
    return TRUE;
}

//-------------------------------------------------------------------------

static void backup(char *name)
{
    char newname[256], buffer[8192];
    char *s;
    HANDLE in, out;
    int size;
    BY_HANDLE_FILE_INFORMATION info;
    strcpy(newname, name);
    s = strrchr(newname, '.');
    if (s)
        *s = 0;
    
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
        messageBox("Backup file is not writeable", mfError);
        CloseHandle(in);
        return ;
    }
    while (1)
    {
        DWORD read, written;
        if (!ReadFile(in, buffer, 8192, &read, 0) || !read)
            break;
        WriteFile(out, buffer, read, &written, 0);
    }
    SetFileTime(out, &info.ftCreationTime, &info.ftLastAccessTime,
        &info.ftLastWriteTime);
    CloseHandle(out);
    CloseHandle(in);
}

//-------------------------------------------------------------------------

int SaveFile(DWINFO *info)
{
    char *buf = GetEditData(info->child);
    FILE *out;
    int l, i;

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
        messageBox("Output file is not writeable", mfError);
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

int LoadFile(DWINFO *info, BOOL savepos)
{
    long size;
    char *buf,*p,*q;
    FILE *in = fopen(info->dwName, "rb");
    info->dosStyle = FALSE;
    if (!in)
    {
        info->dosStyle = TRUE;
        info->child->setState(sfVisible, True);
        return FALSE;
    }
    fseek(in, 0L, SEEK_END);
    size = ftell(in);
    buf = (char *)calloc(size+1, 1);
    if (!buf)
    {
        info->dosStyle = TRUE;
        fclose(in);
        info->child->setState(sfVisible, True);
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
    SetEditData(info, buf, savepos);
    info->child->WinMessage(EM_SETMODIFY, 0, 0);
    recolorize(info);
    if (FileAttributes(info->dwName) &FILE_ATTRIBUTE_READONLY)
        info->child->WinMessage(EM_SETREADONLY, 1, 0);
    FileTime(&info->time, info->dwName);
    return TRUE;

}

//-------------------------------------------------------------------------

void abspath(char *name, char *path)
{
    char projname[256],  *p,  *nname = name;
    if (!path)
    {
        path = projname;
        GetCurrentDirectory(256, projname);
        strcat(projname,"\\hi");
    }
    if (!path[0])
        return ;
    if (name[0] == 0)
        return ;
    if (name[1] == ':')
        return ;
    strcpy(projname, path);
    p = strrchr(projname, '\\');
    if (!p)
        return ;
    p--;
    if (!strstr(name, "..\\"))
    {
        if (name[0] != '\\')
        {
            strcpy(p + 2, name);
            strcpy(nname, projname);
        }
        return ;
    }
    while (!strncmp(name, "..\\", 3))
    {
        while (p > projname &&  *p-- != '\\')
            ;
        name += 3;
    }
    *++p = '\\';
    p++;
    strcpy(p, name);
    strcpy(nname, projname);
}

//-------------------------------------------------------------------------

char *relpath(char *name, char *path)
{
    static char projname[MAX_PATH], localname[MAX_PATH];
    char *p = localname,  *q = projname,  *r,  *s;
    if (!path[0])
        return name;
    if (toupper(name[0]) != toupper(path[0]))
        return name;

    strcpy(localname, name);
    strcpy(projname, path);
    r = strrchr(localname, '\\');
    if (r)
        *r++ = 0;
    // r has the point to the file name
    else
        r = localname;
    s = strrchr(projname, '\\');
    if (!s)
        return name;
    if (*s)
        *s = 0;

    while (*p &&  *q && toupper(*p) == toupper(*q))
    {
        p++, q++;
    }
    if (!(*p |  *q))
        return r;
    else if (*(p - 1) == '\\' && *(p - 2) == ':')
        return name;
    else
    {
        int count =  *q != 0;
        if (*q != '\\')
            while (p > localname &&  *p != '\\')
                p--;
        while (*q && (q = strchr(q + 1, '\\')))
            count++;
        projname[0] = 0;
        while (count--)
            strcat(projname, "..\\");
        if (*p)
        {
            strcat(projname, p + 1);
            strcat(projname, "\\");
        }
        strcat(projname, r);
        return projname;
    }
}

//-------------------------------------------------------------------------
void SetTitle(DWINFO *info)
{
    char buf[MAX_PATH];
    EDITDATA *dt = info->child;
    int mod = info->child->WinMessage(EM_GETMODIFY, 0, 0);
    strcpy(buf, info->dwName);
    if (buf[0] == 0)
        strcpy(buf, info->dwTitle);
    char dir[MAX_PATH];
    dir[0] = 0;
    GetCurrentDirectory(MAX_PATH, dir);
    strcat(dir,"\\hithere");
    strcpy(buf, relpath(buf, dir));
    if (dt->id)
        sprintf(buf + strlen(buf), " (%d)", dt->id + 1);
    if (mod)
        strcat(buf, " *");
    info->WinMessage(WM_SETTEXT, 0, (LPARAM)buf);
    srcTab->WinMessage(TABM_SETMODIFY, mod, (LPARAM)info);
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
void drawParams(DWINFO *info)
{
    char buf[512];
    int start, ins, col, sel;
    int readonly = info->child->WinMessage(EM_GETREADONLY, 0, 0);
    int mod = info->child->WinMessage(EM_GETMODIFY, 0, 0);
    int maxLines = info->child->WinMessage(EM_GETLINECOUNT, 0, 0) + 1; 
    int textSize = info->child->WinMessage(EM_GETSIZE, 0, 0);
    EDITDATA *dt ;
    CHARRANGE a;
    info->child->WinMessage(EM_EXGETSEL, 0, (LPARAM) &a);
    sel = a.cpMin;
    start = info->child->WinMessage(EM_EXLINEFROMCHAR, 0, sel);
    ins = info->child->WinMessage(EM_GETINSERTSTATUS, 0, 0);
    col = info->child->WinMessage(EM_GETCOLUMN, 0, 0);
    sprintf(buf, "Size: %d", textSize);
    statusLine->WinMessage(SB_SETTEXT, 1 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Lines: %d", maxLines);
    statusLine->WinMessage(SB_SETTEXT, 2 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Line: %d", start + 1);
    statusLine->WinMessage(SB_SETTEXT, 3 | SBT_NOBORDERS, (LPARAM)buf);
    sprintf(buf, "Col: %d", col + 1);
    statusLine->WinMessage(SB_SETTEXT, 4 | SBT_NOBORDERS, (LPARAM)buf);
    statusLine->WinMessage(SB_SETTEXT, 5 | SBT_NOBORDERS, (LPARAM)(ins ? "INS" : "OVR"));
    if (readonly)
        statusLine->WinMessage(SB_SETTEXT, 6 | SBT_NOBORDERS, (LPARAM)("RDONLY"));
    else
        statusLine->WinMessage(SB_SETTEXT, 6 | SBT_NOBORDERS, (LPARAM)(mod ? "MOD" : 
            "    "));

    SetTitle(info);
    int firstlinenum = info->child->WinMessage(EM_GETFIRSTVISIBLELINE, 0, 0) + 1;
    TRect ext = info->getExtent();
    int windowLines = ext.b.y - ext.a.y;
    int count = GetLog10(firstlinenum + windowLines);
    if (count > info->lineWnd->lineNumberDigits)
    {
        info->WinMessage(WM_SETLINENUMBERMODE, count, 0);
    }
    info->lineWnd->startLine = firstlinenum;
    info->lineWnd->drawView();
    
}

//-------------------------------------------------------------------------

void eraseParams()
{
    statusLine->WinMessage(SB_SETTEXT, 1 | SBT_NOBORDERS, (LPARAM)"    ");
    statusLine->WinMessage(SB_SETTEXT, 2 | SBT_NOBORDERS, (LPARAM)"    ");
    statusLine->WinMessage(SB_SETTEXT, 3 | SBT_NOBORDERS, (LPARAM)"    ");
    statusLine->WinMessage(SB_SETTEXT, 4 | SBT_NOBORDERS, (LPARAM)"    ");

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
        - 4 || stristr(ptr->dwName, ".s") == ptr->dwName + strlen(ptr->dwName)
        - 2)
        language = LANGUAGE_ASM;
    else if (stristr(ptr->dwName, ".rc") == ptr->dwName + strlen(ptr->dwName) - 3)
        language = LANGUAGE_RC;
    ptr->child->WinMessage(EM_LANGUAGE, 0, language);
}
void DWINFO::handleEvent(TEvent &event)
{
    TWindow::handleEvent(event);
    if (event.what == evBroadcast)
    {
        if (event.message.command == cmReceivedFocus)
        {
            if (event.message.infoPtr == this)
            {
                srcTab->WinMessage(TABM_SELECT, 0, (LPARAM)this);
                application->current = this;
                drawParams(this);
            }
        }
    }
}
//-------------------------------------------------------------------------
void DWINFO::changeBounds( const TRect& bounds )
{
    WinMessage(WM_SIZE, 0 , (DWORD)&bounds);
    TWindow::changeBounds(bounds);
}

static ushort gotoBoxRect( const TRect &bounds,
                     const char *Title,
                     const char *aLabel,
                     char *s,
                     uchar limit )
{
    TDialog *dialog;
    TView* control;
    TRect r;
    ushort c;

    dialog = new TDialog(bounds, Title);

    r = TRect( 4 + strlen(aLabel), 2, dialog->size.x - 3, 3 );
    control = new TInputLine( r, limit );
    dialog->insert( control );

    r = TRect(2, 2, 3 + strlen(aLabel), 3);
    dialog->insert( new TLabel( r, aLabel, control ) );

    r = TRect( dialog->size.x - 24, dialog->size.y - 4,
               dialog->size.x - 14, dialog->size.y - 2);
    dialog->insert( new TButton(r, MsgBoxText::okText, cmOK, bfDefault));

    r.a.x += 12;
    r.b.x += 12;
    dialog->insert( new TButton(r, MsgBoxText::cancelText, cmCancel, bfNormal));

    r.a.x += 12;
    r.b.x += 12;
    dialog->selectNext(False);
    dialog->setData(s);
    c = TProgram::application->execView(dialog);
    if( c != cmCancel )
        dialog->getData(s);
    TObject::destroy( dialog );
    return c;
}
static ushort gotoBox( const char *Title, const char *aLabel, char *s, uchar limit )
{
    TRect r(0, 0, 35, 8);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2,
           (TProgram::deskTop->size.y - r.b.y) / 2);
    return gotoBoxRect(r, Title, aLabel, s, limit);
}
DWORD DWINFO::WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam)
{
    DWORD threadhand;
    LRESULT rv;
    DWINFO *ptr1;
    EDITDATA *ed;
    TRect r;
    int childheight;
    int startpos, endpos, flag, i;
    FILETIME time;
    CHARRANGE s;
    char buf[256];
    DWINFO *ptr = this;
    TRect bounds;
    switch (iMessage)
    {
    
        /*
        case WM_NOTIFY:
            nm = (NMHDR*)lParam;
            if (nm->code == NM_RCLICK)
            {
                HMENU menu = LoadMenuGeneric(hInstance, "EDITMENU");
                HMENU popup = GetSubMenu(menu, 0);
                POINT pos, pos1;
                RECT rect;
                ptr = (DWINFO*)GetWindowLong(hwnd, 0);
                ptr->WinMessage(EN_SETCURSOR, 0, 0);
                if (!ptr->child->WinMessage(EM_GETMODIFY, 0, 0))
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
            */
        case EN_LINECHANGE:
            FindSetDirty();
            /*
            r = ptr->getBounds();
            r.b.x = ptr->lineWnd->lineNumberDigits + 1;
            ptr->setBounds(r);
            */
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDM_CLOSEFIND:
                ptr->child->WinMessage(WM_COMMAND, wParam, lParam);
                break;
            case IDM_COPYWINDOW:
                newInfo = ptr ;
                application->openfile(ptr, TRUE, TRUE);
                break ;
                /*
            case IDM_SPECIFIEDHELP:
            case IDM_RTLHELP:
            case IDM_LANGUAGEHELP:
                i = ptr->child->WinMessage(WM_COMMAND, wParam, lParam);
                break;
                */
            case IDM_GOTO:
                buf[0] = 0;
                if (gotoBox("Goto Line", "Enter Line Number", buf, 8) != cmOK)
                {
                    break;
                }
                lParam = atoi(buf);
                if (!lParam)
                {
                    break;
                }
                // fall through
            case IDM_SETLINE:
                ptr->child->WinMessage(EM_GETLINECOUNT, 0, 0); 
                    // force update of vertical scroll range
                i = ptr->child->WinMessage(EM_LINEINDEX, lParam - 1, 0);
                s.cpMin = i;
                s.cpMax = i;
                ptr->child->WinMessage(EM_HIDESELECTION, 1, 0);
                ptr->child->WinMessage(EM_EXSETSEL, 0, (LPARAM) &s);
                ptr->child->WinMessage(EM_SCROLLCARET, 0, 1);
                ptr->child->WinMessage(EM_HIDESELECTION, 0, 0);
                drawParams(ptr);
                ptr->drawView();
                break;
            case ID_REDRAWSTATUS:
                drawParams(ptr);
                ptr->drawView();
                break;
            case ID_QUERYHASFILE:
                ptr1 = (DWINFO*)lParam;
                if (!xstricmpz(ptr->dwName, ptr1->dwName))
                {
                    return ptr->child->WinMessage(EM_GETEDITDATA, 0, 0);
                }
                return 0 ;
            case ID_QUERYSAVE:
                rv = ptr->child->WinMessage(EM_GETMODIFY, 0, 0);
                if (rv)
                {
                    char buf[256];
                    sprintf(buf, "File %s has changed.  Do you wish to save it?", ptr
                        ->dwTitle);
                    if (messageBox(buf, mfConfirmation | mfYesButton | mfNoButton) != cmYes)
                        return IDNO;
                    return IDYES;
                }
                else
                    return IDNO;
            case IDM_SAVEAS:
                dialog: 
                    fileDlg = new TFileDialog( "*.*", "Save As", ptr->dwName, fdOKButton, 0);
                    buf[0] = 0;
                    if (application->executeDialog(fileDlg,buf) != cmFileOpen)
                        return 0;
//            case IDM_SAVEAS2:
            {
                EditRenameFile(ptr->dwName, buf);
                char *p = strrchr(buf, '\\');
                if (!p)
                    p = (char *)buf;
                else
                    p++;
                strcpy(ptr->dwTitle, p);
                strcpy(ptr->dwName, (char *)buf);
                recolorize(ptr);
                ptr->child->WinMessage(EM_SETMODIFY, 1, 0);
            }
                // fall through
            case IDM_SAVE:
                if (LOWORD(wParam) == IDM_SAVE)
                {
                    rv = ptr->child->WinMessage(EM_GETMODIFY, 0, 0);
                    if (!rv)
                        break;
                }
                if (ptr->dwName[0] == 0)
                    goto dialog;
                rv = SaveFile(ptr);
                ptr->child->WinMessage(EM_SETMODIFY, 0, 0);
                drawParams(ptr);
                return rv;
            case IDM_CLOSE:
                {
                    rv = ptr->WinMessage(WM_COMMAND, ID_QUERYSAVE, 0);
                    switch (rv)
                    {
                    case IDYES:
                        if (ptr->WinMessage(WM_COMMAND, IDM_SAVE, 0))
                           ptr->WinMessage(WM_CLOSE, 0, 0);
                        break;
                    case IDNO:
                        ptr->WinMessage(WM_CLOSE, 0, 0);
                        break;
                    case IDCANCEL:
                        break;
                    }
                    return rv;
                }
                break;
            case IDM_UNDO:
                FindSetDirty();
                ptr->child->WinMessage(WM_UNDO, 0, 0);
                break;
            case IDM_REDO:
                FindSetDirty();
                ptr->child->WinMessage(WM_REDO, 0, 0);
                break;
            case IDM_CUT:
                FindSetDirty();
                ptr->child->WinMessage(WM_CUT, 0, 0);
                break;
            case IDM_COPY:
                ptr->child->WinMessage(WM_COPY, 0, 0);
                break;
            case IDM_PASTE:
                FindSetDirty();
                ptr->child->WinMessage(WM_PASTE, 0, 0);
                break;
            case IDM_SELECTALL:
                ptr->child->WinMessage(EM_SETSEL, 0,  - 1);
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
                if (finding)
                    DoFindNext();
                else
                    DoReplaceNext();
                break;
            }
            case IDM_TOUPPER:
                ptr->child->WinMessage(EM_TOUPPER, 0, 0);
                break;
            case IDM_TOLOWER:
                ptr->child->WinMessage(EM_TOLOWER, 0, 0);
                break;
            case IDM_COMMENT:
                ptr->child->WinMessage(EM_SELECTCOMMENT, 0, 1);
                break;
            case IDM_UNCOMMENT:
                ptr->child->WinMessage(EM_SELECTCOMMENT, 0, 0);
                break;
            case IDM_INDENT:
                ptr->child->WinMessage(EM_SELECTINDENT, 0,
                    1);
                break;
            case IDM_UNINDENT:
                ptr->child->WinMessage(EM_SELECTINDENT, 0,
                    0);
                break;
            case EN_SETFOCUS:
                break;
            case EN_NEEDFOCUS:
                select();
                child->select();
                setState(sfFocused, True);
                break;
            case EN_CHANGE:
            {
                CHARRANGE xx;
                int lineno;
                ptr->child->WinMessage(EM_EXGETSEL, 0, (LPARAM)&xx);
                lineno = ptr->child->WinMessage(EM_LINEFROMCHAR, xx.cpMin, 0);
                FindSetDirty();
                break;
            }
            }
            
            break;
        case EM_CANREDO:
            return ptr->child->WinMessage(EM_CANREDO, 0, 0)
                ;
        case EM_CANUNDO:
            return ptr->child->WinMessage(EM_CANUNDO, 0, 0)
                ;
        case EN_SETCURSOR:
            drawParams(ptr);
            drawView();
            break;
        case WM_SETTEXT:
        {
            delete title;
            char *ptitle = new char[strlen((char *)lParam) + 1];
            strcpy(ptitle, (char *)lParam);
            title = ptitle;
            frame->drawView();
        }
            break;
        case WM_CREATE:
            //         maximized = TRUE ;			
            ed = (EDITDATA *)lParam;
            if ((int)ed != -1)
            {
                strcpy(ptr->dwTitle, newInfo->dwTitle);
                ptr->WinMessage(WM_SETTEXT, 0, (LPARAM)ptr->dwTitle);
                strcpy(ptr->dwName, newInfo->dwName);
            }
            else
            {
                strcpy(ptr->dwTitle, szUntitled);
                ptr->WinMessage(WM_SETTEXT, 0, (LPARAM)ptr->dwTitle);
            }
            if (!ptr->newFile)
                FileTime(&ptr->time, ptr->dwName);
            bounds = getBounds();
            bounds.b.y -= bounds.a.y;
            bounds.a.y = 0;
            bounds.b.x -= bounds.a.x;
            bounds.a.x = 0;
            bounds.a.y++;
            bounds.b.y--;                
            bounds.a.x = 1;
            bounds.b.x = 9;
            ptr->lineWnd = new LINEWND(bounds);
            bounds = getBounds();
            bounds.b.y -= bounds.a.y;
            bounds.a.y = 0;
            bounds.b.x -= bounds.a.x;
            bounds.a.x = 0;
            if (PropGetBool(NULL, "LINE_NUMBERS"))
            {
                ptr->lineWnd->setState(sfVisible, False);
                bounds.a.x = ptr->lineWnd->lineNumberDigits + 2;
            }
            else
            {
                bounds.a.x = 1;
            }
            bounds.b.x--;
            bounds.a.y++;
            bounds.b.y--;                
            ptr->horizSB = standardScrollBar(sbHorizontal);
            ptr->vertSB = standardScrollBar(sbVertical);
            ptr->child = new EDITDATA(bounds, ptr->horizSB, ptr->vertSB, (EDITDATA *)((ed && ed != (DWINFO *)-1) ? (void *)ed : NULL));
            ptr->child->parent = ptr;
            
            insert(ptr->lineWnd);
            insert(ptr->child);
            
            ptr->dwLineNo =  - 1;
            if (newInfo && !newInfo->newFile && newInfo->dwLineNo !=  -
                1)
            {
                ptr->dwLineNo = newInfo->dwLineNo;
                newInfo->dwLineNo =  - 1;
            }
            if (!ed && newInfo->logMRU)
            {
                cfg.FileMRU().Add(ptr->dwName, ptr->dwTitle);
                cfg.FileMRU().ToMenu(FileMRUMenu);
                cfg.SetChanged();
            }
            if (!ed)
            {
                if (ptr->dwName[0] && (!newInfo || !newInfo->newFile))
                    LoadFile(ptr, FALSE);
                if (ptr->dwLineNo !=  - 1)
                {
                    ptr->WinMessage(WM_COMMAND, IDM_SETLINE, ptr->dwLineNo);
                }
                recolorize(ptr);
            }
            else
            {
                ptr->dosStyle = TRUE;
            }
            if (editWindows)
                editWindows->back = ptr;
            ptr->fwd = editWindows;
            ptr->back = NULL;
            editWindows = ptr;
            ptr->WinMessage(WM_SETLINENUMBERMODE, 2, 0);
            strcpy(buf, ptr->dwName);
            if (ptr->child->id)
                sprintf(buf + strlen(buf), " (%d)", ptr->child->id + 1);
            srcTab->WinMessage(TABM_ADD, (WPARAM)buf, (LPARAM)this);
            WindowMRU.Add(ptr->dwName, ptr->dwTitle, TRUE);
            WindowMRU.ToMenu(NULL);
            return rv;

        case WM_CLOSE:
            EndFind();
            eraseParams();
            WinMessage(WM_DESTROY, 0, 0);
            break;
        case WM_SETSIBLINGID:
            SetTitle(this);
            strcpy(buf, ptr->dwName);
            if (ptr->child->id)
                sprintf(buf + strlen(buf), " (%d)", ptr->child->id + 1);
            srcTab->WinMessage(TABM_RENAME, (WPARAM)buf, (LPARAM)ptr);
            break;
        case WM_DESTROY:
            srcTab->WinMessage(TABM_REMOVE, 0, (LPARAM)this);
            WindowMRU.Remove(ptr->dwName);
            WindowMRU.ToMenu(NULL);
            if (ptr->back)
                ptr->back->fwd = ptr->fwd;
            if (ptr->fwd)
                ptr->fwd->back = ptr->back;
            if (ptr == editWindows)
                editWindows = ptr->fwd;
            ptr->back = ptr->fwd = NULL;
            break;
        case WM_SETLINENUMBERMODE:
        {
            BOOL lineNumbers = PropGetBool(NULL, "LINE_NUMBERS");
            if (wParam)
            {
                    ptr->lineWnd->lineNumberDigits = wParam; // never goes down while the window is open...
            }
            if (lineNumbers)
            {
                TRect r;
                r = lineWnd->getBounds();
                r.b.x = ptr->lineWnd->lineNumberDigits + 2;
                lineWnd->setBounds(r);
                r = child->getBounds();
                r.a.x = ptr->lineWnd->lineNumberDigits + 2;
                child->setBounds(r);
                lineWnd->setState(sfVisible, True);
            }
            else
            {
                TRect r;
                lineWnd->setState(sfVisible, False);
                r = child->getBounds();
                r.a.x = 1;
                child->setBounds(r);
            }
            child->drawView();
            lineWnd->drawView();
            break;
        }
        case WM_SIZE:
        {
            BOOL lineNumbers = PropGetBool(NULL, "LINE_NUMBERS");
            if (lineNumbers)
            {
                TRect r = *(TRect *)lParam;
                r.b -= r.a;
                r.a.x = r.a.y = 1;
                r.b.y--;
                r.b.x = ptr->lineWnd->lineNumberDigits + 1;
                lineWnd->changeBounds(r);
                r = *(TRect *)lParam;
                r.b -= r.a;
                r.a.x = r.a.y = 1;
                r.b.y--;
                r.b.x--;
                r.a.x = ptr->lineWnd->lineNumberDigits + 2;
                child->changeBounds(r);
                lineWnd->setState(sfVisible, True);
            }
            else
            {
                lineWnd->setState(sfVisible, False);
                TRect r = *(TRect *)lParam;
                r.b -= r.a;
                r.a.x = r.a.y = 1;
                r.b.x--;
                r.b.y--;
                child->changeBounds(r);
            }
        }
            
        /*
        case WM_INITMENUPOPUP:
            ptr->child->WinMessage(EM_GETSEL, (WPARAM)
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
            */
        case WM_WORDUNDERCURSOR:
        case WM_WORDUNDERPOINT:
            {
                int rv = ptr->child->WinMessage(iMessage, wParam, lParam);
                return rv;
            }
        case WM_FILETITLE:
            return (DWORD)ptr->dwTitle;
        case WM_FILENAME:
            return (DWORD)ptr->dwName;
        default:
            if (iMessage >= WM_USER)
                return ptr->child->WinMessage(iMessage, wParam, lParam);
            break;
    }
    return 0;
}
void LINEWND::draw()
{
    TDrawBuffer b;
    const int attr= 0xf1; //blue on white
    int y = 0;
    for (int i=0; i < size.y; i++)
    {
        char buf[256], fmt[10];
        sprintf(fmt, "%%0%dd\263", lineNumberDigits);
        sprintf(buf, fmt, i + startLine);
        b.moveCStr(0, buf, attr);
        writeBuf(0, y++, lineNumberDigits + 1, 1, b);
    }
}

