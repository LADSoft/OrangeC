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
    HWND *children;
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

typedef struct {
    DWORD id;
    HWND hWnd;
    HWND blank;
    HWND grip;
    RECT pos;
    char **hints;
    WCHAR **whints;
    TBBUTTON *buttons;
    int btncount;
    int bmpid;
    int bmpcount;
    HWND *children;
    DWORD helpItem;
    HWND notifyParent;
    HIMAGELIST imageList, disabledImageList;
    int nonfloating : 1;
} TBData;

struct _stList
{
    struct _singleTab *head;
    struct _singleTab *tail;
};
struct _stItem
{
    struct _singleTab *prev;
    struct _singleTab *next;
};
struct _singleTab
{
    struct _stItem chain;
    struct _stItem selectedChain;
    char *text;
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
    struct _singleTab *highlighted;
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
    struct _singleTab *dragSrc;
    struct _singleTab **menuList;
} ;

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
} ;
typedef struct
{
    char *colText1;
    char *colText2;
    HBITMAP colBmp1;
    HBITMAP colBmp2;
} TCHeader;

typedef struct
{
    char *col1Text;
    char *col2Text;
    DWORD col1Color;
    DWORD col2Color;
} TCData;

#define TABM_ADD 	WM_USER
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
#define TABN_CLOSED   (-1001)
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
	char *text;
	LPARAM lParam;
} LSTABNOTIFY;

#endif