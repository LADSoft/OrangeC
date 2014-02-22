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
#include "winconst.h"
#include "sqlite3.h"

extern int defaultWorkArea;
extern PROJECTITEM *activeProject;
extern HANDLE hInstance;
extern HWND hwndFrame, hwndClient;
extern PROJECTITEM *workArea;

#define JT_NONE 0
#define JT_FUNC 1
#define JT_VAR 2
#define JT_DEFINE 4
#define JT_TYPEDATA 8
#define JT_LOCALDATA 0x10
#define JT_STATIC 0x20
#define JT_GLOBAL 0x40

typedef struct _JUMPLIST
{
    struct _JUMPLIST *next;
    char *name;
    PROJECTITEM *project;
    int type;
} JUMPLIST;

HWND hwndJumpList;

static JUMPLIST *jumpList, **jumpListTail;
static JUMPLIST **jumpSymbols;
static int jumpListCount;
static char szJumpListClassName[] = "xccJumpListClass";
static HWND hwndTypeCombo;
static HWND hwndValueCombo;
static WNDPROC oldProcCombo, oldProcEdit;

static LOGFONT jumpListFontData = 
{
    -14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN |
        FF_DONTCARE,
        "Arial"
};

static int cf(const void *vleft, const void *vright)
{
    const JUMPLIST *left = *(const JUMPLIST**)vleft;
    const JUMPLIST *right = *(const JUMPLIST**)vright;
    return(stricmp(left->name, right->name));
}
static void EnterJumpSymbol(PROJECTITEM *pj, char *name, int type)
{
    if (name)
    {
        if (!(type & JT_GLOBAL))
            return;
            
        if (strncmp(name, "Unnamed++", 9))
        {
            JUMPLIST *njl = calloc(sizeof(JUMPLIST), 1);
            njl->name = strdup(name);
            njl->type = type;
            njl->project = pj;
            *jumpListTail = njl;
            jumpListTail = &njl->next;
            jumpListCount ++ ;
        }
    }
    else
    {
        int i;
        JUMPLIST **js = calloc(sizeof(JUMPLIST *), jumpListCount);
        for (i=0; jumpList; jumpList = jumpList->next, i++)
        {
            js[i] = jumpList;
        }
        qsort(js, jumpListCount, sizeof(JUMPLIST *), cf);
        jumpListTail = &jumpList;
        for (i=0; i< jumpListCount; i++)
        {
            *jumpListTail = js[i];
            jumpListTail = &(*jumpListTail)->next;
            js[i]->next = NULL;
        }
        jumpSymbols = js;
    }
}
#define LIB_BUCKETS 37

static void PopulateCombo(void);
FILE *LoadJumpSymbols(void)
{
    if (workArea)
    {
        PROJECTITEM *pj = workArea->children;

        if (!PropGetBool(NULL, "BROWSE_INFORMATION") || defaultWorkArea || !pj || jumpSymbols)
            return 0;
        jumpListTail = &jumpList;
        jumpListCount = 0;
        while (pj)
        {
            sqlite3 *db = BrowseDBOpen(pj);
            if (db)
            {
                static char *query = {
                    "SELECT name, type FROM names;"
                };
                sqlite3_stmt *handle;
                int rc = sqlite3_prepare_v2(db, query, strlen(query)+1, &handle, NULL);
                if (rc == SQLITE_OK)
                {
                    int done = FALSE;
                    rc = SQLITE_DONE;
                    sqlite3_reset(handle);
                    while (!done)
                    {
                        switch(rc = sqlite3_step(handle))
                        {
                            case SQLITE_BUSY:
                                done = TRUE;
                                break;
                            case SQLITE_DONE:
                                rc = SQLITE_OK;
                                done = TRUE;
                                break;
                            case SQLITE_ROW:
                                EnterJumpSymbol(pj, sqlite3_column_text(handle, 0), sqlite3_column_int(handle, 1));
                                break;
                            default:
                                done = TRUE;
                                break;
                        }
                    }
                    sqlite3_finalize(handle);
                }
                DBClose(db);
            }
            pj = pj->next;
        }
        EnterJumpSymbol(0,0,0);
        PopulateCombo();
        return 1;
    }
    return 0;
}

void FreeJumpSymbols(void)
{
    JUMPLIST *p = jumpList;
    while (p)
    {
        JUMPLIST *next = p->next;
        free(p->name);
        free(p);
        p = next;
    }
    free (jumpSymbols);
    jumpList = 0;
    jumpSymbols = 0;
    SendMessage(hwndValueCombo, CB_RESETCONTENT, 0, 0);
}
static void PopulateCombo(void)
{
    int type = SendMessage(hwndTypeCombo, CB_GETCURSEL, 0, 0);
    int i;
    int count = 0;
    if (!jumpSymbols)
        return ;
    SendMessage(hwndValueCombo, CB_RESETCONTENT, 0, 0);
    for (i=0; i < jumpListCount; i++)
    {
        if (type == 0 && (jumpSymbols[i]->type & JT_FUNC)
            || type == 1 && (jumpSymbols[i]->type & JT_VAR)
            || type == 2 && (jumpSymbols[i]->type & JT_TYPEDATA)
            || type == 3 && (jumpSymbols[i]->type & JT_DEFINE))
        {
            if (i == 0 || strcmp(jumpSymbols[i]->name, jumpSymbols[i-1]->name))
            {
                SendMessage(hwndValueCombo, CB_ADDSTRING, 0, (LPARAM)jumpSymbols[i]->name);
                SendMessage(hwndValueCombo, CB_SETITEMDATA, count++, (LPARAM)jumpSymbols[i]);
            }
        }
    }
}

static JUMPLIST *GetJumpList(void)
{
    int index = SendMessage(hwndValueCombo, CB_GETCURSEL, 0, 0);
    JUMPLIST *jl = (JUMPLIST *)SendMessage(hwndValueCombo, CB_GETITEMDATA, index, 0);
    if (jl)
    {
        if (jl->next && !strcmp(jl->name, jl->next->name))
        {
            JUMPLIST *cur = jl;
            HWND hwnd = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
            char *p = (char *)SendMessage(hwnd, WM_FILENAME, 0, 0);
            PROJECTITEM *pj = HasFile(workArea, p);
            while (pj && pj->type != PJ_PROJ) pj = pj->parent;
            while (!strcmp(cur->name, jl->name))
            {
                if (cur->project == pj)
                    break;
                cur = cur->next;
            }
            if (cur->project == pj)
                jl = cur;
        }
    }
    return jl;
}
static int JumpTo(void)
{
    int rv = 0;
    JUMPLIST *jl;
    if (!PropGetBool(NULL, "BROWSE_INFORMATION") || defaultWorkArea)
        return 0;

    jl = GetJumpList();
    if (jl)
    {
        sqlite3 *db = BrowseDBOpen(jl->project);
        if (db)
        {
            // this query to attempt to find it in the current file
            static char *query = {
                "SELECT FileNames.Name, lineNumbers.startLine FROM LineNumbers "
                "    JOIN FileNames ON FileNames.id = LineNumbers.fileId "
                "    JOIN Names ON Names.id = LineNumbers.symbolId"
                "    WHERE Names.name = ?"
                "       AND (names.type & 64);"
            };
            int rc = SQLITE_OK;
            sqlite3_stmt *handle;
            rc = sqlite3_prepare_v2(db, query, strlen(query)+1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle);
                sqlite3_bind_text(handle, 1, jl->name, strlen(jl->name), SQLITE_STATIC);
                while (!done)
                {
                    switch(rc = sqlite3_step(handle))
                    {
                        case SQLITE_BUSY:
                            done = TRUE;
                            break;
                        case SQLITE_DONE:
                            done = TRUE;
                            break;
                        case SQLITE_ROW:
                        {
                            char *p = sqlite3_column_text(handle, 0);
                            int line = sqlite3_column_int(handle, 1);
                            DWINFO info;
                            InsertBrowse(p, line);
                            memset(&info, 0, sizeof(info));
                            strcpy(info.dwName, p);
                            p = strrchr(info.dwName, '\\');
                            if (p)
                                strcpy(info.dwTitle, p+ 1);
                            info.dwLineNo = line;
                            CreateDrawWindow(&info, TRUE);
                            rv = 1;
                            rc = SQLITE_OK;
                            done = TRUE;
                        }
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
                sqlite3_finalize(handle);
            }
            DBClose(db);
        }
    }
    return rv;
}
void SetJumplistPos(HWND hwnd, int linepos)
{
    char name[256];
    char *p, *q=name;
    PROJECTITEM *pj;

    if (!PropGetBool(NULL, "BROWSE_INFORMATION") || defaultWorkArea || !workArea)
        return;
    p = (char *)SendMessage(hwnd, WM_FILENAME, 0, 0);
    pj = HasFile(workArea, p);
    while (pj && pj->type != PJ_PROJ) pj= pj->parent;
    if (pj)
    {
        sqlite3 *db = BrowseDBOpen(pj);
        if (db)
        {
            static char *query = {
                "SELECT Names.name FROM JumpTable"
                "    JOIN Names ON Names.id = JumpTable.symbolId"
                "    JOIN FileNames On FileNames.id = JumpTable.fileId"
                "    WHERE ? >= JumpTable.startLine AND ? < JumpTable.endLine"
                "    And FileNames.name = ?"
            };
            sqlite3_stmt *handle;
            int rc ;
            for (; *p; q++,p++) *q = tolower(*p);
            *q = 0;
            rc = sqlite3_prepare_v2(db, query, strlen(query)+1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle);
                sqlite3_bind_int(handle, 1, linepos);
                sqlite3_bind_int(handle, 2, linepos);
                sqlite3_bind_text(handle, 3, name, strlen(name), SQLITE_STATIC);
                while (!done)
                {
                    switch(rc = sqlite3_step(handle))
                    {
                        case SQLITE_BUSY:
                            done = TRUE;
                            break;
                        case SQLITE_DONE:
                            done = TRUE;
                            break;
                        case SQLITE_ROW:
                        {
                            int type = SendMessage(hwndTypeCombo, CB_GETCURSEL, 0, 0);
                            int index;
                            if (type != 0)
                            {
                                SendMessage(hwndTypeCombo, CB_SETCURSEL, 0, 0);
                                PopulateCombo();
                            }
                            index = SendMessage(hwndValueCombo, CB_FINDSTRINGEXACT, -1, (LPARAM)sqlite3_column_text(handle, 0));
                            SendMessage(hwndValueCombo, CB_SETCURSEL, index, 0);
                            rc = SQLITE_OK;
                            done = TRUE;
                            break;
                        }
                        default:
                            done = TRUE;
                            break;
                    }
                }
                sqlite3_finalize(handle);
            }
            DBClose(db);
            if (rc != SQLITE_OK)
            {
                SendMessage(hwndValueCombo, CB_SETCURSEL, -1, 0);
            }
        }
    }
    else
    {
        SendMessage(hwndValueCombo, CB_SETCURSEL, -1, 0);
    }
}
// this gets rid of the caret and selection, and routes WM_CHAR events to the parent
// effectively, it makes the window readonly without the readonly background :)
LRESULT CALLBACK ValueEditProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    int rv;
    switch(iMessage)
    {
        case WM_CHAR:
            return SendMessage(GetParent(hwnd), iMessage, wParam, lParam);
        case WM_RBUTTONDBLCLK:
            iMessage = WM_RBUTTONDOWN;
            break;
        case WM_LBUTTONDBLCLK:
            iMessage = WM_LBUTTONDOWN;
            break;
        case EM_SETSEL:
        case EM_EXSETSEL:
            return 0;
        case WM_SETFOCUS:
            rv = CallWindowProc(oldProcEdit, hwnd, iMessage, wParam, lParam);
            DestroyCaret();
            return rv;
        
                        
    }
    return CallWindowProc(oldProcEdit, hwnd, iMessage, wParam, lParam);
}
LRESULT CALLBACK ValueComboProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static char buf[256];
    static int count;
    switch (iMessage)
    {
        case WM_CREATE:
            count = 0;
            buf[0] = 0;
            break;
        case WM_CHAR:
            if (isprint(wParam))
            {
                if (isalnum(wParam) || wParam == '_')
                {
                    if (count < sizeof(buf)-1)
                    {
                        int pos;
                        MessageBeep(0);
                        buf[count++] = wParam;
                        buf[count] = 0;
                        SetStatusMessage(buf , 0);
                        pos = SendMessage(hwnd, CB_FINDSTRING, -1, (LPARAM)buf);
                        if (pos != CB_ERR)						
                        {
                            SendMessage(hwnd, CB_SETCURSEL, pos, 0);
                        }
                    }
                    else
                    {
                        MessageBeep(MB_ICONEXCLAMATION);
                    }
                }
                   return 0;   
            }
            break;
        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_UP:
                case VK_DOWN:
                case VK_PRIOR:
                case VK_NEXT:
                case VK_HOME:
                case VK_END:
                case VK_ESCAPE:
                case VK_RETURN:
                    count = 0;
                    buf[count] = 0;
                    break;
                case VK_BACK:
                    if (count)
                    {
                        int pos;
                        buf[--count] = 0;
                        if (count)
                        {
                            pos = SendMessage(hwnd, CB_FINDSTRING, -1, (LPARAM)buf);
                            if (pos != CB_ERR)
                                SendMessage(hwnd, CB_SETCURSEL, pos, 0);
                        }
                        else
                        {
                            SendMessage(hwnd, CB_SETCURSEL, -1, 0);
                        }
                    }
                    return 0;
            }
            break;
        case CB_RESETCONTENT:
            count = 0;
            buf[count] = 0;
            break;
    }
    return CallWindowProc(oldProcCombo, hwnd, iMessage, wParam, lParam);
}
LRESULT CALLBACK JumpListProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    RECT r;
    POINT pt;
    HWND editWnd;
    HFONT xfont;
    switch (iMessage)
    {
        case WM_SYSCOMMAND:
            if (wParam == SC_CLOSE)
            {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        case WM_COMMAND:
            switch (HIWORD(wParam))
            {
                case CBN_SELENDOK:
                    if (LOWORD(wParam) == 100)
                    {
                        // type
                        PopulateCombo();
                    }
                    else if (LOWORD(wParam) == 200)
                    {
                        JumpTo();
                    }
                    break;
                default:
                    break;
            }
        case WM_GETHEIGHT:
            return 24;
        case WM_SETFOCUS:
            break;
        case WM_CREATE:
            hwndJumpList = hwnd;
            GetClientRect(hwnd, &r);
            hwndTypeCombo = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD + WS_CLIPSIBLINGS +
                WS_BORDER + WS_VISIBLE + CBS_DROPDOWN + CBS_AUTOHSCROLL, 
                                r.left + 70, r.top, 200, 100, hwnd, (HMENU)100, hInstance, 0);
            xfont = CreateFontIndirect(&jumpListFontData);
            SendMessage(hwndTypeCombo, WM_SETFONT, (WPARAM)xfont, 1);
            hwndValueCombo = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD + WS_CLIPSIBLINGS +
                WS_BORDER + WS_VISIBLE + CBS_DROPDOWN + CBS_AUTOHSCROLL + WS_VSCROLL, 
                                r.left + 70, r.top, 200, 200, hwnd, (HMENU)200, hInstance, 0);
            xfont = CreateFontIndirect(&jumpListFontData);
            SendMessage(hwndValueCombo, WM_SETFONT, (WPARAM)xfont, 1);
            oldProcCombo = (WNDPROC)GetWindowLong(hwndValueCombo, GWL_WNDPROC);
            SetWindowLong(hwndValueCombo, GWL_WNDPROC, (long)ValueComboProc);

            pt.x = pt.y = 5;
            editWnd = ChildWindowFromPoint(hwndTypeCombo, pt);
            oldProcEdit = (WNDPROC)GetWindowLong(editWnd, GWL_WNDPROC);
            SetWindowLong(editWnd, GWL_WNDPROC, (long)ValueEditProc);
            editWnd = ChildWindowFromPoint(hwndValueCombo, pt);
            SetWindowLong(editWnd, GWL_WNDPROC, (long)ValueEditProc);
            
            SendMessage(hwndTypeCombo, CB_INSERTSTRING, 0, (LPARAM)"Global Functions");
            SendMessage(hwndTypeCombo, CB_INSERTSTRING, 1, (LPARAM)"Global Variables");
            SendMessage(hwndTypeCombo, CB_INSERTSTRING, 2, (LPARAM)"Global Types");
            SendMessage(hwndTypeCombo, CB_INSERTSTRING, 3, (LPARAM)"Preprocessor Macros");
            SendMessage(hwndTypeCombo, CB_SETCURSEL, 0, 0); 
            CalculateLayout( - 1, FALSE);
            return 0;
        case WM_CLOSE:
            return 0;
        case WM_DESTROY:
            DestroyWindow(hwndValueCombo);
            DestroyWindow(hwndTypeCombo);
            break;

        case WM_SIZE:
            r.left = 0;
            r.right = LOWORD(lParam);
            r.top = 0;
            r.bottom = HIWORD(lParam);
            MoveWindow(hwndTypeCombo, r.left + 30, r.top, (r.right-r.left-100)/2, 100, TRUE);
            MoveWindow(hwndValueCombo, r.left + 30 + (r.right-r.left-100)/2 + 40, r.top, (r.right-r.left-100)/2, 100, TRUE);
            break;
        default:
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}

void CreateJumpListWindow(void)
{
    if (hwndJumpList)
        return ;
    hwndJumpList = CreateWindow(szJumpListClassName, "", WS_CHILD,
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, hwndFrame, 0, hInstance, 0);
}
void RegisterJumpListWindow(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW + CS_VREDRAW;
    wc.lpfnWndProc = &JumpListProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = szJumpListClassName;
    RegisterClass(&wc);
}
