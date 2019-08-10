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
#include <stdio.h>
#include <richedit.h>
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
extern DWINFO* editWindows;
extern HWND hwndFind, hwndASM;
extern int making;
extern enum DebugState uState;
extern PROJECTITEM* activeProject;
extern int defaultWorkArea;

int WindowItemCount = 0;
static int winMenuCount;
static int winShowCount;
static char windowTexts[MAX_WINMENU][260];
static HWND winMenu[MAX_WINMENU];
static WNDPROC oldMenuProc;

static int MenuBitmapIDs[] = {ID_EDITTB, ID_BUILDTB, ID_DEBUGTB, ID_EXTRA, ID_NAVTB, ID_BOOKMARKTB, ID_BROWSETB};
static struct menuBitmap
{
    int id;
    int bitmapNum;
    int submapNum;
    HBITMAP bitmap;
} MenuBitmaps[] = {
    {IDM_NEWFILE, 4, 0},
    {IDM_OPEN, 4, 1},
    {IDM_SAVE, 0, 0},
    {IDM_PRINT, 0, 2},
    {IDM_CUT, 0, 3},
    {IDM_COPY, 0, 4},
    {IDM_PASTE, 0, 5},
    {IDM_UNDO, 0, 6},
    {IDM_REDO, 0, 7},
    {IDM_FIND, 4, 4},
    {IDM_FINDNEXT, 4, 5},
    {IDM_REPLACE, 4, 6},
    {IDM_GOTO, 4, 8},
    {IDM_FINDINFILES, 4, 7},
    {IDM_TOUPPER, 0, 8},
    {IDM_TOLOWER, 0, 9},
    {IDM_INDENT, 0, 10},
    {IDM_UNINDENT, 0, 11},
    {IDM_COMMENT, 0, 12},
    {IDM_UNCOMMENT, 0, 13},
    {IDM_SAVEALL, 0, 1},
    {IDM_COMPILEFILE, 1, 0},
    {IDM_MAKE, 1, 1},
    {IDM_BUILDSELECTED, 1, 2},
    {IDM_BUILDALL, 1, 3},
    {IDM_RUN, 2, 7},
    {IDM_RUNTO, 2, 1},
    {IDM_BREAKPOINT, 2, 2},
    {IDM_STEPIN, 2, 3},
    {IDM_STEPOVER, 2, 4},
    {IDM_STEPOUT, 2, 5},
    {IDM_STOP, 2, 6},
    {IDM_REMOVEALLBREAKPOINTS, 2, 8},
    {IDM_RUNNODEBUG, 2, 9},
    {IDM_CLOSE, 3, 0},
    {IDM_TILEVERT, 3, 1},
    {IDM_TILEHORIZ, 3, 2},
    {IDM_CLOSEWINDOW, 3, 3},
    {IDM_CASCADE, 3, 4},
    {IDM_GENERALPROPERTIES, 3, 5},
    {IDM_PROJECTPROPERTIES, 3, 5},
    //#ifdef XXXXX
    {IDM_COPYWINDOW, 3, 7},
    {IDM_OCIDEHELP, 3, 8},
    {IDM_LANGUAGEHELP, 3, 8},
    {IDM_RTLHELP, 3, 8},
    {IDM_TOOLSHELP, 3, 8},
    {IDM_WEBHELP, 3, 8},
    {IDM_SAVEAS, 3, 9},
    {IDM_OPENWS, 3, 11},
    {IDM_BOOKMARK, 5, 0},
    {IDM_PREVBOOKMARK, 5, 1},
    {IDM_NEXTBOOKMARK, 5, 2},
    {IDM_PREVBOOKMARKFILE, 5, 3},
    {IDM_NEXTBOOKMARKFILE, 5, 4},
    {IDM_BOOKMARKWINDOW, 5, 5},
    {IDM_REMOVEBOOKMARKS, 5, 6},
    {IDM_BROWSETO, 6, 0},
    {IDM_BROWSEBACK, 6, 1},
    {IDM_BROWSETODEFINITION, 6, 2},
    {IDM_BROWSETODECLARATION, 6, 3}
    //#endif
};

//-------------------------------------------------------------------------

BOOL CALLBACK WindowChangeEnumProc(HWND window, LPARAM param)
{
    HWND hwndLV = (HWND)param;
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

long APIENTRY WindowShowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LV_ITEM item;
    switch (message)
    {
        case WM_INITDIALOG:
            ApplyDialogFont((HWND)GetDlgItem(hwnd, IDC_FILELIST));
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
                    SendMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)item.lParam, 0);
                    EndDialog(hwnd, IDOK);
                }
            }
            return 0;
        case WM_COMMAND:
            switch (wParam & 0xffff)
            {
                case IDCANCEL:
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

void ShowWindowList(void) { DialogBoxParam(hInstance, "DLG_MANYWINDOWS", 0, (DLGPROC)WindowShowProc, 0); }
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
        InsertMenuItem(hsub, 1 + winMenuCount + WindowItemCount, TRUE, &mi);
        return FALSE;
    }
    else
    {
        sprintf(windowTexts[winMenuCount], "%2d: ", winMenuCount + 1);
        GetWindowText(window, windowTexts[winMenuCount] + 4, 60);
        mi.dwTypeData = &windowTexts[winMenuCount][0];
        mi.wID = ID_WINDOW_LIST + winMenuCount;
        InsertMenuItem(hsub, 1 + winMenuCount + WindowItemCount, TRUE, &mi);
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
    HBITMAP bitmaps[sizeof(MenuBitmapIDs) / sizeof(int)];
    HDC hDC = GetDC(hwndFrame);
    int i;
    for (i = 0; i < sizeof(MenuBitmapIDs) / sizeof(int); i++)
    {
        bitmaps[i] = LoadBitmap(hInstance, (LPTSTR)MenuBitmapIDs[i]);
    }

    for (i = 0; i < sizeof(MenuBitmaps) / sizeof(struct menuBitmap); i++)
    {
        DWORD n = GetVersion();
        if (LOBYTE(LOWORD(n)) >= 6)
        {
            // windows 7 or later, use transparent bitmaps
            MenuBitmaps[i].bitmap = ConvertToTransparent(
                CopyBitmap(hwndFrame, bitmaps[MenuBitmaps[i].bitmapNum], MenuBitmaps[i].submapNum * 16, 0, 16, 16), 0xc0c0c0);
        }
        else
        {
            // windows XP or earlier, change the bitmap background...
            MenuBitmaps[i].bitmap =
                CopyBitmap(hwndFrame, bitmaps[MenuBitmaps[i].bitmapNum], MenuBitmaps[i].submapNum * 16, 0, 16, 16);
            ChangeBitmapColor(MenuBitmaps[i].bitmap, 0xc0c0c0, RetrieveSysColor(COLOR_MENU));
        }
    }

    for (i = 0; i < sizeof(MenuBitmapIDs) / sizeof(int); i++)
        DeleteObject(bitmaps[i]);
    ReleaseDC(hwndFrame, hDC);
}

void InsertBitmapsInMenu(HMENU hMenu)
{
    int i;
    for (i = 0; i < sizeof(MenuBitmaps) / sizeof(struct menuBitmap); i++)
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

UINT GetMenuCheckedState(HMENU menu, int Id) { return !!(GetMenuState(menu, Id, MF_BYCOMMAND) & MF_CHECKED); }

//-------------------------------------------------------------------------

void SetMenuCheckedState(HMENU menu, int did, int id)
{
    MENUITEMINFO info;
    info.cbSize = sizeof(MENUITEMINFO);
    info.fMask = MIIM_STATE;
    GetMenuItemInfo(menu, id, MF_BYCOMMAND, &info);
    info.fState = (info.fState & ~MFS_CHECKED) | (GetWindowIndex(did) == -1 ? 0 : MFS_CHECKED);
    SetMenuItemInfo(menu, id, MF_BYCOMMAND, &info);
}
void InitMenuPopup(HMENU menu)
{
    HWND win;
    BOOL mf_state;
    BOOL x_state;
    int selstart, selend;
    SetWindowMenu();
    SetMenuCheckedState(menu, DID_PROJWND, IDM_VIEWPROJECT);
    SetMenuCheckedState(menu, DID_PROPSWND, IDM_VIEWPROPS);
    SetMenuCheckedState(menu, DID_INFOWND, IDM_VIEWINFO);
    SetMenuCheckedState(menu, DID_ERRWND, IDM_VIEWERR);
    SetMenuCheckedState(menu, DID_BROWSEWND, IDM_VIEWBROWSE);
    SetMenuCheckedState(menu, DID_STACKWND, IDM_VIEWSTACK);
    SetMenuCheckedState(menu, DID_THREADWND, IDM_VIEWTHREAD);
    {
        MENUITEMINFO info;

        info.cbSize = sizeof(MENUITEMINFO);
        info.fMask = MIIM_STATE;
        GetMenuItemInfo(menu, IDM_VIEWASM, MF_BYCOMMAND, &info);
        info.fState = (info.fState & ~MFS_CHECKED) | (hwndASM ? MFS_CHECKED : 0);
        SetMenuItemInfo(menu, IDM_VIEWASM, MF_BYCOMMAND, &info);
    }
    SetMenuCheckedState(menu, DID_MEMWND, IDM_VIEWMEM);
    SetMenuCheckedState(menu, DID_MEMWND + 1, IDM_VIEWMEM2);
    SetMenuCheckedState(menu, DID_MEMWND + 2, IDM_VIEWMEM3);
    SetMenuCheckedState(menu, DID_MEMWND + 3, IDM_VIEWMEM4);
    SetMenuCheckedState(menu, DID_REGWND, IDM_VIEWREGISTER);
    SetMenuCheckedState(menu, DID_WATCHWND, IDM_VIEWWATCH);
    SetMenuCheckedState(menu, DID_WATCHWND + 1, IDM_VIEWWATCH2);
    SetMenuCheckedState(menu, DID_WATCHWND + 2, IDM_VIEWWATCH3);
    SetMenuCheckedState(menu, DID_WATCHWND + 3, IDM_VIEWWATCH4);
    SetMenuCheckedState(menu, DID_LOCALSWND, IDM_VIEWLOCALS);
    SetMenuCheckedState(menu, DID_BREAKWND, IDM_VIEWBP);

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
        SendMessage(GetDlgItem(win, ID_EDITCHILD), EM_GETSEL, (WPARAM)&selstart, (LPARAM)&selend);
    }
    EnableMenuItem(menu, IDM_COPYWINDOW, mf_state);
    EnableMenuItem(menu, IDM_PRINT, mf_state);
    EnableMenuItem(menu, IDM_TOUPPER, (mf_state == MF_ENABLED && selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_TOLOWER, (mf_state == MF_ENABLED && selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_INDENT, (mf_state == MF_ENABLED && selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_UNINDENT, (mf_state == MF_ENABLED && selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_COMMENT, (mf_state == MF_ENABLED && selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_UNCOMMENT, (mf_state == MF_ENABLED && selstart != selend) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, IDM_SELECTALL, mf_state);
    EnableMenuItem(menu, IDM_FIND, mf_state);

    x_state = mf_state;
    if (!AnyBookmarks())
        x_state = MF_GRAYED;
    EnableMenuItem(menu, IDM_NEXTBOOKMARK, x_state);
    EnableMenuItem(menu, IDM_PREVBOOKMARK, x_state);
    EnableMenuItem(menu, IDM_NEXTBOOKMARKFILE, x_state);
    EnableMenuItem(menu, IDM_PREVBOOKMARKFILE, x_state);
    EnableMenuItem(menu, IDM_BOOKMARKWINDOW, x_state);
    EnableMenuItem(menu, IDM_REMOVEBOOKMARKS, x_state);
    EnableMenuItem(menu, IDM_FIND, mf_state);
    //      EnableMenuItem(menu,IDM_FINDINFILES,mf_state) ;
    EnableMenuItem(menu, IDM_FINDNEXT, mf_state);
    EnableMenuItem(menu, IDM_REPLACE, mf_state);
    x_state = mf_state;
    if (win == hwndASM)
        x_state = MF_ENABLED;
    EnableMenuItem(menu, IDM_GOTO, x_state);
    if (!PropGetBool(NULL, "BROWSE_INFORMATION"))
        mf_state = MF_GRAYED;
    EnableMenuItem(menu, IDM_BROWSETODECLARATION, mf_state);
    EnableMenuItem(menu, IDM_BROWSETODEFINITION, mf_state);

    if (!PropGetBool(NULL, "BROWSE_INFORMATION"))
        mf_state = MF_GRAYED;
    else
        mf_state = MF_ENABLED;
    EnableMenuItem(menu, IDM_VIEWBROWSE, mf_state);
    EnableMenuItem(menu, IDM_BROWSETO, mf_state);
    EnableMenuItem(menu, IDM_BROWSEBACK, mf_state);
    EnableMenuItem(menu, IDM_BOOKMARK, mf_state);
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

    EnableMenuItem(menu, IDM_NEWPROJECT, MF_ENABLED);
    EnableMenuItem(menu, IDM_VIEWPROJECT, MF_ENABLED);
    if (making || hwndGeneralProps)
        mf_state = MF_GRAYED;
    else
        mf_state = MF_ENABLED;
    EnableMenuItem(menu, IDM_CLOSEWS, mf_state);
    EnableMenuItem(menu, IDM_GENMAKE, mf_state);
    EnableMenuItem(menu, IDM_MAKE, mf_state);
    EnableMenuItem(menu, IDM_BUILDALL, mf_state);
    EnableMenuItem(menu, IDM_BUILDSELECTED, mf_state);
    mf_state = MF_GRAYED;
    if (!making && !hwndGeneralProps)
        mf_state = MF_ENABLED;
    EnableMenuItem(menu, IDM_GENERALPROPERTIES, mf_state);
    EnableMenuItem(menu, IDM_SELECTPROFILE, mf_state);
    EnableMenuItem(menu, IDM_BUILDRULES, mf_state);

    if (!activeProject)
        mf_state = MF_GRAYED;
    EnableMenuItem(menu, IDM_PROJECTDEPENDS, mf_state);
    if (uState == notDebugging)
        mf_state = MF_ENABLED;
    else
        mf_state = MF_GRAYED;
    EnableMenuItem(menu, IDM_RUNNODEBUG, mf_state);
    EnableMenuItem(menu, IDM_RUNNODEBUGPROJ, mf_state);
    EnableMenuItem(menu, IDM_RUNPROJ, mf_state);
    if (making)
        mf_state = MF_ENABLED;
    else
        mf_state = MF_GRAYED;
    EnableMenuItem(menu, IDM_STOPBUILD, mf_state);

    if (IsWindow(win))
    {
        EnableMenuItem(menu, IDM_UNDO, SendMessage(win, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);
        EnableMenuItem(menu, IDM_REDO, SendMessage(win, EM_CANREDO, 0, 0) ? MF_ENABLED : MF_GRAYED);
    }
    if (uState == notDebugging)
        mf_state = MF_ENABLED;
    else
        mf_state = MF_GRAYED;
    EnableMenuItem(menu, IDM_OPENWS, mf_state);
    EnableMenuItem(menu, IDM_CLOSEWS, mf_state);
    EnableMenuItem(menu, IDM_NEWWS, mf_state);
    EnableMenuItem(menu, IDM_REOPENWS, mf_state);
    EnableMenuItem(menu, IDM_IMPORTWS, mf_state);

    mf_state = ((uState == notDebugging || uState == atException || uState == atBreakpoint) && !making && activeProject)
                   ? MF_ENABLED
                   : MF_GRAYED;
    EnableMenuItem(menu, IDM_RUN, mf_state);

    mf_state = (uState != notDebugging && (uState == atException || uState == atBreakpoint) && !making && activeProject)
                   ? MF_ENABLED
                   : MF_GRAYED;
    EnableMenuItem(menu, IDM_STOPDEBUGGING, mf_state);
    EnableMenuItem(menu, IDM_STEPIN, mf_state);
    EnableMenuItem(menu, IDM_STEPOUT, mf_state);
    EnableMenuItem(menu, IDM_STEPOVER, mf_state);
    EnableMenuItem(menu, IDM_RUNTO, mf_state);
    EnableMenuItem(menu, IDM_SCROLLTOBP, mf_state);
    //    EnableMenuItem(menu, IDM_VIEWBP, mf_state);
    EnableMenuItem(menu, IDM_VIEWASM, mf_state);
    EnableMenuItem(menu, IDM_VIEWMEM, mf_state);
    EnableMenuItem(menu, IDM_VIEWMEM2, mf_state);
    EnableMenuItem(menu, IDM_VIEWMEM3, mf_state);
    EnableMenuItem(menu, IDM_VIEWMEM4, mf_state);
    EnableMenuItem(menu, IDM_VIEWREGISTER, mf_state);
    EnableMenuItem(menu, IDM_VIEWSTACK, mf_state);
    EnableMenuItem(menu, IDM_VIEWWATCH, mf_state);
    EnableMenuItem(menu, IDM_VIEWWATCH2, mf_state);
    EnableMenuItem(menu, IDM_VIEWWATCH3, mf_state);
    EnableMenuItem(menu, IDM_VIEWWATCH4, mf_state);
    EnableMenuItem(menu, IDM_VIEWLOCALS, mf_state);
    EnableMenuItem(menu, IDM_VIEWTHREAD, mf_state);
    EnableMenuItem(menu, IDM_HBREAK, mf_state);
    EnableMenuItem(menu, IDM_DATABREAKPOINT, mf_state);
    EnableMenuItem(menu, IDM_FUNCTIONBREAKPOINT, mf_state);

    mf_state = uState == Running ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(menu, IDM_STOP, mf_state);

    {
        BOOL state = TagAnyDisabledBreakpoints() || hbpAnyDisabledBreakpoints() || databpAnyDisabledBreakpoints();
        ModifyMenu(menu, IDM_DISABLEALLBREAKPOINTS, MF_BYCOMMAND | MF_STRING, IDM_DISABLEALLBREAKPOINTS,
                   state ? "E&nable All Breakpoints" : "D&isable All Breakpoints");
    }
    mf_state = (TagAnyBreakpoints() || hbpAnyBreakpoints() || databpAnyBreakpoints()) ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(menu, IDM_REMOVEALLBREAKPOINTS, mf_state);
    EnableMenuItem(menu, IDM_DISABLEALLBREAKPOINTS, mf_state);

    mf_state = MF_GRAYED;
    if (activeProject && GetResData(activeProject))
        mf_state = MF_ENABLED;
    EnableMenuItem(menu, IDM_NEWRESOURCE, mf_state);
    mf_state = MF_GRAYED;
    if (IsWindow(win) && IsResourceWindow(win))
    {
        struct resRes* res = (struct resRes*)GetWindowLong(win, 0);
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
            CheckMenuItem(menu, IDM_SNAPTOGRID, MF_BYCOMMAND | MF_CHECKED);
        }
        else
        {
            CheckMenuItem(menu, IDM_SNAPTOGRID, MF_BYCOMMAND);
        }
        if (showGrid)
        {
            CheckMenuItem(menu, IDM_SHOWGRID, MF_BYCOMMAND | MF_CHECKED);
        }
        else
        {
            CheckMenuItem(menu, IDM_SHOWGRID, MF_BYCOMMAND);
        }
    }
    mf_state = MF_GRAYED;
    if (IsWindow(win) && IsResourceWindow(win))
    {
        struct resRes* res = (struct resRes*)GetWindowLong(win, 0);
        if (res->resource->itype == RESTYPE_BITMAP || res->resource->itype == RESTYPE_CURSOR ||
            res->resource->itype == RESTYPE_ICON)
            mf_state = MF_ENABLED;
    }
    EnableMenuItem(menu, IDM_FLIPMENU, mf_state);
    EnableMenuItem(menu, IDM_ROTATEMENU, mf_state);
    EnableMenuItem(menu, IDM_CLEAR, mf_state);
    EnableMenuItem(menu, IDM_CLEARSELECTION, mf_state);

    //         mf_state = uState == atBreakpoint || uState == atException ? MF_ENABLED : MF_GRAYED ;
    //         EnableMenuItem(menu,IDM_HBREAK, mf_state ) ;

    mf_state = PropGetInt(NULL, "MSDN_HELP_MODE") == 0 ? MF_ENABLED : MF_GRAYED;
    EnableMenuItem(menu, IDM_CONFIGWEBHELP, mf_state);
}
void MenuActivateWinmenu(int wParam) { PostMessage(hwndClient, WM_MDIACTIVATE, (WPARAM)winMenu[wParam - ID_WINDOW_LIST], 0); }
HMENU LoadMenuGeneric(HANDLE inst, char* name)
{
    HMENU result = LoadMenu(inst, name);
    if (result)
        InitMenuPopup(result);
    return result;
}
