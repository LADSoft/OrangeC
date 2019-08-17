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

#ifndef CTL_H
#define CTL_H

#define COLOR_MENUHIGHLIGHT 29
#define COLOR_MENUBAR 30

#define LCF_ADJUSTRECT WM_USER + 1024
#define LCF_SETFLEX WM_USER + 1025
#define LCF_SETVERTICAL WM_USER + 1026
#define LCF_CONTAINER WM_USER + 1027
#define LCF_SETACTIVE WM_USER + 1028
#define LCF_ADDCONTROL WM_USER + 1029
#define LCF_FLOATINGTOOL WM_USER + 1030

#define TCF_SETHEADER (TV_FIRST + 120)
#define TCF_SETDIVIDER (TV_FIRST + 121)

#define TCN_PAINT (TV_FIRST + 150)
#define TCN_EDITDONE (TV_FIRST + 151)
#define TCN_EDITQUERY (TV_FIRST + 152)

#define TCS_LINE 0x1000
#undef TV_FIRST
#undef HDM_FIRST
#define TV_FIRST 0x1100
#define HDM_FIRST 0x1200

#define TW_SELECT 10000
#define TW_ADD 10001
#define TW_REMOVE 10002
#define TW_GETACTIVE 10003
#define TW_SETACTIVE 10004
#define TW_HAS 10005
#define TW_CLOSE 10006
#define TW_ISCONT 10007
#define TW_CAPTURE 10008
#define TW_TRANSFERCHILDREN 10009
#define TW_MAKEPOPUP 10010

typedef struct
{
    HWND hwnd;
    RECT pos;
    HWND* children;
    int childCount;
    int activeIndex;
    HWND tabCtl;
} TWData;

#define GRIPWIDTH 18
#define TITLEWIDTH 15
#define RIGHTWIDTH 5
#define EDGEWIDTH 1
#define TITLETOP 5
#define BARLEFT 3
#define BARRIGHT 3
#define BARTOP 0
#define BUTTONCORNER 1
#define BUTTONWIDTH 12

typedef struct
{
    DWORD id;
    HWND hWnd;
    HWND blank;
    HWND grip;
    RECT pos;
    char** hints;
    WCHAR** whints;
    TBBUTTON* buttons;
    int btncount;
    int bmpid;
    int bmpcount;
    HWND* children;
    DWORD helpItem;
    HWND notifyParent;
    HIMAGELIST imageList, disabledImageList;
    int nonfloating : 1;
} TBData;

struct _stList
{
    struct _singleTab* head;
    struct _singleTab* tail;
};
struct _stItem
{
    struct _singleTab* prev;
    struct _singleTab* next;
};
struct _singleTab
{
    struct _stItem chain;
    struct _stItem selectedChain;
    char* text;
    LPARAM lParam;
    BOOL displayed;
    BOOL modified;
    RECT displayRect;
    RECT closeBtn;
    int iImage;
};
struct _tabStruct
{
    struct _stList active;
    struct _stList selected;
    struct _singleTab* highlighted;
    int fontHeight;
    HFONT normalFont, boldFont;
    RECT windowBtn;
    HPEN greyPen;
    HPEN whitePen;
    HWND toolTip;
    HBITMAP xBitmap, xBitmapSelected;
    HBITMAP xBack;
    HBITMAP menuBitmap;
    HBITMAP menuBitmap2;
    RECT xRect;
    BOOL captured;
    BOOL flat;
    BOOL displayMenu;
    DWORD windowBtnMode;
    BOOL dragging;
    HCURSOR oldCursor;
    HIMAGELIST hImageList;
    struct _singleTab* dragSrc;
    struct _singleTab** menuList;
};

struct ttrack
{
    HANDLE tWait;
    HWND hwnd;
    HMENU menu;
    int rows;
    int cols;
    int rowHeight;
    int rowWidth;
    BOOL inWindow;
    int id;
    int oldTrack;
};
typedef struct
{
    char* colText1;
    char* colText2;
    HBITMAP colBmp1;
    HBITMAP colBmp2;
} TCHeader;

typedef struct
{
    char* col1Text;
    char* col2Text;
    DWORD col1Color;
    DWORD col2Color;
} TCData;

#define TABM_ADD WM_USER
#define TABM_REMOVE (WM_USER + 1)
#define TABM_REMOVEALL (WM_USER + 2)
#define TABM_SELECT (WM_USER + 3)
#define TABM_GETCURSEL (WM_USER + 4)
#define TABM_RENAME (WM_USER + 5)
#define TABM_SETMODIFY (WM_USER + 6)
#define TABM_GETITEM (WM_USER + 7)
#define TABM_INSERT (WM_USER + 8)
#define TABM_SETIMAGELIST (WM_USER + 9)
#define TABM_SETIMAGE (WM_USER + 10)

#define TABN_SELECTED (-1000)
#define TABN_CLOSED (-1001)
#define TABN_STARTDRAG (-1002)

#define TABS_BOTTOM 1
#define TABS_CLOSEBTN 2
#define TABS_WINDOWBTN 4
#define TABS_HOTTRACK 8
#define TABS_HINT 16
#define TABS_FLAT 32
#define TABS_DRAGABLE 64

typedef struct LsTabNotify
{
    NMHDR hdr;
    char* text;
    LPARAM lParam;
} LSTABNOTIFY;

#endif