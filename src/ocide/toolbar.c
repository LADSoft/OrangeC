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
#include <string.h>
#include <limits.h>
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>

#include "header.h"
#include <float.h>
#include "helpid.h"
//#include <dir.h>
#include "wargs.h"
#include "splash.h"
#include "..\version.h"
#include <sys\stat.h>

extern PROJECTITEM *activeProject;
extern HWND hwndFrame, hwndClient;
extern HINSTANCE hInstance;
extern DWINFO *editWindows;
extern enum DebugState uState;
extern HWND hwndFind, hwndProject, hwndASM, hwndThread;
extern HWND hwndRegister, hwndMem, hwndTab, hwndWatch, hwndStack;
extern int making;
extern FILEBROWSE *fileBrowseCursor;
extern FILEBROWSE *fileBrowseInfo;
extern PROJECTITEM *activeProject;

HWND hwndTbFind, hwndTbThreads, hwndTbProcedure;

HWND hwndToolNav, hwndToolEdit, hwndToolDebug, hwndToolBuild, hwndToolBookmark, hwndToolThreads;

static char *navhints[] = 
{
    "New", "Open", "", 
    "Back", "Forward", "", "Goto Line [Ctl+G]", "",
    "Find [Ctl+F]", "Find Next [F3]", "Replace [Ctl+H]", "Find In Files", "", 
     "Customize"
};
static char *edithints[] = 
{
    "Save [Ctl+S]", "Save All", "Print", "", 
    "Cut [Ctl+X]", "Copy [Ctl+C]", "Paste [Ctl+V]", "", 
    "Undo [Ctl+Z]", "Redo [Ctl+Y]", "",
    "To Upper Case", "To Lower Case", "Indent [TAB]", "Un-Indent [Shift+TAB]", "Comment", "Uncomment", "Customize"
};

static char *makehints[] = 
{
    "Compile [Ctl+F7]", "Build All [F7]", "Build Active [Shift+F7]", "Rebuild All", "Stop Build", "Customize"
};
static char *debughints[] = 
{
    "Start/Continue Debugging [F5] ", "Stop Debugging [Shift-F5]", "", "Run Without Debugger", "", "Toggle Breakpoint [F9]", "", "Stop", "", "Run to Cursor [Ctl+F5]", 
        "", "Step Into [F11]", "Step Over [F10]", "Step Out [Alt+F11]", "", "Remove All Breakpoints", "Customize"
};
static char *bookmarkhints[] =
{
    "Toggle Bookmark [Alt+F2]", "",
    "Previous Bookmark [Shift+F2]", "Next Bookmark [F2]", "Previous File With Bookmark", "Next File With Bookmark", "",
    "Show Bookmarks [Ctl+F2]", "Remove All Bookmarks", "Customize"
} ;
static char *threadhints[] =
{
    "","Threads", "", "Breakpoints", "Customize"
} ;
static TBBUTTON navButtons[] = 
{
    {
        0, IDM_NEWFILE, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    ,
    {
        1, IDM_OPEN, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    ,
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        2, IDM_BACK, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    ,
    {
        3, IDM_FORWARD, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN
    }
    ,
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        8, IDM_GOTO, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    ,
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        4, IDM_FIND, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        5, IDM_FINDNEXT, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        6, IDM_REPLACE, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        7, IDM_FINDINFILES, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        130, 10000, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT, {0}, 0, -1
    }
    , 
    {
        0, 0, 0, 0
    }
    , 
};
static TBBUTTON editButtons[] = 
{
    {
        0, IDM_SAVE, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        1, IDM_SAVEALL, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        2, IDM_PRINT, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        3, IDM_CUT, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        4, IDM_COPY, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        5, IDM_PASTE, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        6, IDM_UNDO, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        7, IDM_REDO, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        8, IDM_TOUPPER, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        9, IDM_TOLOWER, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        10, IDM_INDENT, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        11, IDM_UNINDENT, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        12, IDM_COMMENT, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        13, IDM_UNCOMMENT, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    ,
    {
        0, 0, 0, 0
    }
    , 
};
static TBBUTTON makeButtons[] = 
{
    {
        0, IDM_COMPILEFILE, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        1, IDM_MAKE, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        2, IDM_BUILDSELECTED, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        3, IDM_BUILDALL, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        4, IDM_STOPBUILD, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, 0, 0
    }
    , 
};

static TBBUTTON debugButtons[] = 
{
    {
        7, IDM_RUN, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        10, IDM_STOPDEBUGGING, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    ,
    {
        9, IDM_RUNNODEBUG, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        2, IDM_BREAKPOINT, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        6, IDM_STOP, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        1, IDM_RUNTO, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        3, IDM_STEPIN, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        4, IDM_STEPOVER, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        5, IDM_STEPOUT, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        8, IDM_REMOVEALLBREAKPOINTS, TBSTATE_ENABLED | TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, 0, 0
    }
    , 
};
static TBBUTTON bookmarkButtons[] = 
{
    {
        0, IDM_BOOKMARK, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        1, IDM_PREVBOOKMARK, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        2, IDM_NEXTBOOKMARK, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        3, IDM_PREVBOOKMARKFILE, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        4, IDM_NEXTBOOKMARKFILE, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT
    }
    , 
    {
        5, IDM_BOOKMARKWINDOW, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        6, IDM_REMOVEBOOKMARKS, TBSTATE_ENABLED |  TBSTATE_WRAP, TBSTYLE_BUTTON
    }
    , 
    {
        0, 0, 0, 0
    }
    , 
} ;
static TBBUTTON threadButtons[] = 
{
    {
        60, 10000, TBSTATE_WRAP, TBSTYLE_SEP, {0}, 0, -1
    }
    ,
    {
        160, 10001, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT, {0}, 0, -1
    }
    ,
    {
        0, 0, TBSTATE_WRAP, TBSTYLE_SEP, {0}, 0, -1 // not a control, a real separator
    }
    ,
    {
        80, 10002, TBSTATE_WRAP, TBSTYLE_SEP, {0}, 0, -1
    }
    ,
    {
        161, 10003, TBSTATE_WRAP, TBSTYLE_SEP | TBSTYLE_FLAT, {0}, 0, -1
    }
    ,
    {
        0, 0, 0, 0
    }
    , 
};
typedef struct
{
    char *text;
    int did;
    HWND *wnd;
} TBCUSTOMDATA;

static TBCUSTOMDATA customData[] = 
{
    { "Bookmark Toolbar", DID_BOOKMARKTOOL, &hwndToolBookmark },
    { "Build Toolbar", DID_BUILDTOOL, &hwndToolBuild },
    { "Debug Toolbar", DID_DEBUGTOOL, &hwndToolDebug },
    { "Edit Toolbar", DID_EDITTOOL, &hwndToolEdit },
    { "Navigation Toolbar", DID_NAVTOOL, &hwndToolNav },    
    { "Threads Toolbar", DID_THREADSTOOL, &hwndToolThreads },
} ;
static void PopulateCustomView(HWND hwnd)
{
    int items = 0;
    int i;
    LV_ITEM item;
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_TOOLCUSTOM);
    LV_COLUMN lvC;
    ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM ;
    lvC.cx = 20;
    lvC.iSubItem = 0;
    ListView_InsertColumn(hwndLV, 0, &lvC);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = 32;
    lvC.iSubItem = 1;
    ListView_InsertColumn(hwndLV, 1, &lvC);
    lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
    lvC.fmt = LVCFMT_LEFT;
    lvC.cx = r.right - r.left - 56;
    lvC.iSubItem = 2;
    ListView_InsertColumn(hwndLV, 2, &lvC);
    
    for (i=0; i < sizeof(customData)/sizeof(customData[0]); i++)
    {
        TBCUSTOMDATA *d = &customData[i];
        int v;
        item.iItem = items++;
        item.iSubItem = 0;
        item.mask = LVIF_PARAM ;
        item.lParam = (LPARAM)i;
        item.pszText = ""; // LPSTR_TEXTCALLBACK ;
        v = ListView_InsertItem(hwndLV, &item);
        ListView_SetCheckState(hwndLV, v, dmgrGetHiddenState(d->did) ? 0 : 1);
    }
    if (items)
    {
        ListView_SetSelectionMark(hwndLV, 0);
        ListView_SetItemState(hwndLV, 0, LVIS_SELECTED, LVIS_SELECTED);
    }

}
LRESULT CALLBACK CustomizeProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam)
{
    static BOOL start;
    switch (iMessage)
    {
        case WM_NOTIFY:
            if (wParam == IDC_TOOLCUSTOM)
            {
                if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO)
                {
                    LV_DISPINFO *plvdi = (LV_DISPINFO*)lParam;
                    plvdi->item.mask |= LVIF_TEXT | LVIF_DI_SETITEM;
                    plvdi->item.mask &= ~LVIF_STATE;
                    switch (plvdi->item.iSubItem)
                    {
                    case 2:
                        plvdi->item.pszText = customData[plvdi->item.iItem].text;
                        break;
                    default:
                        plvdi->item.pszText = "";
                        break;
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED)
                {
                    if (!start)
                    {
                        LPNMLISTVIEW lp = (LPNMLISTVIEW)lParam;
                        int n = lp->uNewState & LVIS_STATEIMAGEMASK;
                        if (n & LVIS_STATEIMAGEMASK)
                        {
                            dmgrHideWindow(customData[lp->iItem].did, !ListView_GetCheckState(lp->hdr.hwndFrom, lp->iItem));
                        }
                        if (lp->uNewState & LVIS_SELECTED)
                        {
                            EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
                        }
                    }
                }
                else if (((LPNMHDR)lParam)->code == LVN_KEYDOWN)
                {
                    switch (((LPNMLVKEYDOWN)lParam)->wVKey )
                    {
                        case VK_INSERT:
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_TOOLCUSTOM);
                                ListView_SetCheckState(hwndLV, -1, TRUE);
                            }
                            else
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_TOOLCUSTOM);
                                int i = ListView_GetSelectionMark(hwndLV);
                                ListView_SetCheckState(hwndLV, i, TRUE);
                            }
                            break;
                        case VK_DELETE:
                            if (GetKeyState(VK_CONTROL) & 0x80000000)
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_TOOLCUSTOM);
                                ListView_SetCheckState(hwndLV, -1, FALSE);
                            }
                            else
                            {
                                HWND hwndLV = GetDlgItem(hwnd, IDC_TOOLCUSTOM);
                                int i = ListView_GetSelectionMark(hwndLV);
                                ListView_SetCheckState(hwndLV, i, FALSE);
                            }
                            break;
                    }
                }
            }
            return 0;
        case WM_COMMAND:
            switch (wParam & 0xffff)
            {
                case IDOK:
                {
                    int n = ListView_GetSelectionMark(GetDlgItem(hwnd, IDC_TOOLCUSTOM));
                    if (n >= 0)
                    {
                        PostMessage(*customData[n].wnd, WM_COMMAND, ID_TOOLBARCUSTOM, 0);
                    }
                    break;
                }
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    break;
                case IDHELP:
                    ContextHelp(IDH_TOOLBAR_SELECT_DIALOG);
                    break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd);
            start = TRUE;
            
            EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
            PopulateCustomView(hwnd);
            start = FALSE;
            break;
    }
    return 0;
}
void TBCustomize(void)
{
    DialogBox(hInstance, "DLG_CUSTOMIZE", hwndFrame, (DLGPROC)CustomizeProc);
}
void RedrawToolBar(void)
{
    HWND win;
    BOOL mf_state;
    BOOL x_state;
    win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
    mf_state = !!SendMessage(win, EM_CANUNDO, 0, 0); 
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_UNDO, MAKELONG
        (mf_state, 0));
    mf_state = !!SendMessage(win, EM_CANREDO, 0, 0);
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_REDO, MAKELONG
        (mf_state, 0));
    mf_state = FALSE;
    if (editWindows && IsWindow(win))
        if (IsEditWindow(win) || IsResourceWindow(win))
        {
            mf_state = TRUE;
        }
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_SAVE, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_SAVEALL, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_CUT, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_COPY, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_PASTE, MAKELONG
        (mf_state, 0));
    mf_state = FALSE;
    if (editWindows && IsWindow(win))
        if (IsEditWindow(win))
        {
            mf_state = TRUE;
        }
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_PRINT, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolNav, TB_ENABLEBUTTON, IDM_FIND, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_TOUPPER, MAKELONG
        (mf_state /*&& selstart != selend*/, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_TOLOWER, MAKELONG
        (mf_state /*&& selstart != selend*/, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_INDENT, MAKELONG
        (mf_state /*&& selstart != selend*/, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_UNINDENT, MAKELONG
        (mf_state /*&& selstart != selend*/, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_COMMENT, MAKELONG
        (mf_state /*&& selstart != selend*/, 0));
    SendMessage(hwndToolEdit, TB_ENABLEBUTTON, IDM_UNCOMMENT, MAKELONG
        (mf_state /*&& selstart != selend*/, 0));
    //         SendMessage(hwndToolEdit,TB_ENABLEBUTTON,IDM_FINDINFILES,MAKELONG(mf_state,0)) ;
    SendMessage(hwndToolNav, TB_ENABLEBUTTON, IDM_FINDNEXT, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolNav, TB_ENABLEBUTTON, IDM_REPLACE, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolNav, TB_ENABLEBUTTON, IDM_GOTO, MAKELONG
        (mf_state || win == hwndASM || win == hwndMem, 0));
    SendMessage(hwndToolBookmark, TB_ENABLEBUTTON, IDM_BOOKMARK, MAKELONG
        (mf_state, 0));
    x_state = AnyBookmarks();
    SendMessage(hwndToolBookmark, TB_ENABLEBUTTON, IDM_NEXTBOOKMARK, MAKELONG
        (x_state, 0));
    SendMessage(hwndToolBookmark, TB_ENABLEBUTTON, IDM_PREVBOOKMARK, MAKELONG
        (x_state, 0));
    SendMessage(hwndToolBookmark, TB_ENABLEBUTTON, IDM_NEXTBOOKMARKFILE, MAKELONG
        (x_state, 0));
    SendMessage(hwndToolBookmark, TB_ENABLEBUTTON, IDM_PREVBOOKMARKFILE, MAKELONG
        (x_state, 0));
    SendMessage(hwndToolBookmark, TB_ENABLEBUTTON, IDM_BOOKMARKWINDOW, MAKELONG
        (x_state, 0));
    SendMessage(hwndToolBookmark, TB_ENABLEBUTTON, IDM_REMOVEBOOKMARKS, MAKELONG
        (x_state, 0));
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_BREAKPOINT,
        MAKELONG(mf_state || win == hwndASM, 0));

    if (mf_state)
    {
        if (making)
            mf_state = FALSE;
        if (!FindItemByWind(win))
            mf_state = FALSE;
    }
    SendMessage(hwndToolBuild, TB_ENABLEBUTTON, IDM_COMPILEFILE,
        MAKELONG(mf_state, 0));
    mf_state = activeProject && making ? TRUE : FALSE;
    SendMessage(hwndToolBuild, TB_ENABLEBUTTON, IDM_STOPBUILD, MAKELONG
        (mf_state, 0));
    mf_state = activeProject && !making ? TRUE : FALSE;
    SendMessage(hwndToolBuild, TB_ENABLEBUTTON, IDM_MAKE, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolBuild, TB_ENABLEBUTTON, IDM_BUILDSELECTED, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolBuild, TB_ENABLEBUTTON, IDM_BUILDALL, MAKELONG
        (mf_state, 0));

    mf_state = uState == notDebugging && !making && activeProject;
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_RUNNODEBUG, MAKELONG
        (mf_state, 0));
    
    mf_state = uState == atBreakpoint || uState == atException || (uState == notDebugging && !making && activeProject);
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_RUN, MAKELONG
        (mf_state, 0));
    mf_state = (uState == atBreakpoint || uState == atException);
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_RUNTO, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_STEPOVER, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_STEPIN, MAKELONG
        (mf_state, 0));
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_STEPOUT, MAKELONG
        (mf_state, 0));
    
    mf_state = uState != notDebugging && uState != Running && !making && activeProject;
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_STOPDEBUGGING, MAKELONG
        (mf_state, 0));

    mf_state = TagAnyBreakpoints();
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_REMOVEALLBREAKPOINTS, MAKELONG
        (mf_state, 0));

    mf_state = uState == Running;
    SendMessage(hwndToolDebug, TB_ENABLEBUTTON, IDM_STOP, MAKELONG
        (mf_state, 0));
    
    mf_state = fileBrowseInfo && fileBrowseInfo->next && (!fileBrowseCursor || fileBrowseCursor->next);
    SendMessage(hwndToolNav, TB_ENABLEBUTTON, IDM_FORWARD, MAKELONG
        (mf_state, 0));
    mf_state = fileBrowseInfo && fileBrowseInfo->next && fileBrowseCursor;
    SendMessage(hwndToolNav, TB_ENABLEBUTTON, IDM_BACK, MAKELONG
        (mf_state, 0));
    
}

int ToolBarDropDown(int id, int lParam)
{
    if (id == IDM_FORWARD)
    {
        POINT p;
        RECT rt;
        GetWindowRect(hwndToolNav, &rt);
        p.x = LOWORD(lParam) + rt.left;
        p.y = HIWORD(lParam) + rt.top;
        FileBrowseMenu(p.x, p.y, hwndToolNav);
    }
    return 1;
}

LRESULT CALLBACK tbStatProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
    lParam)
{
    HWND hwndStatic;
    RECT r;
    LPCREATESTRUCT lpcp;
    HDC dc;
    switch (iMessage)
    {
        case WM_CREATE:
            lpcp = (LPCREATESTRUCT) lParam;
            hwndStatic = CreateWindowEx(0, "static", lpcp->lpszName, WS_CHILD +
                WS_VISIBLE | SS_CENTERIMAGE, 0,0,100, 200, hwnd, 0, lpcp->hInstance, 0);  
            ApplyDialogFont(hwndStatic);
            SetWindowLong(hwnd, GWL_USERDATA, (long)hwndStatic);
            break;
        case WM_DESTROY:
            DestroyWindow((HWND)GetWindowLong(hwnd, GWL_USERDATA));
            break;
        case WM_SIZE:
            GetClientRect(hwnd, &r);
            MoveWindow((HWND)GetWindowLong(hwnd, GWL_USERDATA), r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
            return 0;
        case WM_ERASEBKGND:
            GetClientRect(hwnd, &r);
            dc = GetDC(hwnd);
            FillGradientTB(dc, &r, FALSE);
            ReleaseDC(hwnd, dc);
            return 0;
        case WM_CTLCOLORSTATIC:
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
//-------------------------------------------------------------------------
static void RegisterTbControls(void)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = &tbStatProc;
    wc.lpszClassName = "xccTbStatic";
    wc.hInstance = hInstance;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 4;
    wc.style = CS_DBLCLKS;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    RegisterClass(&wc);
}
void MakeToolBar(HWND hwnd)
{
    HWND hwndTemp;
    RegisterTbControls();
    hwndToolEdit = CreateToolBarWindow(DID_EDITTOOL, hwndFrame, hwndFrame, 16,
        15, ID_EDITTB, 17, editButtons, edithints, 0, "Edit Tools", IDH_EDIT_TOOLBAR);
    hwndToolNav = CreateToolBarWindow(DID_NAVTOOL, hwndFrame, hwndFrame, 16,
        15, ID_NAVTB, 13, navButtons, navhints, 0, "Nav Tools", IDH_NAV_TOOLBAR);
    hwndToolBuild = CreateToolBarWindow(DID_BUILDTOOL, hwndFrame, hwndFrame,
        16, 15, ID_BUILDTB, 5, makeButtons, makehints, 0, "Build Tools", IDH_BUILD_TOOLBAR);
    hwndToolDebug = CreateToolBarWindow(DID_DEBUGTOOL, hwndFrame, hwndFrame,
        16, 15, ID_DEBUGTB, 15, debugButtons, debughints, 0, "Debug Tools", IDH_DEBUG_TOOLBAR);
    hwndToolBookmark = CreateToolBarWindow(DID_BOOKMARKTOOL, hwndFrame, hwndFrame,
        16, 15, ID_BOOKMARKTB, 8, bookmarkButtons, bookmarkhints, 0, "Bookmark Tools", IDH_BOOKMARK_TOOLBAR);
    hwndToolThreads = CreateToolBarWindow(DID_THREADSTOOL, hwndFrame, hwndFrame,
        16, 15, ID_THREADSTB, 4, threadButtons, threadhints, 0, "Bookmark Tools", IDH_THREAD_TOOLBAR);
  
    hwndTemp = CreateWindowEx(WS_EX_TRANSPARENT, "xccTbStatic", "Thread:", WS_CHILD +
        WS_VISIBLE, 
                        0,0,100, 200, hwndFrame, 0, hInstance, 0);  
    SendMessage(hwndToolThreads, LCF_ADDCONTROL, 0, (LPARAM)hwndTemp);
    hwndTbThreads = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD + WS_BORDER +
        WS_VISIBLE + WS_TABSTOP + CBS_DROPDOWN + CBS_AUTOHSCROLL, 
                        0,0,100, 200, hwndThread, (HMENU)ID_TBTHREADS, hInstance, 0);  
    SendMessage(hwndToolThreads, LCF_ADDCONTROL, 1, (LPARAM)hwndTbThreads);
    hwndTemp = CreateWindowEx(WS_EX_TRANSPARENT, "xccTbStatic", "Procedure:", WS_CHILD +
        WS_VISIBLE, 
                        0,0,100, 200, hwndFrame, 0, hInstance, 0);  
    SendMessage(hwndToolThreads, LCF_ADDCONTROL, 2, (LPARAM)hwndTemp);
    hwndTbProcedure = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD + WS_BORDER +
        WS_VISIBLE + WS_TABSTOP + CBS_DROPDOWN + CBS_AUTOHSCROLL, 
                        0,0,100, 200, hwndStack, (HMENU)ID_TBPROCEDURE, hInstance, 0);  
    SendMessage(hwndToolThreads, LCF_ADDCONTROL, 3, (LPARAM)hwndTbProcedure);
  
    hwndTbFind = CreateWindowEx(0, "COMBOBOX", "", WS_CHILD + WS_BORDER +
        WS_VISIBLE + WS_TABSTOP + CBS_DROPDOWN + CBS_AUTOHSCROLL, 
                        0,0,100, 200, hwndFrame, (HMENU)ID_TBFIND, hInstance, 0);  
    SendMessage(hwndToolNav, LCF_ADDCONTROL, 0, (LPARAM)hwndTbFind);
    SubClassHistoryCombo(hwndTbFind);
    
    ApplyDialogFont(hwndTbFind);
    ApplyDialogFont(hwndTbThreads);
    ApplyDialogFont(hwndTbProcedure);
    
    EnableWindow(hwndTbThreads, FALSE);
    EnableWindow(hwndTbProcedure, FALSE);
}
