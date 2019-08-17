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
#include <limits.h>
#include <stdio.h>
#include <float.h>
#include <richedit.h>
#include "helpid.h"
#include "header.h"
//#include <dir.h>
#include "wargs.h"
#include "splash.h"
#include "..\version.h"
#include <sys\stat.h>

extern HWND hwndFrame, hwndClient, hwndStatus, hwndJumpList, hwndSrcTab, hwndToolbarBar;
extern HINSTANCE hInstance;
extern int defaultWorkArea;
#define EXTRA 1000

int sbheight;

#define UNDOCKABLE 0

#define POSITION_MASK 0xf00
#define COUNT_MASK 0xff

#define CLIENT 0x1000
#define DOCKABLE_DEBUG 0x2000
#define DOCKABLE_EDIT 0x4000
#define DOCKABLE_INITIAL 0x8000
#define CHANGED 0x10000

#define TOP 1 * 0x100
#define BOTTOM 2 * 0x100
#define LEFT 3 * 0x100
#define RIGHT 4 * 0x100

#define MAX_DOCK 50
#define FRAMEWIDTH 5

#define DEFAULT_WINDOW_WIDTH 210
#define DEFAULT_WINDOW_HEIGHT 180

DOCK_STR debugDocks[MAX_DOCK], releaseDocks[MAX_DOCK];
int debugDockCount, releaseDockCount;
HWND debugfreeWindows[100], releasefreeWindows[100];
int debugfreeWindowCount, releasefreeWindowCount;
HWND handles[MAX_HANDLES];
RECT oldFrame;

static HBITMAP dockCenterBmp, dockLeftBmp, dockRightBmp, dockTopBmp, dockBottomBmp;
static char* szDockRectClassName = "xccDockRect";
static char* szDockHighlightRectClassName = "xccDockHighlightRectRect";
int freeWindowCount;
int dockCount;
static HWND lastWindowUnderCursor;
static HWND* freeWindows;
static DOCK_STR* docks;
static int dockSelected;
static HWND left;
static HWND right;
static HWND top;
static HWND bottom;
static HWND center;
static RECT dragPos;
static BOOL moving;
static POINT pt, pt2;

typedef HWND CreateFunc(void);
typedef void RegisterFunc(HINSTANCE);
typedef struct
{
    int CTL_ID;  // or zero for not a control;
    int defaultDock;
    CreateFunc* cfunc;
    RegisterFunc* rfunc;
    HWND* cFunc;

} WINDOW_STR;

static int frameCount = 0;
static int framePos = 0;
static HWND frames[100];

static WINDOW_STR windowDeclarations[MAX_HANDLES] = {
    {0, UNDOCKABLE, NULL, RegisterXeditWindow},
    {0, UNDOCKABLE, NULL, RegisterextTreeWindow},
    {0, UNDOCKABLE, NULL, RegisterControlWindow},
    {0, UNDOCKABLE, NULL, RegisterFrameWindow},
    {0, UNDOCKABLE, NULL, RegisterToolBarWindow},
    {0, UNDOCKABLE, NULL, RegisterLsTabWindow},
    {0, UNDOCKABLE, NULL, RegisterTTIPWindow},
    {0, UNDOCKABLE, NULL, RegisterHistoryComboWindow},
    {0, UNDOCKABLE, NULL, RegisterPropWindows},
    {0, UNDOCKABLE, NULL, RegisterImageWindows},

    {0, UNDOCKABLE, NULL, RegisterDrawWindow},
    {0, UNDOCKABLE, NULL, RegisterASMWindow},
    {0, UNDOCKABLE, NULL, RegisterDlgDrawWindow},
    {0, UNDOCKABLE, NULL, RegisterAcceleratorDrawWindow},
    {0, UNDOCKABLE, NULL, RegisterImageDrawWindow},
    {0, UNDOCKABLE, NULL, RegisterMenuDrawWindow},
    {0, UNDOCKABLE, NULL, RegisterVersionDrawWindow},
    {0, UNDOCKABLE, NULL, RegisterStringTableDrawWindow},
    {0, UNDOCKABLE, NULL, RegisterRCDataDrawWindow},
    {0, UNDOCKABLE, NULL, RegisterJumpListWindow},

    {DID_PROJWND, CLIENT | LEFT | DOCKABLE_INITIAL | DOCKABLE_DEBUG | DOCKABLE_EDIT, CreateProjectWindow, RegisterProjectWindow},
    {DID_RESWND, CLIENT | LEFT | DOCKABLE_INITIAL | DOCKABLE_EDIT, CreateResourceWindow, RegisterResourceWindow},
    {DID_REGWND, CLIENT | RIGHT | DOCKABLE_DEBUG, CreateRegisterWindow, RegisterRegisterWindow},
    {DID_PROPSWND, CLIENT | RIGHT | DOCKABLE_INITIAL | DOCKABLE_EDIT, CreatePropsWindow, RegisterPropsWindow},
    {DID_CTLTBWND, CLIENT | RIGHT | DOCKABLE_EDIT, CreateCtlTbWindow, RegisterCtlTbWindow},
    {DID_ERRWND, BOTTOM, CreateErrorWindow, RegisterErrorWindow},
    {DID_INFOWND, BOTTOM | DOCKABLE_INITIAL | DOCKABLE_DEBUG | DOCKABLE_EDIT, CreateInfoWindow, RegisterInfoWindow},
    {DID_FINDWND, BOTTOM | DOCKABLE_INITIAL | DOCKABLE_DEBUG | DOCKABLE_EDIT, CreateFind1Window, RegisterFindWindow},
    {DID_FINDWND + 1, BOTTOM | 1 | DOCKABLE_DEBUG | DOCKABLE_EDIT, CreateFind2Window, RegisterFindWindow},
    {DID_WATCHWND, BOTTOM | DOCKABLE_DEBUG | DOCKABLE_INITIAL, CreateWatch1Window, RegisterWatchWindow},
    {DID_WATCHWND + 1, BOTTOM | 1 | DOCKABLE_DEBUG, CreateWatch2Window, RegisterWatchWindow},
    {DID_WATCHWND + 2, TOP | DOCKABLE_DEBUG, CreateWatch3Window, RegisterWatchWindow},
    {DID_WATCHWND + 3, TOP | 1 | DOCKABLE_DEBUG, CreateWatch4Window, RegisterWatchWindow},
    {DID_BROWSEWND, BOTTOM | 1 | DOCKABLE_EDIT, CreateBrowseWindow, RegisterBrowseWindow},
    {DID_LOCALSWND, BOTTOM | 1 | DOCKABLE_DEBUG, CreateLocalsWindow, RegisterWatchWindow},
    {DID_STACKWND, BOTTOM | 1 | DOCKABLE_DEBUG | DOCKABLE_INITIAL, CreateStackWindow, RegisterStackWindow},
    {DID_THREADWND, BOTTOM | 1 | DOCKABLE_DEBUG, CreateThreadWindow, RegisterThreadWindow},
    {DID_MEMWND, BOTTOM | 1 | DOCKABLE_DEBUG, CreateMem1Window, RegisterMemWindow},
    {DID_MEMWND + 1, BOTTOM | 0 | DOCKABLE_DEBUG, CreateMem2Window, RegisterMemWindow},
    {DID_MEMWND + 2, TOP | 1 | DOCKABLE_DEBUG, CreateMem3Window, RegisterMemWindow},
    {DID_MEMWND + 3, TOP | 0 | DOCKABLE_DEBUG, CreateMem4Window, RegisterMemWindow},
    {DID_BREAKWND, BOTTOM | DOCKABLE_DEBUG, CreateBreakWindow, RegisterBreakWindow},
};
void GetFrameWindowRect(RECT* r)
{
    GetWindowRect(hwndFrame, r);
    r->top += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYFRAME);
    r->bottom -= GetSystemMetrics(SM_CYFRAME);
    r->left += GetSystemMetrics(SM_CXFRAME);
    r->right -= GetSystemMetrics(SM_CXFRAME);

    r->bottom -= sbheight;
}
void GetRelativeRect(HWND parent, HWND self, RECT* r)
{
    POINT pt;
    GetWindowRect(self, r);
    pt.x = r->left;
    pt.y = r->top;
    ScreenToClient(parent, &pt);
    r->bottom = r->bottom - r->top + pt.y;
    r->right = r->right - r->left + pt.x;
    r->left = pt.x;
    r->top = pt.y;
}
void RegisterAllWindows(void)
{
    int i;
    for (i = 0; i < sizeof(windowDeclarations) / sizeof(windowDeclarations[0]); i++)
        windowDeclarations[i].rfunc(hInstance);
}
void CreateAllWindows(void)
{
    int i;
    for (i = 0; i < sizeof(windowDeclarations) / sizeof(windowDeclarations[0]); i++)
        if (windowDeclarations[i].cfunc)
            handles[windowDeclarations[i].CTL_ID] = windowDeclarations[i].cfunc();
}
HWND GetWindowHandle(int wndId) { return handles[wndId]; }
LRESULT PostDIDMessage(int DID, unsigned iMessage, WPARAM wParam, LPARAM lParam)
{
    return PostMessage(GetWindowHandle(DID), iMessage, wParam, lParam);
}
LRESULT SendDIDMessage(int DID, unsigned iMessage, WPARAM wParam, LPARAM lParam)
{
    return SendMessage(GetWindowHandle(DID), iMessage, wParam, lParam);
}
static void Show(int flag)
{
    int i;
    for (i = 0; i < dockCount; i++)
        ShowWindow(docks[i].hwnd, flag ? SW_SHOW : SW_HIDE);
    for (i = 0; i < freeWindowCount; i++)
        ShowWindow(freeWindows[i], flag ? SW_SHOW : SW_HIDE);
}
void Select(int flag)
{
    RECT r;
    GetClientRect(hwndFrame, &r);
    if (!flag)
        flag = dockSelected;
    if (dockSelected & DOCKABLE_EDIT)
    {
        releaseDockCount = dockCount;
        releasefreeWindowCount = freeWindowCount;
    }
    if (dockSelected & DOCKABLE_DEBUG)
    {
        debugDockCount = dockCount;
        debugfreeWindowCount = freeWindowCount;
    }
    if (flag != dockSelected)
    {
        Show(FALSE);
        dockSelected = flag;
        if (dockSelected & DOCKABLE_EDIT)
        {
            dockCount = releaseDockCount;
            docks = releaseDocks;
            freeWindowCount = releasefreeWindowCount;
            freeWindows = releasefreeWindows;
        }
        if (dockSelected & DOCKABLE_DEBUG)
        {
            dockCount = debugDockCount;
            docks = debugDocks;
            freeWindowCount = debugfreeWindowCount;
            freeWindows = debugfreeWindows;
        }
        ResizeLayout(NULL);
        Show(TRUE);
    }
    else
    {
        Show(FALSE);
        ResizeLayout(NULL);
        Show(TRUE);
    }
}
void SelectDebugWindows(BOOL dbgMode)
{
    if (dbgMode)
        Select(DOCKABLE_DEBUG);
    else
        Select(DOCKABLE_EDIT);
}
int GetWindowIndex(int DID)
{
    int i;
    for (i = 0; i < dockCount; i++)
    {
        if (docks[i].hwnd)
        {
            if (SendMessage(docks[i].hwnd, TW_HAS, 0, (LPARAM)handles[DID]))
            {
                return i;
            }
        }
    }
    return -1;
}
void RemoveWindow(int DID)
{
    int i = GetWindowIndex(DID);
    if (i >= 0)
        SendMessage(docks[i].hwnd, TW_REMOVE, 0, (LPARAM)handles[DID]);
}
static void PositionWindow(int z)
{
    int i, j;
    int start, end, bottom;
    RECT frame;
    RECT toolBar;
    RECT statusBar;
    GetClientRect(hwndFrame, &frame);
    GetClientRect(hwndToolbarBar, &toolBar);
    GetClientRect(hwndStatus, &statusBar);
    frame.top += toolBar.bottom - toolBar.top;
    frame.bottom -= statusBar.bottom - statusBar.top;
    // this isn't going to be general, it is only in support of the expected layout
    switch (docks[z].ctl & POSITION_MASK)
    {
        case LEFT:
            docks[z].r.left = 0;
            docks[z].r.right = DEFAULT_WINDOW_WIDTH * EXTRA;
            if (z != dockCount - 1 && (docks[z + 1].ctl & POSITION_MASK) == LEFT)
            {
                for (i = z + 1; i < dockCount && (docks[i].ctl & POSITION_MASK) == LEFT; i++)
                {
                    docks[i].r.left += DEFAULT_WINDOW_WIDTH * EXTRA;
                    docks[i].r.right += DEFAULT_WINDOW_WIDTH * EXTRA;
                }
                docks[z].r.top = docks[z + 1].r.top;
                docks[z].r.bottom = docks[z + 1].r.bottom;
            }
            else
            {
                for (i = 0; i < dockCount && (docks[i].ctl & POSITION_MASK) != BOTTOM; i++)
                    ;
                for (j = 0; j < dockCount && (docks[j].ctl & POSITION_MASK) != TOP; j++)
                    ;
                for (; j < dockCount - 1 && (docks[j + 1].ctl & POSITION_MASK) == TOP; j++)
                    ;
                if (i == dockCount)
                    docks[z].r.bottom = frame.bottom * EXTRA;
                else
                    docks[z].r.bottom = docks[i].r.top;
                if (j == dockCount)
                    docks[z].r.top = frame.top * EXTRA;
                else
                    docks[z].r.top = docks[i].r.bottom;
            }
            break;
        case RIGHT:
            docks[z].r.left = (frame.right - DEFAULT_WINDOW_WIDTH) * EXTRA;
            docks[z].r.right = frame.right * EXTRA;
            if (z && (docks[z - 1].ctl & POSITION_MASK) == RIGHT)
            {
                for (i = z - 1; i >= 0 && (docks[i].ctl & POSITION_MASK) == RIGHT; i--)
                {
                    docks[i].r.left -= DEFAULT_WINDOW_WIDTH * EXTRA;
                    docks[i].r.right -= DEFAULT_WINDOW_WIDTH * EXTRA;
                }
                docks[z].r.top = docks[z - 1].r.top;
                docks[z].r.bottom = docks[z - 1].r.bottom;
            }
            else
            {
                for (i = 0; i < dockCount && (docks[i].ctl & POSITION_MASK) != BOTTOM; i++)
                    ;
                for (j = 0; j < dockCount && (docks[j].ctl & POSITION_MASK) != TOP; j++)
                    ;
                for (; j < dockCount - 1 && (docks[j + 1].ctl & POSITION_MASK) == TOP; j++)
                    ;
                if (i == dockCount)
                    docks[z].r.bottom = frame.bottom * EXTRA;
                else
                    docks[z].r.bottom = docks[i].r.top;
                if (j == dockCount)
                    docks[z].r.top = frame.top * EXTRA;
                else
                    docks[z].r.top = docks[i].r.bottom;
            }
            break;
        case BOTTOM:
        case TOP:
            start = end = z;

            if (start & (docks[start - 1].ctl & POSITION_MASK) == (docks[start].ctl & POSITION_MASK))
            {
                docks[start].r = docks[start - 1].r;
                bottom = -1;
            }
            else
            {
                if ((docks[start].ctl & POSITION_MASK) == TOP)
                {
                    bottom = docks[start].r.top = frame.top * EXTRA;
                    docks[start].r.bottom = docks[start].r.top + DEFAULT_WINDOW_HEIGHT * EXTRA;
                }
                else
                {
                    bottom = docks[start].r.bottom = frame.bottom * EXTRA;
                    docks[start].r.top = docks[start].r.bottom - DEFAULT_WINDOW_HEIGHT * EXTRA;
                }
            }
            while (start && (docks[start - 1].ctl & POSITION_MASK) == (docks[start].ctl & POSITION_MASK) &&
                   docks[start - 1].r.top == docks[start].r.top && docks[start - 1].r.bottom == docks[start].r.bottom)
                start--;
            while (end < dockCount - 1 && (docks[end + 1].ctl & POSITION_MASK) == (docks[start].ctl & POSITION_MASK) &&
                   docks[end + 1].r.top == docks[start].r.top && docks[end + 1].r.bottom == docks[start].r.bottom)
                end++;
            for (j = start; j < end + 1; j++)
            {

                if (j == start)
                    docks[j].r.left = frame.left * EXTRA;
                else
                    docks[j].r.left = docks[j - 1].r.right;
                if (j == end)
                {
                    docks[j].r.right = frame.right * EXTRA;
                }
                else
                {
                    docks[j].r.right = docks[j].r.left + (frame.right - frame.left) * EXTRA / (end + 1 - start);
                }
            }
            if (start == end && bottom != -1)
            {
                if ((docks[start].ctl & POSITION_MASK) == TOP)
                {
                    for (i = 0; i < dockCount; i++)
                    {
                        if (i != z && docks[i].r.top == bottom)
                        {
                            docks[i].r.top += DEFAULT_WINDOW_HEIGHT * EXTRA;
                        }
                    }
                }
                else
                {
                    for (i = 0; i < dockCount; i++)
                    {
                        if (i != z && docks[i].r.bottom == bottom)
                        {
                            docks[i].r.bottom -= DEFAULT_WINDOW_HEIGHT * EXTRA;
                        }
                    }
                }
            }
            break;
    }
}
static BOOL InContact(int start, int end, int start2, int end2)
{
    int middle = (start2 + end2) / 2;
    return (start < middle && end > middle);
}
static void ExtendLeft(DOCK_STR* dock, int w)
{
    int j;
    for (j = 0; j < dockCount; j++)
    {
        if (InContact(dock->r.top, dock->r.bottom, docks[j].r.top, docks[j].r.bottom))
            if (docks[j].r.left == dock->r.right)
            {
                switch (docks[j].ctl & POSITION_MASK)
                {
                    case TOP:
                    case BOTTOM:
                        if (!(docks[j].ctl & CHANGED))
                        {
                            docks[j].ctl |= CHANGED;
                            docks[j].r.left = dock->r.left;
                        }
                        break;
                    case LEFT:
                        ExtendLeft(docks + j, w);
                        docks[j].r.left -= w;
                        docks[j].r.right -= w;
                        break;
                }
            }
    }
}
static void ExtendRight(DOCK_STR* dock, int w)
{
    int j;
    for (j = 0; j < dockCount; j++)
    {
        if (InContact(dock->r.top, dock->r.bottom, docks[j].r.top, docks[j].r.bottom))
            if (docks[j].r.right == dock->r.left)
            {
                switch (docks[j].ctl & POSITION_MASK)
                {
                    case TOP:
                    case BOTTOM:
                        if (!(docks[j].ctl & CHANGED))
                        {
                            docks[j].ctl |= CHANGED;
                            docks[j].r.right = dock->r.right;
                        }
                        break;
                    case RIGHT:
                        ExtendRight(docks + j, w);
                        docks[j].r.left += w;
                        docks[j].r.right += w;
                        break;
                }
            }
    }
}
static void ExtendTop(DOCK_STR* dock, int w)
{
    int j;
    for (j = 0; j < dockCount; j++)
    {
        if (InContact(dock->r.left, dock->r.right, docks[j].r.left, docks[j].r.right))
            if (docks[j].r.top == dock->r.bottom)
            {
                switch (docks[j].ctl & POSITION_MASK)
                {
                    case RIGHT:
                    case LEFT:
                        if (!(docks[j].ctl & CHANGED))
                        {
                            docks[j].ctl |= CHANGED;
                            docks[j].r.top = dock->r.top;
                        }
                        break;
                    case TOP:
                        ExtendTop(docks + j, w);
                        docks[j].r.top -= w;
                        docks[j].r.bottom -= w;
                        break;
                }
            }
    }
}
static void ExtendBottom(DOCK_STR* dock, int w)
{
    int j;
    for (j = 0; j < dockCount; j++)
    {
        if (InContact(dock->r.left, dock->r.right, docks[j].r.left, docks[j].r.right))
            if (docks[j].r.bottom == dock->r.top)
            {
                switch (docks[j].ctl & POSITION_MASK)
                {
                    case RIGHT:
                    case LEFT:
                        if (!(docks[j].ctl & CHANGED))
                        {
                            docks[j].ctl |= CHANGED;
                            docks[j].r.bottom = dock->r.bottom;
                        }
                        break;
                    case BOTTOM:
                        ExtendBottom(docks + j, w);
                        docks[j].r.top += w;
                        docks[j].r.bottom += w;
                        break;
                }
            }
    }
}
static void ExtendSurroundingWindows(DOCK_STR* dock, HWND hwnd)
{
    int i;
    for (i = 0; i < dockCount; i++)
        docks[i].ctl &= ~CHANGED;
    switch (dock->ctl & POSITION_MASK)
    {
        case LEFT:
            if (dock != docks && dock[-1].r.bottom == dock->r.top && dock[-1].r.left == dock->r.left &&
                dock[-1].r.right == dock->r.right)
            {
                dock[-1].r.bottom = dock->r.bottom;
            }
            else if (dock != docks + dockCount - 1 && dock->r.bottom == dock[1].r.top && dock[1].r.left == dock->r.left &&
                     dock[1].r.right == dock->r.right)
            {
                dock[1].r.top = dock->r.top;
            }
            else
            {
                ExtendLeft(dock, dock->r.right - dock->r.left);
            }
            break;
        case RIGHT:
            if (dock != docks && dock[-1].r.bottom == dock->r.top && dock[-1].r.left == dock->r.left &&
                dock[-1].r.right == dock->r.right)
            {
                dock[-1].r.bottom = dock->r.bottom;
            }
            else if (dock != docks + dockCount - 1 && dock->r.bottom == dock[1].r.top && dock[1].r.left == dock->r.left &&
                     dock[1].r.right == dock->r.right)
            {
                dock[1].r.top = dock->r.top;
            }
            else
            {
                ExtendRight(dock, dock->r.right - dock->r.left);
            }
            break;
        case TOP:
            if (dock != docks && dock[-1].r.right == dock->r.left && dock[-1].r.top == dock->r.top &&
                dock[-1].r.bottom == dock->r.bottom)
            {
                dock[-1].r.right = dock->r.right;
            }
            else if (dock != docks + dockCount - 1 && dock->r.right == dock[1].r.left && dock[1].r.top == dock->r.top &&
                     dock[1].r.bottom == dock->r.bottom)
            {
                dock[1].r.left = dock->r.left;
            }
            else
            {
                ExtendTop(dock, dock->r.bottom - dock->r.top);
            }
            break;
        case BOTTOM:
            if (dock != docks && dock[-1].r.right == dock->r.left && dock[-1].r.top == dock->r.top &&
                dock[-1].r.bottom == dock->r.bottom)
            {
                dock[-1].r.right = dock->r.right;
            }
            else if (dock != docks + dockCount - 1 && dock->r.right == dock[1].r.left && dock[1].r.top == dock->r.top &&
                     dock[1].r.bottom == dock->r.bottom)
            {
                dock[1].r.left = dock->r.left;
            }
            else
            {
                ExtendBottom(dock, dock->r.bottom - dock->r.top);
            }
            break;
    }
}
BOOL SelectWindowInternal(int DID, BOOL select)
{
    BOOL rv = FALSE;
    WINDOW_STR* p = NULL;
    int i = GetWindowIndex(DID);
    int count = 0;
    if (i >= 0)
    {
        SendMessage(docks[i].hwnd, TW_SELECT, select, (LPARAM)handles[DID]);
        return FALSE;
    }
    for (i = 0; i < freeWindowCount; i++)
    {
        if (SendMessage(freeWindows[i], TW_HAS, select, (LPARAM)handles[DID]))
            return FALSE;
    }
    for (i = 0; i < sizeof(windowDeclarations) / sizeof(windowDeclarations[0]); i++)
    {
        if (windowDeclarations[i].CTL_ID == DID)
        {
            p = &windowDeclarations[i];
            break;
        }
    }
    if (p)
    {
        int n1 = -1, n2 = -1, z = -1;
        for (i = 0; i < dockCount; i++)
        {
            if ((p->defaultDock & POSITION_MASK) == (docks[i].ctl & POSITION_MASK))
            {
                if (n1 == -1 && !(docks[i].ctl & CLIENT))
                {
                    n1 = i;
                }
                if (n2 == -1 && (docks[i].ctl & CLIENT))
                {
                    n2 = i;
                }
            }
        }
        if (p->defaultDock & CLIENT)
        {
            if (n2 != -1)
                z = n2;
            else if (n1 != -1)
                z = n1;
        }
        else
        {
            if (n1 != -1)
                z = n1;
            else if (n2 != -1)
                z = n2;
        }
        if (z == -1)
        {
            z = dockCount;
        }
        if ((p->defaultDock & COUNT_MASK))
        {
            if (z < dockCount - 1 && (docks[z].ctl & POSITION_MASK) == (docks[z + 1].ctl & POSITION_MASK))
                switch (docks[z].ctl & POSITION_MASK)
                {
                    case TOP:
                    case BOTTOM:
                        if (docks[z].r.top == docks[z + 1].r.top)
                            if (docks[z].r.bottom = docks[z + 1].r.bottom)
                            {
                                SendMessage(docks[z + 1].hwnd, TW_ADD, select, (LPARAM)handles[DID]);
                                return rv;
                            }
                        break;
                    case LEFT:
                    case RIGHT:
                        if (docks[z].r.right == docks[z + 1].r.right)
                            if (docks[z].r.left = docks[z + 1].r.left)
                            {
                                SendMessage(docks[z + 1].hwnd, TW_ADD, select, (LPARAM)handles[DID]);
                                return rv;
                            }
                        break;
                }
            if (z != dockCount)
                z++;
        }
        else if (z < dockCount && !(docks[z].ctl & COUNT_MASK))
        {
            SendMessage(docks[z].hwnd, TW_ADD, select, (LPARAM)handles[DID]);
            return rv;
        }
        if (z < dockCount)
            memmove(&docks[z + 1], &docks[z], sizeof(DOCK_STR) * (dockCount - z - 1));
        dockCount++;
        docks[z].ctl = p->defaultDock;
        docks[z].hwnd = CreateControlWindow(hwndFrame);
        PositionWindow(z);
        SendMessage(docks[z].hwnd, TW_ADD, select, (LPARAM)handles[DID]);
        ShowWindow(docks[z].hwnd, SW_SHOW);
        rv = TRUE;
    }
    return rv;
}
void SelectWindow(int DID)
{
    if (SelectWindowInternal(DID, TRUE))
        ResizeLayout(NULL);
}
static void PutWindow(HWND hwnd, RECT* r)
{
    InvalidateRect(hwnd, 0, 0);
    MoveWindow(hwnd, r->left, r->top, r->right - r->left, r->bottom - r->top, 1);
}
static void CreateFrame(int left, int right, int top, int bottom)
{
    HANDLE hwnd;
    if (framePos < frameCount)
    {
        hwnd = frames[framePos++];
    }
    else
    {
        hwnd = CreateFrameWindow(hwndFrame);
        frames[frameCount++] = hwnd;
        framePos++;
    }
    ShowWindow(hwnd, SW_SHOW);
    MoveWindow(hwnd, left, top, right - left, bottom - top, 1);
}
static void PutFrames(void)
{

    int i;
    for (i = 0; i < framePos; i++)
        ShowWindow(frames[i], SW_HIDE);
    framePos = 0;
    for (i = 0; i < dockCount; i++)
    {
        switch (docks[i].ctl & POSITION_MASK)
        {
            case LEFT:
                docks[i].display.right -= FRAMEWIDTH;
                CreateFrame(docks[i].display.right, docks[i].display.right + FRAMEWIDTH, docks[i].display.top,
                            docks[i].display.bottom);
                if (i && (docks[i - 1].ctl & POSITION_MASK) == (docks[i].ctl & POSITION_MASK) &&
                    docks[i - 1].r.bottom == docks[i].r.top && docks[i - 1].r.left == docks[i].r.left &&
                    docks[i - 1].r.right == docks[i].r.right)
                {
                    docks[i].display.top += FRAMEWIDTH;
                    CreateFrame(docks[i].display.left, docks[i].display.right, docks[i].display.top - FRAMEWIDTH,
                                docks[i].display.top);
                }
                break;
            case RIGHT:
                docks[i].display.left += FRAMEWIDTH;
                CreateFrame(docks[i].display.left - FRAMEWIDTH, docks[i].display.left, docks[i].display.top,
                            docks[i].display.bottom);
                if (i && (docks[i - 1].ctl & POSITION_MASK) == (docks[i].ctl & POSITION_MASK) &&
                    docks[i - 1].r.bottom == docks[i].r.top && docks[i - 1].r.left == docks[i].r.left &&
                    docks[i - 1].r.right == docks[i].r.right)
                {
                    docks[i].display.top += FRAMEWIDTH;
                    CreateFrame(docks[i].display.left, docks[i].display.right, docks[i].display.top - FRAMEWIDTH,
                                docks[i].display.top);
                }
                break;
            case TOP:
                docks[i].display.bottom -= FRAMEWIDTH;
                CreateFrame(docks[i].display.left, docks[i].display.right, docks[i].display.bottom,
                            docks[i].display.bottom + FRAMEWIDTH);
                if (i && (docks[i - 1].ctl & POSITION_MASK) == (docks[i].ctl & POSITION_MASK) &&
                    docks[i - 1].r.right == docks[i].r.left && docks[i - 1].r.top == docks[i].r.top &&
                    docks[i - 1].r.bottom == docks[i].r.bottom)
                {
                    docks[i].display.left += FRAMEWIDTH;
                    CreateFrame(docks[i].display.left - FRAMEWIDTH, docks[i].display.left, docks[i].display.top,
                                docks[i].display.bottom);
                }
                break;
            case BOTTOM:
                docks[i].display.top += FRAMEWIDTH;
                CreateFrame(docks[i].display.left, docks[i].display.right, docks[i].display.top - FRAMEWIDTH, docks[i].display.top);
                if (i && (docks[i - 1].ctl & POSITION_MASK) == (docks[i].ctl & POSITION_MASK) &&
                    docks[i - 1].r.right == docks[i].r.left && docks[i - 1].r.top == docks[i].r.top &&
                    docks[i - 1].r.bottom == docks[i].r.bottom)
                {
                    docks[i].display.left += FRAMEWIDTH;
                    CreateFrame(docks[i].display.left - FRAMEWIDTH, docks[i].display.left, docks[i].display.top,
                                docks[i].display.bottom);
                }
                break;
        }
    }
}
void GetSpanWidth(DOCK_STR* one, RECT* r)
{
    DOCK_STR *start = one, *end = one;
    while (start > docks)
    {
        if ((start[-1].ctl & POSITION_MASK) != (one->ctl & POSITION_MASK))
            break;
        if (start[-1].r.bottom != -1 && start[-1].r.bottom != one->r.bottom)
            break;
        start--;
    }
    while (end < docks + dockCount - 1)
    {
        if ((end[1].ctl & POSITION_MASK) != (one->ctl & POSITION_MASK))
            break;
        if (end[1].r.bottom != -1 && end[1].r.bottom != one->r.bottom)
            break;
        end++;
    }
    r->left = start->display.left;
    r->right = end->display.right;
}
void GetSpanHeight(DOCK_STR* one, RECT* r)
{
    DOCK_STR *start = one, *end = one;
    while (start > docks)
    {
        if ((start[-1].ctl & POSITION_MASK) != (one->ctl & POSITION_MASK))
            break;
        if (start[-1].r.right != -1 && start[-1].r.right != one->r.right)
            break;
        start--;
    }
    while (end < docks + dockCount - 1)
    {
        if ((end[1].ctl & POSITION_MASK) != (one->ctl & POSITION_MASK))
            break;
        if (end[1].r.right != -1 && end[1].r.right != one->r.right)
            break;
        end++;
    }
    r->top = start->display.top;
    r->bottom = end->display.bottom;
}
void ResizeDockWindow(HWND hwnd, int ox, int oy, int x, int y)
{
    RECT r;
    RECT client;
    RECT toolBar;
    POINT curs;
    int i;
    BOOL changed = FALSE;
    BOOL readjust = TRUE;
    DOCK_STR *one = NULL, *two = NULL;
    GetCursorPos(&curs);
    MapWindowPoints(HWND_DESKTOP, hwndFrame, &curs, 1);
    GetWindowRect(hwnd, &r);
    GetWindowRect(hwndClient, &client);
    GetClientRect(hwndToolbarBar, &toolBar);
    MapWindowPoints(HWND_DESKTOP, hwndFrame, (LPPOINT)&r, 2);
    client.top -= toolBar.bottom - toolBar.top;
    client.bottom -= toolBar.bottom - toolBar.top;
    if (r.bottom - r.top > r.right - r.left)
    {
        // vert
        int dx = (x - ox) * EXTRA;
        int right;
        for (i = 0; i < dockCount; i++)
            if (docks[i].display.top <= curs.y && docks[i].display.bottom > curs.y)
            {
                if (docks[i].display.left == r.right)
                {
                    two = &docks[i];
                }
                else if (docks[i].display.right == r.left)
                {
                    one = &docks[i];
                }
            }
        if (one && two)
        {
            if ((one->ctl & POSITION_MASK) == (two->ctl & POSITION_MASK))
            {
                if ((one->ctl & POSITION_MASK) != LEFT && (one->ctl & POSITION_MASK) != RIGHT)
                {
                    readjust = FALSE;
                }
            }
        }
        if (one && two)
        {
            if ((one->ctl & POSITION_MASK) == LEFT || (one->ctl & POSITION_MASK) == RIGHT)
            {
                right = one->r.right;
            }
            else
            {
                one = two;
                right = one->r.left;
            }
        }
        else if (!one)
        {
            one = two;
            right = one->r.left;
        }
        else
        {
            right = one->r.right;
        }
        if (one)
        {
            if (readjust)
            {

                GetSpanHeight(one, &r);
            }
            for (i = 0; i < dockCount; i++)
            {
                if (docks[i].display.top >= r.top && docks[i].display.bottom <= r.bottom)
                {
                    if (docks[i].r.right == right)
                    {
                        if (docks[i].r.right - docks[i].r.left + dx < 100 * EXTRA)
                            return;
                    }
                    else if (docks[i].r.left == right)
                    {
                        if (docks[i].r.right - docks[i].r.left - dx < 100 * EXTRA)
                            return;
                    }
                }
            }
            if (right == client.right)
                if (client.right - client.left + dx < 100 * EXTRA)
                    return;
            if (right == client.left)
                if (client.right - client.left - dx < 100 * EXTRA)
                    return;
            for (i = 0; i < dockCount; i++)
            {
                if (docks[i].display.top >= r.top && docks[i].display.bottom <= r.bottom)
                {
                    if (docks[i].r.right == right)
                    {
                        docks[i].r.right += dx;
                        changed = TRUE;
                    }
                    else if (docks[i].r.left == right)
                    {
                        docks[i].r.left += dx;
                        changed = TRUE;
                    }
                }
            }
        }
    }
    else
    {
        // horiz
        int dy = (y - oy) * EXTRA;
        int bottom;
        for (i = 0; i < dockCount; i++)
            if (docks[i].display.left <= curs.x && docks[i].display.right > curs.x)
            {
                if (docks[i].display.top == r.bottom)
                {
                    two = &docks[i];
                }
                else if (docks[i].display.bottom == r.top)
                {
                    one = &docks[i];
                }
            }
        if (one && two)
        {
            if ((one->ctl & POSITION_MASK) == (two->ctl & POSITION_MASK))
            {
                if ((one->ctl & POSITION_MASK) != BOTTOM && (one->ctl & POSITION_MASK) != TOP)
                {
                    readjust = FALSE;
                }
            }
        }
        if (one && two)
        {
            if ((one->ctl & POSITION_MASK) == TOP || (one->ctl & POSITION_MASK) == BOTTOM)
            {
                bottom = one->r.bottom;
            }
            else
            {
                one = two;
                bottom = one->r.top;
            }
        }
        else if (!one)
        {
            one = two;
            bottom = one->r.top;
        }
        else
        {
            bottom = one->r.bottom;
        }
        if (one)
        {
            if (readjust)
            {
                GetSpanWidth(one, &r);
            }
            for (i = 0; i < dockCount; i++)
            {
                if (docks[i].display.left >= r.left && docks[i].display.right <= r.right)
                {
                    if (docks[i].r.bottom == bottom)
                    {
                        if (docks[i].r.bottom - docks[i].r.top + dy < 100 * EXTRA)
                            return;
                    }
                    else if (docks[i].r.top == bottom)
                    {
                        if (docks[i].r.bottom - docks[i].r.top - dy < 100 * EXTRA)
                            return;
                    }
                }
            }
            if (bottom == client.bottom)
                if (client.bottom - client.top + dy < 100 * EXTRA)
                    return;
            if (bottom == client.top)
                if (client.bottom - client.top - dy < 100 * EXTRA)
                    return;
            for (i = 0; i < dockCount; i++)
            {
                if (docks[i].display.left >= r.left && docks[i].display.right <= r.right)
                {
                    if (docks[i].r.bottom == bottom)
                    {
                        docks[i].r.bottom += dy;
                        changed = TRUE;
                    }
                    else if (docks[i].r.top == bottom)
                    {
                        docks[i].r.top += dy;
                        changed = TRUE;
                    }
                }
            }
        }
    }
    if (changed)
    {
        ResizeLayout(0);
    }
}
static void ResizeAllWindows(int dx, int dy)
{
    int i;
    DOCK_STR* arr[100];
    int count;
    if (dx)
    {
        for (i = 0; i < dockCount; i++)
            if ((docks[i].ctl & POSITION_MASK) == RIGHT)
            {
                docks[i].r.right += dx;
                docks[i].r.left += dx;
            }
        count = 0;
        for (i = 0; i < dockCount;)
        {
            if ((docks[i].ctl & POSITION_MASK) == TOP || (docks[i].ctl & POSITION_MASK) == BOTTOM)
            {
                int j;
                int len = 0, len2;
                while (!count || (arr[count - 1]->ctl & POSITION_MASK) == (docks[i].ctl & POSITION_MASK) &&
                                     arr[count - 1]->r.top == docks[i].r.top && arr[count - 1]->r.bottom == docks[i].r.bottom)
                {
                    arr[count++] = &docks[i++];
                }
                len = arr[count - 1]->r.right - arr[0]->r.left;
                len2 = len + dx;
                len /= EXTRA;
                len2 /= EXTRA;
                if (len == 0)
                    len = len2;
                for (j = 0; j < count; j++)
                {
                    if (j)
                        arr[j]->r.left = arr[j - 1]->r.right;
                    if (j != count - 1)
                    {
                        int w = arr[j]->r.right - arr[j]->r.left;
                        w = w * len2 / len;
                        arr[j]->r.right = arr[j]->r.left + w;
                    }
                    else
                    {
                        arr[j]->r.right += dx;
                    }
                }
                count = 0;
            }
            else
            {
                i++;
            }
        }
    }
    if (dy)
    {
        for (i = 0; i < dockCount; i++)
            if ((docks[i].ctl & POSITION_MASK) == BOTTOM)
            {
                docks[i].r.bottom += dy;
                docks[i].r.top += dy;
            }
        count = 0;
        for (i = 0; i < dockCount;)
        {
            if ((docks[i].ctl & POSITION_MASK) == LEFT || (docks[i].ctl & POSITION_MASK) == RIGHT)
            {
                int j;
                int len = 0, len2;
                while (!count || (arr[count - 1]->ctl & POSITION_MASK) == (docks[i].ctl & POSITION_MASK) &&
                                     arr[count - 1]->r.left == docks[i].r.left && arr[count - 1]->r.right == docks[i].r.right)
                {
                    arr[count++] = &docks[i++];
                }
                len = arr[count - 1]->r.bottom - arr[0]->r.top;
                len2 = len + dy;
                len /= EXTRA;
                len2 /= EXTRA;
                if (len == 0)
                    len = len2;
                for (j = 0; j < count; j++)
                {
                    if (j)
                        arr[j]->r.top = arr[j - 1]->r.bottom;
                    if (j != count - 1)
                    {
                        int h = arr[j]->r.bottom - arr[j]->r.top;
                        h = h * len2 / len;
                        arr[j]->r.bottom = arr[j]->r.top + h;
                    }
                    else
                    {
                        arr[j]->r.bottom += dy;
                    }
                }
                count = 0;
            }
            else
            {
                i++;
            }
        }
    }
}
static void ReTopAllWindows(RECT* old, RECT* new)
{
    if (old->top != new->top)
    {
        int i;
        for (i = 0; i < debugDockCount; i++)
        {
            if (debugDocks[i].r.top == old->top * EXTRA)
                debugDocks[i].r.top = new->top* EXTRA;
        }
        for (i = 0; i < releaseDockCount; i++)
        {
            if (releaseDocks[i].r.top == old->top * EXTRA)
                releaseDocks[i].r.top = new->top* EXTRA;
        }
    }
}
void ResizeLayout(RECT* rect)
{
    int jmpListHeight = 0;
    int i;
    RECT client;
    RECT frame;
    RECT statusBar;
    RECT toolBar;
    BOOL settop = FALSE;
    int deltax = INT_MAX, deltay = INT_MAX;
    GetClientRect(hwndStatus, &statusBar);
    sbheight = statusBar.bottom - statusBar.top;
    if (rect)
    {
        frame = *rect;
    }
    else
    {
        GetClientRect(hwndFrame, &frame);
    }
    GetClientRect(hwndToolbarBar, &toolBar);
    frame.bottom -= statusBar.bottom - statusBar.top;
    client = frame;
    frame.top += toolBar.bottom - toolBar.top;
    if (oldFrame.right == 0)
        oldFrame = frame;
    deltax = frame.right - oldFrame.right;
    deltay = frame.bottom - oldFrame.bottom;
    ResizeAllWindows(deltax * EXTRA, deltay * EXTRA);
    ReTopAllWindows(&oldFrame, &frame);
    oldFrame = frame;
    for (i = 0; i < dockCount; i++)
    {
        switch (docks[i].ctl & POSITION_MASK)
        {
            case LEFT:
                if (docks[i].r.right > client.left * EXTRA)
                    client.left = docks[i].r.right / EXTRA;
                break;
            case RIGHT:
                if (docks[i].r.left < client.right * EXTRA)
                    client.right = docks[i].r.left / EXTRA;
                break;
            case TOP:
                if (docks[i].r.bottom > client.top * EXTRA)
                {
                    settop = TRUE;
                    client.top = docks[i].r.bottom / EXTRA;
                }
                break;
            case BOTTOM:
                if (docks[i].r.top < client.bottom * EXTRA)
                    client.bottom = docks[i].r.top / EXTRA;
                break;
        }
    }
    if (!IsWindowVisible(hwndSrcTab))
        ShowWindow(hwndSrcTab, SW_SHOW);
    if (!settop)
        client.top += toolBar.bottom - toolBar.top;
    client.top += 28;  // src tab height;
    if (PropGetBool(NULL, "BROWSE_INFORMATION") && !defaultWorkArea)
    {
        if (!IsWindowVisible(hwndJumpList))
            ShowWindow(hwndJumpList, SW_SHOW);
    }
    else
    {
        if (IsWindowVisible(hwndJumpList))
            ShowWindow(hwndJumpList, SW_HIDE);
    }
    if (IsWindowVisible(hwndJumpList))
    {
        client.top += jmpListHeight = SendMessage(hwndJumpList, WM_GETHEIGHT, 0, 0);
    }
    PutWindow(hwndClient, &client);
    if (IsWindowVisible(hwndJumpList))
    {
        client.bottom = client.top;
        client.top -= jmpListHeight;
        PutWindow(hwndJumpList, &client);
    }
    client.bottom = client.top;
    client.top -= 28;

    PutWindow(hwndSrcTab, &client);

    for (i = 0; i < dockCount; i++)
    {
        docks[i].display = docks[i].r;
        docks[i].display.left /= EXTRA;
        docks[i].display.right /= EXTRA;
        docks[i].display.top /= EXTRA;
        docks[i].display.bottom /= EXTRA;
    }
    PutFrames();
    for (i = 0; i < dockCount; i++)
    {
        PutWindow(docks[i].hwnd, &docks[i].display);
    }
    toolBar.right = frame.right;
    MoveWindow(hwndToolbarBar, toolBar.left, toolBar.top, toolBar.right - toolBar.left, toolBar.bottom - toolBar.top, 1);
}
static HWND WindowUnderCursor(HWND hwnd)
{
    RECT r;
    POINT p;
    int i;
    GetCursorPos(&p);
    MapWindowPoints(HWND_DESKTOP, hwndFrame, &p, 1);
    for (i = 0; i < dockCount; i++)
    {
        //        GetWindowRect(docks[i].hwnd, &r);
        if (PtInRect(&docks[i].display, p))
            return docks[i].hwnd;
    }
    GetWindowRect(hwndClient, &r);
    MapWindowPoints(HWND_DESKTOP, hwndFrame, &r, 2);
    if (PtInRect(&r, p))
        return hwndClient;
    return NULL;
}
#define TRANSPARENT_COLOR 0xffeedd
#define HIGHLIGHT_COLOR 0x777777
#define DLEFT 1
#define DRIGHT 2
#define DTOP 3
#define DBOTTOM 4
#define DCENTER 5
#define DCLIENT 0x100
static LRESULT CALLBACK DockRectWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC dc;
    HBRUSH brush;
    PAINTSTRUCT ps;
    RECT rect;
    switch (iMessage)
    {
        case WM_CREATE:
            SetWindowLong(hwnd, GWL_USERDATA, (long)((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xff, LWA_COLORKEY);
            break;
        case WM_PAINT:
            GetClientRect(hwnd, &rect);
            dc = BeginPaint(hwnd, &ps);
            brush = CreateSolidBrush(TRANSPARENT_COLOR);
            FillRect(dc, &rect, brush);
            DeleteObject(brush);
            if (GetWindowLong(hwnd, GWL_USERDATA) == DCENTER)
            {
                HBITMAP bmp = dockCenterBmp;
                RECT r;
                HDC dcMem = CreateCompatibleDC(dc);
                GetClientRect(hwnd, &r);
                SelectObject(dcMem, dockLeftBmp);
                BitBlt(dc, 2, 34, 32, 32, dcMem, 0, 0, SRCCOPY);
                SelectObject(dcMem, dockRightBmp);
                BitBlt(dc, 32 * 3 + 4 - 34, 34, 32, 32, dcMem, 0, 0, SRCCOPY);
                SelectObject(dcMem, dockTopBmp);
                BitBlt(dc, 34, 2, 32, 32, dcMem, 0, 0, SRCCOPY);
                SelectObject(dcMem, dockBottomBmp);
                BitBlt(dc, 34, 32 * 3 + 4 - 34, 32, 32, dcMem, 0, 0, SRCCOPY);
                SelectObject(dcMem, dockCenterBmp);
                BitBlt(dc, 34, 34, 32, 32, dcMem, 0, 0, SRCCOPY);
                DeleteDC(dcMem);
            }
            else
            {
                HBITMAP bmp;
                switch (GetWindowLong(hwnd, GWL_USERDATA))
                {
                    case DLEFT:
                        bmp = dockLeftBmp;
                        break;
                    case DRIGHT:
                        bmp = dockRightBmp;
                        break;
                    case DTOP:
                        bmp = dockTopBmp;
                        break;
                    case DBOTTOM:
                        bmp = dockBottomBmp;
                        break;
                }
                if (bmp)
                {
                    int n;
                    RECT r;
                    HDC dcMem = CreateCompatibleDC(dc);
                    GetClientRect(hwnd, &r);
                    SelectObject(dcMem, bmp);
                    n = (r.right - r.left - 32) / 2;
                    BitBlt(dc, n, n, 32, 32, dcMem, 0, 0, SRCCOPY);
                    DeleteDC(dcMem);
                }
            }
            EndPaint(hwnd, &ps);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
#define DOCKHIGHLIGHT_COLOR 0x663300

static LRESULT CALLBACK DockHighlightRectWndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HBRUSH brush;
    HDC dc;
    PAINTSTRUCT ps;
    RECT rect;
    switch (iMessage)
    {
        case WM_CREATE:
            SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLOR, 0xc0, LWA_ALPHA);
            break;
        case WM_PAINT:
            GetClientRect(hwnd, &rect);
            dc = BeginPaint(hwnd, &ps);
            brush = CreateSolidBrush(DOCKHIGHLIGHT_COLOR);
            FillRect(dc, &rect, brush);
            DeleteObject(brush);
            EndPaint(hwnd, &ps);
            break;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}
static int GetDockRectSelected(void)
{
    RECT r;
    POINT pt;
    GetCursorPos(&pt);
    GetWindowRect(left, &r);
    if (PtInRect(&r, pt))
        return DLEFT;
    GetWindowRect(right, &r);
    if (PtInRect(&r, pt))
        return DRIGHT;
    GetWindowRect(top, &r);
    if (PtInRect(&r, pt))
        return DTOP;
    GetWindowRect(bottom, &r);
    if (PtInRect(&r, pt))
        return DBOTTOM;
    GetWindowRect(center, &r);
    if (PtInRect(&r, pt))
    {
        int x = (pt.x - r.left - 2) / 32;
        int y = (pt.y - r.top - 2) / 32;
        int z = (y << 8) + x;
        if (x < 0)
            x = 0;
        if (x > 2)
            x = 2;
        if (y < 0)
            y = 0;
        if (y > 2)
            y = 2;
        switch (z)
        {
            case 1:
                return DCLIENT + DTOP;

            case 0x100:
                return DCLIENT + DLEFT;
            case 0x101:
                return DCLIENT + DCENTER;
            case 0x102:
                return DCLIENT + DRIGHT;
            case 0x201:
                return DCLIENT + DBOTTOM;
        }
    }

    return 0;
}
static void Highlight(int n, BOOL select)
{
    HWND hwnd;
    POINT pt = {2, 2};
    HDC dc;
    HBRUSH oldBrush;
    HPEN oldPen;
    switch (n)
    {
        case DCLIENT + DLEFT:
            pt.y += 32;
            hwnd = center;
            break;
        case DLEFT:
            hwnd = left;
            break;
        case DCLIENT + DRIGHT:
            pt.y += 32;
            pt.x += 32 + 32;
            hwnd = center;
            break;
        case DRIGHT:
            hwnd = right;
            break;
        case DCLIENT + DTOP:
            pt.x += 32;
            hwnd = center;
            break;
        case DTOP:
            hwnd = top;
            break;
        case DCLIENT + DBOTTOM:
            pt.y += 32 + 32 + 2;
            pt.x += 32;
            hwnd = center;
            break;
        case DBOTTOM:
            hwnd = bottom;
            break;
        case DCLIENT + DCENTER:
            pt.y += 32;
            pt.x += 32;
            hwnd = center;
            break;
        default:
            return;
    }
    dc = GetDC(hwnd);
    oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    oldPen = SelectObject(dc, CreatePen(PS_SOLID, 2, select ? HIGHLIGHT_COLOR : 0xffffff));
    Rectangle(dc, pt.x, pt.y, pt.x + 32, pt.y + 32);
    SelectObject(dc, oldBrush);
    DeleteObject(SelectObject(dc, oldPen));
    ReleaseDC(hwnd, dc);
}
static HWND CreateDockRect(int type)
{
    static BOOL registered;
    BOOL center = type == DCENTER;
    if (!registered)
    {
        WNDCLASS wc;
        registered = TRUE;
        memset(&wc, 0, sizeof(wc));
        wc.style = 0;
        wc.lpfnWndProc = DockRectWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = 0;
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        wc.lpszClassName = szDockRectClassName;
        RegisterClass(&wc);
        dockCenterBmp = LoadBitmap(hInstance, "ID_DOCKCBMP");
        dockLeftBmp = LoadBitmap(hInstance, "ID_DOCKLBMP");
        dockRightBmp = LoadBitmap(hInstance, "ID_DOCKRBMP");
        dockTopBmp = LoadBitmap(hInstance, "ID_DOCKTBMP");
        dockBottomBmp = LoadBitmap(hInstance, "ID_DOCKBBMP");
    }
    return CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, szDockRectClassName, "", WS_CLIPSIBLINGS | WS_POPUP, 0, 0,
                          center ? 32 * 3 + 4 : 36, center ? 32 * 3 + 4 : 36, hwndFrame, 0, hInstance, (void*)type);
}
static HWND CreateDockHighlightRect(void)
{
    static BOOL registered;
    static HWND hwnd;
    if (hwnd)
        return hwnd;
    if (!registered)
    {
        WNDCLASS wc;
        registered = TRUE;
        memset(&wc, 0, sizeof(wc));
        wc.style = 0;
        wc.lpfnWndProc = DockHighlightRectWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = 0;
        wc.hbrBackground = 0;
        wc.lpszMenuName = 0;
        wc.lpszClassName = szDockHighlightRectClassName;
        RegisterClass(&wc);
    }
    return hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, szDockHighlightRectClassName, "", WS_CLIPSIBLINGS | WS_POPUP, 0,
                                 0, 10, 10, hwndFrame, 0, hInstance, NULL);
}
void drawSideBars(HWND hwnd, BOOL show)
{
    if (show)
    {
        RECT r, r1, r2;
        GetWindowRect(hwndFrame, &r);
        GetWindowRect(hwndToolbarBar, &r1);
        GetWindowRect(hwndStatus, &r2);
        r.top += r1.bottom - r1.top;
        r.top += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYFRAME);
        r.bottom -= GetSystemMetrics(SM_CYFRAME);
        r.bottom -= r2.bottom - r2.top;
        r.left += GetSystemMetrics(SM_CXFRAME);
        r.right -= GetSystemMetrics(SM_CYFRAME);
        if (!left)
        {
            left = CreateDockRect(DLEFT);
            right = CreateDockRect(DRIGHT);
            top = CreateDockRect(DTOP);
            bottom = CreateDockRect(DBOTTOM);
        }
        SetWindowPos(left, HWND_TOPMOST, 5 + r.left, (r.bottom + r.top) / 2 - 18, 36, 36, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        SetWindowPos(right, HWND_TOPMOST, r.right - 5 - 36, (r.bottom + r.top) / 2 - 18, 36, 36, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        SetWindowPos(top, HWND_TOPMOST, (r.left + r.right) / 2 - 18, r.top + 5, 36, 36, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        SetWindowPos(bottom, HWND_TOPMOST, (r.left + r.right) / 2 - 18, (r.bottom - 5 - 36), 36, 36,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    else
    {
        DestroyWindow(left);
        DestroyWindow(right);
        DestroyWindow(top);
        DestroyWindow(bottom);
        left = right = top = bottom = NULL;
    }
}
void drawCtl(HWND hwnd, BOOL show)
{
    if (show)
    {
        RECT r;
        GetWindowRect(hwnd, &r);
        if (!center)
            center = CreateDockRect(DCENTER);
        SetWindowPos(center, HWND_TOPMOST, (r.right + r.left) / 2 - (32 * 3 + 4) / 2, (r.bottom + r.top) / 2 - (32 * 3 + 4) / 2,
                     32 * 3 + 4, 32 * 3 + 4, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    else
    {
        DestroyWindow(center);
        center = NULL;
    }
}
void DockRemoveWindow(HWND hwnd)
{
    int i;
    DestroyWindow(hwnd);
    for (i = 0; i < freeWindowCount; i++)
    {
        if (freeWindows[i] == hwnd)
        {
            memcpy(freeWindows + i, freeWindows + i + 1, (freeWindowCount - i - 1) * sizeof(freeWindows[i]));
            freeWindowCount--;
            return;
        }
    }
    for (i = 0; i < dockCount; i++)
    {
        if (docks[i].hwnd == hwnd)
        {
            ExtendSurroundingWindows(docks + i, hwnd);
            memcpy(docks + i, docks + i + 1, (dockCount - i - 1) * sizeof(docks[i]));
            dockCount--;
            ResizeLayout(NULL);
            return;
        }
    }
}
static void GetNewDockRect(HWND hwnd, RECT* r, int selected)
{
    int ctl = 0;
    int i;
    //    int i = 0;
    RECT frame;
    RECT toolBar;
    RECT statusBar;
    RECT client;
    int n;
    for (i = 0; i < dockCount; i++)
        if (docks[i].hwnd == hwnd)
        {
            ctl = docks[i].ctl;
            break;
        }
    GetWindowRect(hwnd, r);
    MapWindowPoints(HWND_DESKTOP, hwndFrame, (LPPOINT)r, 2);
    GetClientRect(hwndFrame, &frame);
    GetClientRect(hwndStatus, &statusBar);
    GetClientRect(hwndToolbarBar, &toolBar);
    GetWindowRect(hwndClient, &client);
    MapWindowPoints(HWND_DESKTOP, hwndFrame, (LPPOINT)&client, 2);
    frame.top += toolBar.bottom - toolBar.top;
    frame.bottom -= statusBar.bottom - statusBar.top;
    if (hwnd == hwndClient)
    {
        int n = 28;  //- FRAMEWIDTH;
        if (IsWindowVisible(hwndJumpList))
        {
            n += SendMessage(hwndJumpList, WM_GETHEIGHT, 0, 0);
        }
        r->top -= n;
    }
    else
    {
        switch (docks[i].ctl & POSITION_MASK)
        {
            case LEFT:
                r->right += FRAMEWIDTH;
                if (i && (docks[i - 1].ctl & POSITION_MASK) == LEFT && docks[i].r.left == docks[i - 1].r.left)
                {
                    r->top -= FRAMEWIDTH;
                }
                break;
            case RIGHT:
                r->left -= FRAMEWIDTH;
                if (i && (docks[i - 1].ctl & POSITION_MASK) == RIGHT && docks[i].r.left == docks[i - 1].r.left)
                {
                    r->top -= FRAMEWIDTH;
                }
                break;
            case TOP:
                r->bottom += FRAMEWIDTH;
                if (i && (docks[i - 1].ctl & POSITION_MASK) == TOP && docks[i].r.top == docks[i - 1].r.top)
                {
                    r->left -= FRAMEWIDTH;
                }
                break;
            case BOTTOM:
                r->top -= FRAMEWIDTH;
                if (i && (docks[i - 1].ctl & POSITION_MASK) == BOTTOM && docks[i].r.top == docks[i - 1].r.top)
                {
                    r->left -= FRAMEWIDTH;
                }
                break;
        }
    }
    switch (selected)
    {
        case DCLIENT | DCENTER:
            break;
        case DCLIENT | DLEFT:
            if (hwnd == hwndClient)
            {
                r->right = r->left + DEFAULT_WINDOW_WIDTH;
                return;
            }
            else if ((ctl & POSITION_MASK) == LEFT || (ctl & POSITION_MASK) == RIGHT)
            {
                GetSpanHeight(docks + i, r);
                if ((ctl & POSITION_MASK) == RIGHT)
                {
                    n = r->right - r->left;
                    r->left -= n;
                    r->right -= n;
                }
            }
            else
            {
                r->right = r->left + DEFAULT_WINDOW_WIDTH;
                return;
            }
            break;
        case DCLIENT | DRIGHT:
            if (hwnd == hwndClient)
            {
                r->left = r->right - DEFAULT_WINDOW_WIDTH;
                return;
            }
            else if ((ctl & POSITION_MASK) == LEFT || (ctl & POSITION_MASK) == RIGHT)
            {
                GetSpanHeight(docks + i, r);
                if ((ctl & POSITION_MASK) == LEFT)
                {
                    n = r->right - r->left;
                    r->left += n;
                    r->right += n;
                }
            }
            else
            {
                r->left = r->right - DEFAULT_WINDOW_WIDTH;
                return;
            }
            break;
        case DCLIENT | DTOP:
            if (hwnd == hwndClient)
            {
                r->bottom = r->top + DEFAULT_WINDOW_HEIGHT;
                return;
            }
            else if ((ctl & POSITION_MASK) == TOP || (ctl & POSITION_MASK) == BOTTOM)
            {
                GetSpanWidth(docks + i, r);

                if ((ctl & POSITION_MASK) == BOTTOM)
                {
                    n = r->bottom - r->top;
                    r->top -= n;
                    r->bottom -= n;
                }
            }
            else
            {
                r->bottom = r->top + DEFAULT_WINDOW_HEIGHT;
                return;
            }
            break;
            break;
        case DCLIENT | DBOTTOM:
            if (hwnd == hwndClient)
            {
                r->top = r->bottom - DEFAULT_WINDOW_HEIGHT;
                return;
            }
            else if ((ctl & POSITION_MASK) == TOP || (ctl & POSITION_MASK) == BOTTOM)
            {
                GetSpanWidth(docks + i, r);

                if ((ctl & POSITION_MASK) == TOP)
                {
                    n = r->bottom - r->top;
                    r->top += n;
                    r->bottom += n;
                }
            }
            else
            {
                r->top = r->bottom - DEFAULT_WINDOW_HEIGHT;
                return;
            }
            break;
        case DLEFT:
            r->top = frame.top;
            r->bottom = frame.bottom;
            r->left = frame.left;
            r->right = r->left + DEFAULT_WINDOW_WIDTH;
            for (i = 0; i < dockCount; i++)
                if ((docks[i].ctl & POSITION_MASK == LEFT) && docks[i].display.left == r->left)
                {
                    r->left = docks[i].display.left;
                    r->right = docks[i].display.right;
                    break;
                }
            break;
        case DRIGHT:
            r->top = frame.top;
            r->bottom = frame.bottom;
            r->right = frame.right;
            r->left = r->right - DEFAULT_WINDOW_WIDTH;
            for (i = 0; i < dockCount; i++)
                if ((docks[i].ctl & POSITION_MASK == RIGHT) && docks[i].display.right == r->right)
                {
                    r->left = docks[i].display.left;
                    r->right = docks[i].display.right;
                    break;
                }
            break;
        case DTOP:
            r->left = frame.left;
            r->right = frame.right;
            r->top = frame.top;
            r->bottom = r->top + DEFAULT_WINDOW_HEIGHT;
            for (i = 0; i < dockCount; i++)
                if ((docks[i].ctl & POSITION_MASK == TOP) && docks[i].display.top == r->top)
                {
                    r->top = docks[i].display.top;
                    r->bottom = docks[i].display.bottom;
                    break;
                }
            break;
        case DBOTTOM:
            r->left = frame.left;
            r->right = frame.right;
            r->bottom = frame.bottom;
            r->top = r->bottom - DEFAULT_WINDOW_HEIGHT;
            for (i = 0; i < dockCount; i++)
                if ((docks[i].ctl & POSITION_MASK == BOTTOM) && docks[i].display.bottom == r->bottom)
                {
                    r->top = docks[i].display.top;
                    r->bottom = docks[i].display.bottom;
                    break;
                }
            break;
    }
}
static void ReassignTopBottomLeft(int i, int w)
{
    int left, width;
    int oldleft, oldwidth;
    int start = i;
    int end = i + 1;
    int last;
    while (end < dockCount && (docks[i].ctl & POSITION_MASK) == (docks[end].ctl & POSITION_MASK) &&
           docks[end].r.left == docks[end - 1].r.right)
        end++;
    oldleft = docks[start].r.left;
    left = oldleft + w;
    oldwidth = docks[end - 1].r.right - oldleft;
    width = docks[end - 1].r.right - left;
    last = left;
    while (start < end)
    {
        docks[start].r.right = (int)(((__int64)docks[start].r.right - oldleft) * width / oldwidth + left);
        docks[start].r.left = last;
        last = docks[start].r.right;
        start++;
    }
}
static void ConsiderNewLeft(int last, RECT* r, int w)
{
    int j;
    for (j = 0; j < dockCount; j++)
    {
        if (docks[j].r.left == r->left)
            if (InContact(r->top, r->bottom, docks[j].r.top, docks[j].r.bottom))
            {
                switch (docks[j].ctl & POSITION_MASK)
                {
                    case TOP:
                    case BOTTOM:
                        ReassignTopBottomLeft(j, w);
                        break;
                    case LEFT:
                        if (j != last)
                        {
                            RECT r = docks[j].r;
                            r.left = r.right;
                            ConsiderNewLeft(j, &r, w);
                            GetSpanHeight(&docks[j], &docks[j].r);
                            docks[j].r.left += w;
                            docks[j].r.right += w;
                            docks[j].r.top *= EXTRA;
                            docks[j].r.bottom *= EXTRA;
                        }
                        break;
                }
            }
            else if (r->top == docks[j].r.top)
            {
                docks[j].r.top = r->bottom;
            }
            else if (r->bottom == docks[j].r.bottom)
            {
                docks[j].r.bottom = r->top;
            }
    }
}
static void ReassignTopBottomRight(int i, int w)
{
    int right, width;
    int oldright, oldwidth;
    int start = i + 1;
    int end = i;
    int last;
    while (end > 0 && (docks[i].ctl & POSITION_MASK) == (docks[end - 1].ctl & POSITION_MASK) &&
           docks[end - 1].r.right == docks[end].r.left)
        end--;
    oldright = docks[start - 1].r.right;
    right = oldright - w;
    oldwidth = oldright - docks[end].r.left;
    width = right - docks[end].r.left;
    last = right;
    while (start-- > end)
    {
        docks[start].r.left = (right - ((__int64)oldright - docks[start].r.left) * width / oldwidth);
        docks[start].r.right = last;
        last = docks[start].r.left;
    }
}
static void ConsiderNewRight(int last, RECT* r, int w)
{
    int j;
    for (j = 0; j < dockCount; j++)
    {
        if (docks[j].r.right == r->right)
            if (InContact(r->top, r->bottom, docks[j].r.top, docks[j].r.bottom))
            {
                switch (docks[j].ctl & POSITION_MASK)
                {
                    case TOP:
                    case BOTTOM:
                        ReassignTopBottomRight(j, w);
                        break;
                    case RIGHT:
                        if (j != last)
                        {
                            RECT r = docks[j].r;
                            r.right = r.left;
                            ConsiderNewRight(j, &r, w);
                            GetSpanHeight(&docks[j], &docks[j].r);
                            docks[j].r.left -= w;
                            docks[j].r.right -= w;
                            docks[j].r.top *= EXTRA;
                            docks[j].r.bottom *= EXTRA;
                        }
                        break;
                }
            }
            else if (r->top == docks[j].r.top)
            {
                docks[j].r.top = r->bottom;
            }
            else if (r->bottom == docks[j].r.bottom)
            {
                docks[j].r.bottom = r->top;
            }
    }
}
static void ReassignLeftRightTop(int i, int w)
{
    int top, height;
    int oldtop, oldheight;
    int start = i;
    int end = i + 1;
    int last;
    while (end < dockCount && (docks[i].ctl & POSITION_MASK) == (docks[end].ctl & POSITION_MASK) &&
           docks[end].r.top == docks[end - 1].r.bottom)
        end++;
    oldtop = docks[start].r.top;
    top = oldtop + w;
    oldheight = docks[end - 1].r.bottom - oldtop;
    height = docks[end - 1].r.bottom - top;
    last = top;
    while (start < end)
    {
        docks[start].r.bottom = (int)(((__int64)docks[start].r.bottom - oldtop) * height / oldheight + top);
        docks[start].r.top = last;
        last = docks[start].r.bottom;
        start++;
    }
}
static void ConsiderNewTop(int last, RECT* r, int h)
{
    int j;
    for (j = 0; j < dockCount; j++)
    {
        if (docks[j].r.top == r->top)
            if (InContact(r->left, r->right, docks[j].r.left, docks[j].r.right))
            {
                switch (docks[j].ctl & POSITION_MASK)
                {
                    case TOP:
                        if (j != last)
                        {
                            RECT r = docks[j].r;
                            r.top = r.bottom;
                            ConsiderNewTop(j, &r, h);
                            GetSpanWidth(&docks[j], &docks[j].r);
                            docks[j].r.top += h;
                            docks[j].r.bottom += h;
                            docks[j].r.left *= EXTRA;
                            docks[j].r.right *= EXTRA;
                        }
                        break;
                    case LEFT:
                    case RIGHT:
                        ReassignLeftRightTop(j, h);
                        break;
                }
            }
            else if (r->left == docks[j].r.left)
            {
                docks[j].r.left = r->right;
            }
            else if (r->right == docks[j].r.right)
            {
                docks[j].r.right = r->left;
            }
    }
}
static void ReassignLeftRightBottom(int i, int w)
{
    int bottom, height;
    int oldbottom, oldheight;
    int start = i + 1;
    int end = i;
    int last;
    while (end > 0 && (docks[i].ctl & POSITION_MASK) == (docks[end - 1].ctl & POSITION_MASK) &&
           docks[end - 1].r.bottom == docks[end].r.top)
        end--;
    oldbottom = docks[start - 1].r.bottom;
    bottom = oldbottom - w;
    oldheight = oldbottom - docks[end].r.top;
    height = bottom - docks[end].r.top;
    last = bottom;
    while (start-- > end)
    {
        docks[start].r.top = (bottom - ((__int64)oldbottom - docks[start].r.top) * height / oldheight);
        docks[start].r.bottom = last;
        last = docks[start].r.top;
    }
}
static void ConsiderNewBottom(int last, RECT* r, int h)
{
    int j;
    for (j = 0; j < dockCount; j++)
    {
        if (docks[j].r.bottom == r->bottom)
            if (InContact(r->left, r->right, docks[j].r.left, docks[j].r.right))
            {
                switch (docks[j].ctl & POSITION_MASK)
                {
                    case BOTTOM:
                        if (j != last)
                        {
                            RECT r = docks[j].r;
                            r.bottom = r.top;
                            ConsiderNewBottom(j, &r, h);
                            GetSpanWidth(&docks[j], &docks[j].r);
                            docks[j].r.top -= h;
                            docks[j].r.bottom -= h;
                            docks[j].r.left *= EXTRA;
                            docks[j].r.right *= EXTRA;
                        }
                        break;
                    case LEFT:
                    case RIGHT:
                        ReassignLeftRightBottom(j, h);
                        break;
                }
            }
            else if (r->left == docks[j].r.left)
            {
                docks[j].r.left = r->right;
            }
            else if (r->right == docks[j].r.right)
            {
                docks[j].r.right = r->left;
            }
    }
}
static BOOL InsertDock(HWND hwnd, int i, RECT* r, int target)
{
    BOOL rv = TRUE;
    int j;
    long style;
    RECT frame;
    RECT toolBar;
    RECT statusBar;
    GetClientRect(hwndFrame, &frame);
    GetClientRect(hwndStatus, &statusBar);
    GetClientRect(hwndToolbarBar, &toolBar);
    frame.top += toolBar.bottom - toolBar.top;
    frame.bottom -= statusBar.bottom - statusBar.top;
    frame.left *= EXTRA;
    frame.right *= EXTRA;
    frame.top *= EXTRA;
    frame.bottom *= EXTRA;
    for (j = 0; j < freeWindowCount; j++)
        if (freeWindows[j] == hwnd)
        {
            memcpy(freeWindows + j, freeWindows + j + 1, (freeWindowCount - j - 1) * sizeof(HWND));
            freeWindowCount--;
        }
    memmove(docks + i + 1, docks + i, (dockCount - i) * sizeof(DOCK_STR));
    dockCount++;
    docks[i].hwnd = hwnd;
    docks[i].ctl = target;
    docks[i].r.left = -1;
    docks[i].r.right = -1;
    docks[i].r.top = -1;
    docks[i].r.bottom = -1;
    for (j = 0; j < dockCount; j++)
        docks[j].hr = docks[j].r;
    style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLong(hwnd, GWL_STYLE, (style | WS_CHILD) & ~(WS_POPUP | WS_SIZEBOX));
    //    SetParent(hwnd, hwndFrame);

    switch (target & POSITION_MASK)
    {
        case LEFT:
            ConsiderNewLeft(i, r, r->right - r->left);
            break;
        case RIGHT:
            ConsiderNewRight(i, r, r->right - r->left);
            break;
        case TOP:
            ConsiderNewTop(i, r, r->bottom - r->top);
            break;
        case BOTTOM:
            ConsiderNewBottom(i, r, r->bottom - r->top);
            break;
    }
    docks[i].r = *r;
    for (j = 0; j < dockCount; j++)
    {
        if (docks[j].r.right - docks[j].r.left < 100 * EXTRA)
            rv = FALSE;
        if (docks[j].r.bottom - docks[j].r.top < 100 * EXTRA)
            rv = FALSE;
    }
    if (!rv)
    {
        for (j = 0; j < dockCount; j++)
            docks[j].r = docks[j].hr;
        memcpy(docks + i, docks + i + 1, (dockCount - i - 1) * sizeof(docks[i]));
        dockCount--;
    }
    return rv;
}
static BOOL PositionNewDockRect(HWND hwnd, HWND src, RECT* r, int selected)
{
    BOOL rv = TRUE;
    RECT client;
    GetClientRect(hwndClient, &client);
    client.right *= EXTRA;
    client.bottom *= EXTRA;
    if (hwnd == hwndClient && (selected & DCLIENT))
    {
        switch (selected)
        {
            int i;
            case DCLIENT | DCENTER:
                SetWindowPos(src, HWND_TOP, r->left / EXTRA, r->top / EXTRA, (r->right - r->left) / EXTRA,
                             (r->bottom - r->top) / EXTRA, SWP_NOACTIVATE);
                break;
            case DCLIENT | DLEFT:
                if (client.right < r->right - r->left + 100 * EXTRA)
                    return FALSE;
                for (i = 0; i < dockCount; i++)
                {
                    if ((docks[i].ctl & POSITION_MASK) != LEFT)
                        break;
                }
                rv = InsertDock(src, i, r, LEFT);
                break;
            case DCLIENT | DRIGHT:
                if (client.right < r->right - r->left + 100 * EXTRA)
                    return FALSE;
                for (i = dockCount; i > 0; i--)
                {
                    if ((docks[i - 1].ctl & POSITION_MASK) != BOTTOM && (docks[i].ctl & POSITION_MASK) != TOP)
                    {
                        break;
                    }
                }
                rv = InsertDock(src, i, r, RIGHT);
                break;
            case DCLIENT | DTOP:
                if (client.bottom < r->top - r->bottom + 100 * EXTRA)
                    return FALSE;
                for (i = dockCount; i < dockCount; i++)
                {
                    if ((docks[i - 1].ctl & POSITION_MASK) == BOTTOM)
                        break;
                }
                rv = InsertDock(src, i, r, TOP);
                break;
            case DCLIENT | DBOTTOM:
                if (client.bottom < r->top - r->bottom + 100 * EXTRA)
                    return FALSE;
                for (i = dockCount; i > 0; i--)
                {
                    if ((docks[i - 1].ctl & POSITION_MASK) != BOTTOM)
                        break;
                }
                rv = InsertDock(src, i, r, BOTTOM);
                break;
            default:
                return FALSE;
        }
    }
    else
    {
        int i;
        if (selected & DCLIENT)
        {
            for (i = 0; i < dockCount; i++)
                if (docks[i].hwnd == hwnd)
                {
                    break;
                }
            switch (selected)
            {
                case DCLIENT | DLEFT:
                    rv = InsertDock(src, i == dockCount ? 0 : i, r, docks[i].ctl);
                    break;
                case DCLIENT | DTOP:
                    rv = InsertDock(src, i == dockCount ? 0 : i, r, docks[i].ctl);
                    break;
                case DCLIENT | DRIGHT:
                    rv = InsertDock(src, i == dockCount ? i : i + 1, r, docks[i].ctl);
                    break;
                case DCLIENT | DBOTTOM:
                    rv = InsertDock(src, i == dockCount ? i : i + 1, r, docks[i].ctl);
                    break;
            }
        }
        else
            switch (selected)
            {
                case DLEFT:
                    rv = InsertDock(src, 0, r, LEFT);
                    break;
                case DRIGHT:
                    for (i = 0; i < dockCount; i++)
                    {
                        if ((docks[i].ctl & POSITION_MASK) != LEFT && (docks[i].ctl & POSITION_MASK) != RIGHT)
                            break;
                    }
                    rv = InsertDock(src, i, r, RIGHT);
                    break;
                case DTOP:
                    for (i = 0; i < dockCount; i++)
                    {
                        if ((docks[i].ctl & POSITION_MASK) != LEFT && (docks[i].ctl & POSITION_MASK) != RIGHT)
                            break;
                    }
                    rv = InsertDock(src, i, r, TOP);
                    break;
                case DBOTTOM:
                    rv = InsertDock(src, dockCount, r, BOTTOM);
                    break;
                default:
                    return FALSE;
            }
    }
    return rv;
}
void DockEndDrag(HWND hwnd, int ox, int oy, int x, int y)
{
    if (lastWindowUnderCursor)
    {
        HWND target = WindowUnderCursor(hwnd);
        BOOL free = FALSE;
        HWND hwndHighlight = CreateDockHighlightRect();
        int n = GetDockRectSelected();
        ShowWindow(hwndHighlight, SW_HIDE);
        if (!target || target == hwndClient && (n & DCLIENT))
        {
            RECT r;
            POINT pt;
            int i;
            GetCursorPos(&pt);
            for (i = 0; i < freeWindowCount; i++)
            {
                if (freeWindows[i] != hwnd)
                {
                    GetWindowRect(freeWindows[i], &r);
                    if (PtInRect(&r, pt))
                    {
                        target = freeWindows[i];
                        free = TRUE;
                        break;
                    }
                }
            }
        }
        if (n == 0 && free || n == (DCLIENT | DCENTER) && target != hwndClient)
        {
            SendMessage(hwnd, TW_TRANSFERCHILDREN, 0, (LPARAM)target);
        }
        else if (n)
        {
            RECT r;
            GetNewDockRect(lastWindowUnderCursor, &r, n);
            r.left *= EXTRA;
            r.right *= EXTRA;
            r.top *= EXTRA;
            r.bottom *= EXTRA;
            if (!PositionNewDockRect(target, hwnd, &r, n))
                SetWindowPos(hwnd, HWND_TOP, dragPos.left + pt2.x - pt.x, dragPos.top + pt2.y - pt.y, dragPos.right - dragPos.left,
                             dragPos.bottom - dragPos.top, SWP_NOACTIVATE);
            else
                ResizeLayout(0);
        }
        else
        {
            SetWindowPos(hwnd, HWND_TOP, dragPos.left + pt2.x - pt.x, dragPos.top + pt2.y - pt.y, dragPos.right - dragPos.left,
                         dragPos.bottom - dragPos.top, SWP_NOACTIVATE);
        }
        drawSideBars(lastWindowUnderCursor, FALSE);
        drawCtl(lastWindowUnderCursor, FALSE);
    }
    lastWindowUnderCursor = NULL;
    moving = FALSE;
}
BOOL DockDrag(HWND hwnd, int ox, int oy, int x, int y)
{
    static int selected;
    static POINT pt3, pt4;
    RECT r;
    HWND hwndParent;
    if (!moving)
    {
        selected = 0;
        GetCursorPos(&pt);
        GetWindowRect(hwnd, &dragPos);
        moving = TRUE;
        pt3 = pt;
    }
    else
    {
        GetCursorPos(&pt4);
        if (pt4.x == pt3.x && pt4.y == pt3.y)
            return FALSE;
        pt3 = pt4;
    }
    hwndParent = WindowUnderCursor(hwnd);
    if (hwndParent && hwndParent != lastWindowUnderCursor)
    {
        int i;
        if (SendMessage(hwnd, TW_ISCONT, 0, 0) == TW_ISCONT)
        {
            GetWindowRect(hwnd, &r);
            if (!(((int)GetWindowLong(hwnd, GWL_STYLE)) & WS_POPUP))
            {
                MapWindowPoints(HWND_DESKTOP, hwndFrame, (LPPOINT)&r, 2);
                for (i = 0; i < dockCount; i++)
                {
                    if (docks[i].display.left == r.left && docks[i].display.top == r.top)
                    {
                        break;
                    }
                }
                if (i < dockCount)
                {
                    ExtendSurroundingWindows(docks + i, hwnd);
                    memcpy(docks + i, docks + i + 1, (dockCount - i - 1) * sizeof(docks[i]));
                    dockCount--;
                    freeWindows[freeWindowCount++] = hwnd;
                    SendMessage(hwnd, TW_MAKEPOPUP, 1, 0);
                    //                    SetParent(hwnd, HWND_DESKTOP);
                    if (dragPos.left < pt.x - DEFAULT_WINDOW_WIDTH || dragPos.left > pt.x)
                        dragPos.left = pt.x - 125;
                    if (dragPos.top < pt.y - SM_CYSMCAPTION || dragPos.top > pt.y)
                        dragPos.top = pt.y - SM_CYSMCAPTION / 2;
                    dragPos.right = dragPos.left + DEFAULT_WINDOW_WIDTH;
                    dragPos.bottom = dragPos.top + 400;
                    ResizeLayout(NULL);
                }
                else
                {
                    freeWindows[freeWindowCount++] = hwnd;
                    SendMessage(hwnd, TW_MAKEPOPUP, 1, 0);
                    //                    SetParent(hwnd, HWND_DESKTOP);
                }
            }
        }
        lastWindowUnderCursor = hwndParent;
        drawSideBars(lastWindowUnderCursor, TRUE);
        drawCtl(lastWindowUnderCursor, TRUE);
    }
    GetCursorPos(&pt2);
    MapWindowPoints(HWND_DESKTOP, hwndFrame, &pt2, 1);
    if (pt2.y >= 0)
    {
        GetClientRect(hwndFrame, &r);
        if (pt2.y < r.bottom - GetSystemMetrics(SM_CYCAPTION))
        {
            SetWindowPos(hwnd, HWND_TOP, dragPos.left + pt2.x - pt.x, dragPos.top + pt2.y - pt.y, dragPos.right - dragPos.left,
                         dragPos.bottom - dragPos.top, SWP_NOACTIVATE);
            if (lastWindowUnderCursor)
            {
                int n = GetDockRectSelected();
                if (n != selected)
                {
                    Highlight(selected, FALSE);
                    selected = n;
                    Highlight(selected, TRUE);
                    if (n)
                    {
                        RECT r;
                        HWND hwnd = CreateDockHighlightRect();
                        GetNewDockRect(lastWindowUnderCursor, &r, n);
                        MapWindowPoints(hwndFrame, HWND_DESKTOP, (LPPOINT)&r, 2);
                        SetWindowPos(hwnd, HWND_TOP, r.left, r.top, r.right - r.left, r.bottom - r.top,
                                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
                    }
                    else
                    {
                        HWND hwnd = CreateDockHighlightRect();
                        ShowWindow(hwnd, SW_HIDE);
                    }
                }
            }
        }
    }
    return FALSE;
}
HWND CreateInternalWindow(int DID, char* clsName, char* title)
{
    return CreateWindow(clsName, title, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, hwndFrame, 0, hInstance, 0);
}
static void SetupInitialDocks(int flag)
{
    int i;
    for (i = 0; i < sizeof(windowDeclarations) / sizeof(windowDeclarations[0]); i++)
    {
        if (windowDeclarations[i].defaultDock & DOCKABLE_INITIAL)
        {
            if (windowDeclarations[i].defaultDock & flag)
            {
                SelectWindowInternal(windowDeclarations[i].CTL_ID, FALSE);
            }
        }
    }
}
void FreeDockWindows()
{
    int i;
    for (i = 0; i < dockCount; i++)
    {
        CloseWindow(docks[i].hwnd);
        DestroyWindow(docks[i].hwnd);
    }
    dockCount = 0;
}
void CreateDocks(void)
{
    RECT statusBar;
    RECT toolBar;
    if (!debugDockCount && !releaseDockCount && !debugfreeWindowCount && !releasefreeWindowCount)
    {
        GetClientRect(hwndFrame, &oldFrame);
        GetClientRect(hwndStatus, &statusBar);
        GetClientRect(hwndToolbarBar, &toolBar);
        oldFrame.bottom -= statusBar.bottom - statusBar.top;
        oldFrame.top += toolBar.bottom - toolBar.top;

        Select(DOCKABLE_DEBUG);
        SetupInitialDocks(DOCKABLE_DEBUG);
        Select(DOCKABLE_EDIT);
        SetupInitialDocks(DOCKABLE_EDIT);
    }
    Select(0);
}