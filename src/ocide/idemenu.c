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
#include <stdio.h>
#include <float.h>
#include "helpid.h"
#include "header.h"
//#include <dir.h>
#include "wargs.h"
#include "splash.h"
#include "..\version.h"
#include "rc.h"
#include "rcgui.h"
#include <sys\stat.h>

extern BOOL snapToGrid;
extern BOOL showGrid;
extern HWND hwndClient, hwndFrame;
extern HWND hwndGeneralProps;
extern HINSTANCE hInstance;
extern HMENU hMenuMain;
extern DWINFO *editWindows;
extern HWND hwndFind, hwndProject, hwndASM, hwndThread;
extern HWND hwndRegister, hwndMem, hwndTab, hwndWatch, hwndStack;
extern int making;
extern enum DebugStates uState;
extern char szHelpPath[]; 
extern PROJECTITEM *activeProject;

int WindowItemCount = 0;
static int winMenuCount;
static int winShowCount;
static char windowTexts[MAX_WINMENU][260];
static HANDLE *winMenu[MAX_WINMENU];
static WNDPROC oldMenuProc;

static LOGFONT fontdata = 
{
    -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, 
        CONTROL_FONT
};

static int MenuBitmapIDs[] = { ID_EDITTB, ID_BUILDTB , ID_DEBUGTB, ID_EXTRA, ID_NAVTB, ID_BOOKMARKTB };
static struct menuBitmap {
    int id;
    int bitmapNum;
    int submapNum;
    HBITMAP bitmap;
} MenuBitmaps[] = 
{
    {
        IDM_NEWFILE, 4, 0
    }
    , 
    {
        IDM_OPEN, 4, 1
    }
    , 
    {
        IDM_SAVE, 0, 0
    }
    , 
    {
        IDM_PRINT, 0, 2
    }
    , 
    {
        IDM_CUT, 0, 3
    }
    , 
    {
        IDM_COPY, 0, 4
    }
    , 
    {
        IDM_PASTE, 0, 5
    }
    , 
    {
        IDM_UNDO, 0, 6
    }
    , 
    {
        IDM_REDO, 0, 7
    }
    , 
    {
        IDM_FIND, 4, 4
    }
    , 
    {
        IDM_FINDNEXT, 4, 5
    }
    , 
    {
        IDM_REPLACE, 4, 6
    }
    , 
    {
        IDM_GOTO, 4, 8
    }
    , 
    {
        IDM_FINDINFILES, 4,7
    }
    , 
    {
        IDM_TOUPPER, 0, 8
    }
    , 
    {
        IDM_TOLOWER, 0, 9
    }
    , 
    {
        IDM_INDENT, 0, 10
    }
    , 
    {
        IDM_UNINDENT,0, 11
    }
    , 
    {
        IDM_COMMENT, 0, 12
    }
    , 
    {
        IDM_UNCOMMENT,0, 13
    }
    , 
    {
        IDM_SAVEALL, 0, 1
    }
    ,     
    {
        IDM_COMPILEFILE, 1, 0
    }
    , 
    {
        IDM_MAKE, 1, 1
    }
    , 
    {
        IDM_BUILDSELECTED, 1, 2
    }
    , 
    {
        IDM_BUILDALL, 1, 3
    }
    ,
    {
        IDM_RUN, 2, 7
    }
    , 
    {
        IDM_RUNTO, 2, 1
    }
    , 
    {
        IDM_BREAKPOINT, 2, 2
    }
    ,
    {
        IDM_STEPIN, 2, 3
    }
    , 
    {
        IDM_STEPOVER, 2, 4
    }
    , 
    {
        IDM_STEPOUT, 2, 5
    }
    , 
    {
        IDM_STOP, 2, 6
    }
    , 
    {
        IDM_REMOVEALLBREAKPOINTS, 2, 8
    }
    ,
    {
        IDM_RUNNODEBUG, 2, 9
    }
    , 
    {
        IDM_CLOSE,	3, 0
    }
    ,
    {
        IDM_TILEVERT, 3, 1
    }
    ,
    {
        IDM_TILEHORIZ, 3, 2
    }
    ,
    {
        IDM_CLOSEWINDOW, 3, 3
    }
    ,
    {
        IDM_CASCADE, 3, 4
    }
    ,
    {
        IDM_GENERALPROPERTIES, 3, 5
    }
    ,
    {
        IDM_PROJECTPROPERTIES, 3, 5
    }
    ,
//#ifdef XXXXX
    {
        IDM_COPYWINDOW, 3, 7
    }
    ,
    {
        IDM_OCIDEHELP, 3, 8
    }
    ,
    {
        IDM_LANGUAGEHELP, 3, 8
    }
    ,
    {
        IDM_RTLHELP, 3, 8
    }
    ,
    {
        IDM_TOOLSHELP, 3, 8
    }
    ,
    {
        IDM_SPECIFIEDHELP, 3, 8
    }
    ,
    {
        IDM_SAVEAS, 3, 9
    }
    ,
    {
        IDM_OPENWS,3,11
    }
    ,
    {
        IDM_BOOKMARK,5,0
    }
    ,
    {
        IDM_PREVBOOKMARK,5,1
    }
    ,
    {
        IDM_NEXTBOOKMARK,5,2
    }
    ,
    {
        IDM_PREVBOOKMARKFILE,5,3
    }
    ,
    {
        IDM_NEXTBOOKMARKFILE,5,4
    }
    ,
    {
        IDM_BOOKMARKWINDOW,5,5
    }
    ,
    {
        IDM_REMOVEBOOKMARKS,5,6
    }
//#endif
};

//-------------------------------------------------------------------------

BOOL CALLBACK WindowChangeEnumProc(HWND window, LPARAM param)
{
    HWND hwndLV = (HWND)param;
    MENUITEMINFO mi;
    LV_ITEM item;
    char buf[256];
    int v;
    if (GetParent(window) != hwndClient)
        return TRUE;
    if (!IsWindowVisible(window))
        return TRUE;
    GetWindowText(window, buf, 256);
    item.iItem = winShowCount++;
    item.iSubItem = 0;
    item.mask = LVIF_PARAM | LVIF_TEXT;
    item.lParam = (LPARAM)window;
    item.pszText = buf;
    v = ListView_InsertItem(hwndLV, &item);
    ListView_SetCheckState(hwndLV, v, TRUE);
    return TRUE;
}

//-------------------------------------------------------------------------

static int CreateWindowData(HWND hwnd, int changed)
{
    RECT r;
    HWND hwndLV = GetDlgItem(hwnd, IDC_FILELIST);
    LV_COLUMN lvC;

    GetWindowRect(hwndLV, &r);
    lvC.mask = LVCF_WIDTH | LVCF_SUBITEM;
    lvC.cx = r.right - r.left;
    lvC.iSubItem = 0;
    ListView_InsertColumn(hwndLV, 0, &lvC);
    winShowCount = 0;
    EnumChildWindows(hwndClient, WindowChangeEnumProc, (LPARAM)hwndLV);
    return winShowCount;
}

//-------------------------------------------------------------------------

long APIENTRY WindowShowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam)
{
    LV_ITEM item;
    static HFONT hfont;
    switch (message)
    {
        case WM_INITDIALOG:
            hfont = CreateFontIndirect(&fontdata);
            SendMessage(GetDlgItem(hwnd, IDC_FILELIST), WM_SETFONT, (WPARAM)
                hfont, 0);
            if (!CreateWindowData(hwnd, TRUE))
                EndDialog(hwnd, 1);
            else
                CenterWindow(hwnd);
            return 0;
        case WM_NOTIFY:
            if (wParam == IDC_FILELIST)
            {
                if (((LPNMHDR)lParam)->code == NM_DBLCLK)
                {
                    LPNMITEMACTIVATE plvdi = (LPNMITEMACTIVATE)lParam;
                    item.iItem = plvdi->iItem;
                    item.iSubItem = 0;
                    item.mask = LVIF_PARAM;
                    ListView_GetItem(GetDlgItem(hwnd, IDC_FILELIST), &item);
                    SendMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)item.lParam,
                        0);
                    DeleteObject(hfont);
                    EndDialog(hwnd, IDOK);
                }
            }
            return 0;
        case WM_COMMAND:
            switch (wParam &0xffff)
            {
            case IDCANCEL:
                DeleteObject(hfont);
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            break;
        case WM_CLOSE:
            PostMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

void ShowWindowList(void)
{
    DialogBoxParam(hInstance, "DLG_MANYWINDOWS", 0, (DLGPROC)WindowShowProc, 0);
}
//-------------------------------------------------------------------------

BOOL CALLBACK winmenEnumProc(HWND window, LPARAM param)
{
    HANDLE hsub = (HANDLE)param;
    MENUITEMINFO mi;
    if (GetParent(window) != hwndClient)
        return TRUE;
    if (!IsWindowVisible(window))
        return TRUE;
    if (winMenuCount == 0)
    {
        memset(&mi, 0, sizeof(mi));
        mi.cbSize = sizeof(mi);
        mi.fMask = MIIM_TYPE;
        mi.fType = MFT_SEPARATOR;
        InsertMenuItem(hsub, WindowItemCount, TRUE, &mi);
    }
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA;
    mi.fType = MFT_STRING;
    if (winMenuCount == MAX_WINMENU)
    {
        mi.wID = IDM_WINDOW_MORE;
        mi.dwTypeData = "More Windows...";
        InsertMenuItem(hsub, 1+winMenuCount + WindowItemCount, TRUE, &mi);
        return FALSE;
    }
    else
    {
        sprintf(windowTexts[winMenuCount],"%2d: ",winMenuCount+1);
        GetWindowText(window, windowTexts[winMenuCount]+4, 60);
        mi.dwTypeData = &windowTexts[winMenuCount][0];
        mi.wID = ID_WINDOW_LIST + winMenuCount;
        InsertMenuItem(hsub, 1+winMenuCount + WindowItemCount, TRUE, &mi);
        winMenu[winMenuCount] = window;
        winMenuCount++;
        return TRUE;
    }

}

//-------------------------------------------------------------------------

void SetWindowMenu(void)
{
    HANDLE hsub;
    DWORD maxed;
    int count, i;
    SendMessage(hwndClient, WM_MDIGETACTIVE, 0, (LPARAM)&maxed);
    hsub = GetSubMenu(hMenuMain, WindowMenuItem + maxed);
    count = GetMenuItemCount(hsub);
    for (i = WindowItemCount; i < count; i++)
        DeleteMenu(hsub, WindowItemCount, MF_BYPOSITION);
    winMenuCount = 0;
    EnumChildWindows(hwndClient, winmenEnumProc, (LPARAM)hsub);

}


void CreateMenuBitmaps(void)
{
    HBITMAP bitmaps[sizeof(MenuBitmapIDs)/sizeof(int)];
    HDC hDC = GetDC(hwndFrame);
    int i;
    for (i=0; i < sizeof(MenuBitmapIDs)/sizeof(int); i++)
    {
        bitmaps[i] = LoadBitmap(hInstance, (LPTSTR)MenuBitmapIDs[i]);
    }


    for (i=0; i < sizeof(MenuBitmaps)/ sizeof(struct menuBitmap); i++)
    {
        DWORD n = GetVersion();
        if (LOBYTE(LOWORD(n)) >= 6)
        {
            // windows 7 or later, use transparent bitmaps
            MenuBitmaps[i].bitmap = ConvertToTransparent(CopyBitmap(hwndFrame, bitmaps[MenuBitmaps[i].bitmapNum],
                        MenuBitmaps[i].submapNum * 16, 0 , 16, 16), 0xc0c0c0);		
        }
        else
        {   
            // windows XP or earlier, change the bitmap background...
            MenuBitmaps[i].bitmap = CopyBitmap(hwndFrame, bitmaps[MenuBitmaps[i].bitmapNum],
                        MenuBitmaps[i].submapNum * 16, 0 , 16, 16);
            ChangeBitmapColor(MenuBitmaps[i].bitmap, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
        }
    }
    
    for (i=0; i < sizeof(MenuBitmapIDs)/sizeof(int); i++)
        DeleteObject(bitmaps[i]);
    ReleaseDC(hwndFrame, hDC);
}

void InsertBitmapsInMenu(HMENU hMenu)
{
    int i;
    MENUITEMINFO mi;
    for (i=0; i < sizeof(MenuBitmaps)/ sizeof(struct menuBitmap); i++)
    {
        MENUITEMINFO info;
        memset(&info, 0, sizeof(info));
        info.cbSize = sizeof(MENUITEMINFO);
        info.fMask = MIIM_BITMAP;
        info.hbmpItem = MenuBitmaps[i].bitmap;
        SetMenuItemInfo(hMenu, MenuBitmaps[i].id, MF_BYCOMMAND, &info);
//		SetMenuItemBitmaps(hMenu, MenuBitmaps[i].id, MF_BYCOMMAND,
//			MenuBitmaps[i].bitmap, MenuBitmaps[i].bitmap);
    }
}
//-------------------------------------------------------------------------

UINT GetMenuCheckedState(HMENU menu, int Id)
{
    return !!(GetMenuState(menu, Id, MF_BYCOMMAND) &MF_CHECKED);
}

//-------------------------------------------------------------------------

void SetMenuCheckedState(HMENU menu, int did, int id)
{
    MENUITEMINFO info;

    info.cbSize = sizeof(MENUITEMINFO);
    info.fMask = MIIM_STATE;
    GetMenuItemInfo(menu, id, MF_BYCOMMAND, &info);
    info.fState = (info.fState &~MFS_CHECKED) | (dmgrGetHiddenState(did) ? 0 :
        MFS_CHECKED);
    SetMenuItemInfo(menu, id, MF_BYCOMMAND, &info);
}
void InitMenuPopup(HMENU menu)
{
    HWND win;
    BOOL mf_state;
    BOOL x_state;
    int selstart, selend;
    SetWindowMenu();
    /*
    SetMenuCheckedState(menu, DID_BUILDTOOL, IDM_VIEWBUILDBAR);
    SetMenuCheckedState(menu, DID_NAVTOOL, IDM_VIEWNAVBAR);
    SetMenuCheckedState(menu, DID_BOOKMARKTOOL, IDM_VIEWBOOKMARKBAR);
    SetMenuCheckedState(menu, DID_EDITTOOL, IDM_VIEWEDITBAR);
    SetMenuCheckedState(menu, DID_DEBUGTOOL, IDM_VIEWDEBUGBAR);
    */
    SetMenuCheckedState(menu, DID_TABWND, IDM_VIEWPROJECT);
    SetMenuCheckedState(menu, DID_PROPSWND, IDM_VIEWPROPS);
    SetMenuCheckedState(menu, DID_ERRORWND, IDM_VIEWERROR);
    SetMenuCheckedState(menu, DID_STACKWND, IDM_VIEWSTACK);
    SetMenuCheckedState(menu, DID_THREADWND, IDM_VIEWTHREAD);
    SetMenuCheckedState(menu, DID_ASMWND, IDM_VIEWASM);
    SetMenuCheckedState(menu, DID_MEMWND, IDM_VIEWMEM);
    SetMenuCheckedState(menu, DID_REGWND, IDM_VIEWREGISTER);
    SetMenuCheckedState(menu, DID_WATCHWND, IDM_VIEWWATCH);

    win = (HWND)SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0);
    mf_state = MF_GRAYED;
    if (editWindows && IsWindow(win) && (IsEditWindow(win) || IsResourceWindow(win)))
    {
        mf_state = MF_ENABLED;
    }
    EnableMenuItem(menu, IDM_SAVEAS, mf_state);
    EnableMenuItem(menu, IDM_SAVE, mf_state);
    EnableMenuItem(menu, IDM_CUT, mf_state);
    EnableMenuItem(menu, IDM_COPY, mf_state);
    EnableMenuItem(menu, IDM_PASTE, mf_state);
    EnableMenuItem(menu, IDM_CLOSEWINDOW, mf_state);
    EnableMenuItem(menu, IDM_CLOSE, mf_state);
    mf_state = MF_GRAYED;
    if (editWindows && IsWindow(win) && IsEditWindow(win))
    {
        mf_state = MF_ENABLED;
        SendMessage(GetDlgItem(win, ID_EDITCHILD), EM_GETSEL, (WPARAM)
            &selstart, (LPARAM) &selend);
    }
    EnableMenuItem(menu, IDM_COPYWINDOW, mf_state);
    EnableMenuItem(menu, IDM_PRINT, mf_state);
    EnableMenuItem(menu, IDM_TOUPPER, (mf_state == MF_ENABLED &&
        selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_TOLOWER, (mf_state == MF_ENABLED &&
        selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_INDENT, (mf_state == MF_ENABLED &&
        selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_UNINDENT, (mf_state == MF_ENABLED &&
        selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_COMMENT, (mf_state == MF_ENABLED &&
        selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_UNCOMMENT, (mf_state == MF_ENABLED &&
        selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_SELECTALL, mf_state);
    EnableMenuItem(menu, IDM_FIND, mf_state);
    EnableMenuItem(menu, IDM_BROWSE, mf_state);
    // EnableMenuItem(menu,IDM_BROWSETO,mf_state) ;
    // EnableMenuItem(menu,IDM_BROWSEBACK,mf_state) ;
    EnableMenuItem(menu, IDM_BOOKMARK, mf_state);
    x_state = mf_state;
    if (!AnyBookmarks())
        x_state = MF_GRAYED;
    EnableMenuItem(menu,IDM_NEXTBOOKMARK,x_state) ;
    EnableMenuItem(menu,IDM_PREVBOOKMARK,x_state) ;
    EnableMenuItem(menu,IDM_NEXTBOOKMARKFILE,x_state) ;
    EnableMenuItem(menu,IDM_PREVBOOKMARKFILE,x_state) ;
    EnableMenuItem(menu,IDM_BOOKMARKWINDOW,x_state) ;
    EnableMenuItem(menu,IDM_REMOVEBOOKMARKS,x_state) ;
    EnableMenuItem(menu, IDM_FIND, mf_state);
    //      EnableMenuItem(menu,IDM_FINDINFILES,mf_state) ;
    EnableMenuItem(menu, IDM_FINDNEXT, mf_state);
    EnableMenuItem(menu, IDM_REPLACE, mf_state);
    x_state = mf_state;
    if (win == hwndASM || win == hwndMem)
        x_state = MF_ENABLED;
    EnableMenuItem(menu, IDM_GOTO, x_state);
    if (!making && IsEditWindow(win))
    {
        if (!FindItemByWind(win))
            mf_state = MF_GRAYED;
        else
            mf_state = MF_ENABLED;
    }
    else
        mf_state = MF_GRAYED;

    EnableMenuItem(menu, IDM_COMPILEFILE, mf_state);
    mf_state = MF_GRAYED;
    if (SendMessage(hwndClient, WM_MDIGETACTIVE, 0, 0))
        mf_state = MF_ENABLED;
    EnableMenuItem(menu, IDM_CLOSEALLWINDOWS, mf_state);
    EnableMenuItem(menu, IDM_CASCADE, mf_state);
    EnableMenuItem(menu, IDM_TILEHORIZ, mf_state);
    EnableMenuItem(menu, IDM_TILEVERT, mf_state);
    EnableMenuItem(menu, IDM_ARRANGE, mf_state);
    EnableMenuItem(menu, IDM_SAVEALL, mf_state);
    EnableMenuItem(menu, IDM_SAVEALL2, mf_state);

    EnableMenuItem(menu, IDM_NEWPROJECT, MF_ENABLED);
    EnableMenuItem(menu, IDM_VIEWPROJECT, MF_ENABLED);
    if (making || hwndGeneralProps)
        mf_state = MF_GRAYED;
    EnableMenuItem(menu, IDM_CLOSEWS, mf_state);
    EnableMenuItem(menu, IDM_GENMAKE, mf_state);
    EnableMenuItem(menu, IDM_MAKE, mf_state);
    EnableMenuItem(menu, IDM_BUILDALL, mf_state);
    EnableMenuItem(menu, IDM_BUILDSELECTED, mf_state);
    mf_state = MF_GRAYED;
    if (!making && !hwndGeneralProps)
        mf_state = MF_ENABLED;
    EnableMenuItem(menu, IDM_GENERALPROPERTIES, mf_state);
    EnableMenuItem(menu, IDM_PROJECTPROPERTIES, mf_state);
    EnableMenuItem(menu, IDM_SELECTPROFILE, mf_state);
    EnableMenuItem(menu, IDM_ACTIVEPROJECTPROPERTIES, mf_state);
    EnableMenuItem(menu, IDM_BUILDRULES, mf_state);
    if (making)
        mf_state = MF_ENABLED;
    else
        mf_state = MF_GRAYED;
    EnableMenuItem(menu, IDM_STOPBUILD, mf_state);

    if (IsWindow(win))
    {
            EnableMenuItem(menu, IDM_UNDO, SendMessage(win, EM_CANUNDO,
                0, 0) ? MF_ENABLED : MF_GRAYED);
            EnableMenuItem(menu, IDM_REDO, SendMessage(win, EM_CANREDO,
                0, 0) ? MF_ENABLED : MF_GRAYED);
    }

    mf_state = ((uState == notDebugging || uState == atException || uState == atBreakpoint) && !making && activeProject) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(menu, IDM_RUN, mf_state);

    mf_state = (uState != notDebugging && (uState == atException || uState == atBreakpoint)  && !making && activeProject) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(menu, IDM_STOPDEBUGGING, mf_state);
    EnableMenuItem(menu, IDM_RUNTO, mf_state);
    EnableMenuItem(menu, IDM_SCROLLTOBP, mf_state);
    EnableMenuItem(menu, IDM_VIEWASM, mf_state);
    EnableMenuItem(menu, IDM_VIEWMEM, mf_state);
    EnableMenuItem(menu, IDM_VIEWREGISTER, mf_state);
    EnableMenuItem(menu, IDM_VIEWSTACK, mf_state);
    EnableMenuItem(menu, IDM_VIEWWATCH, mf_state);
    EnableMenuItem(menu, IDM_VIEWTHREAD, mf_state);
    EnableMenuItem(menu, IDM_DATABREAKPOINT, mf_state);
    mf_state = uState == Running ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(menu, IDM_STOP, mf_state);

    mf_state = MF_GRAYED;
    if (activeProject && GetResData(activeProject))
        mf_state = MF_ENABLED;       
    EnableMenuItem(menu, IDM_NEWRESOURCE, mf_state);
    mf_state = MF_GRAYED;
    if (IsWindow(win) && IsResourceWindow(win))
    {
        struct resRes *res = (struct resRes *)GetWindowLong(win, 0);
        if (res->resource->itype == RESTYPE_DIALOG)
            mf_state = MF_ENABLED;
    }
    EnableMenuItem(menu, IDM_CREATIONORDER, mf_state);
    EnableMenuItem(menu, IDM_SETTABSTOPS, mf_state);
    EnableMenuItem(menu, IDM_SETGROUPFLAGS, mf_state);
    EnableMenuItem(menu, IDM_GRIDMENU, mf_state);
    EnableMenuItem(menu, IDM_SHOWGRID, mf_state);
    EnableMenuItem(menu, IDM_SNAPTOGRID, mf_state);
    if (mf_state == MF_ENABLED)
    {
        if (snapToGrid)
        {
            CheckMenuItem( menu, IDM_SNAPTOGRID, MF_BYCOMMAND | MF_CHECKED);
        }
        else
        {
            CheckMenuItem( menu, IDM_SNAPTOGRID, MF_BYCOMMAND);
        }
        if (showGrid)
        {
            CheckMenuItem( menu, IDM_SHOWGRID, MF_BYCOMMAND | MF_CHECKED);
        }
        else
        {
            CheckMenuItem( menu, IDM_SHOWGRID, MF_BYCOMMAND);
        }
    }
    mf_state = MF_GRAYED;
    if (IsWindow(win) && IsResourceWindow(win))
    {
        struct resRes *res = (struct resRes *)GetWindowLong(win, 0);
        if (res->resource->itype == RESTYPE_BITMAP || res->resource->itype == RESTYPE_CURSOR || res->resource->itype == RESTYPE_ICON)
            mf_state = MF_ENABLED;
    }
    EnableMenuItem(menu, IDM_FLIPMENU, mf_state);
    EnableMenuItem(menu, IDM_ROTATEMENU, mf_state);
    EnableMenuItem(menu, IDM_CLEAR, mf_state);
    EnableMenuItem(menu, IDM_CLEARSELECTION, mf_state);

    //         mf_state = uState == atBreakpoint || uState == atException ? MF_ENABLED : MF_GRAYED ;
    //         EnableMenuItem(menu,IDM_HBREAK, mf_state ) ;

    mf_state = szHelpPath[0] ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(menu, IDM_SPECIFIEDHELP, mf_state);



}
void MenuActivateWinmenu(int wParam)
{
    PostMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)
        winMenu[wParam - ID_WINDOW_LIST], 0);
}
HMENU LoadMenuGeneric(HANDLE inst, char *name)
{
    HMENU result = LoadMenu(inst, name);
    if (result)
        InitMenuPopup(result);
    return result;
}
