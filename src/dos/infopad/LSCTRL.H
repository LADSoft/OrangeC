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

#ifndef LSCTRL_H
#define LSCTRL_H
#pragma pack(1)

#define GRIPWIDTH 18

#ifndef GRIP_DEFINES
#define TITLEWIDTH 15
#define EDGEWIDTH 1
#define TITLETOP 5
#define BARLEFT 3
#define BARRIGHT (BUTTONCORNER + 2 * BUTTONWIDTH + 4)
#define BARTOP 0
#define BUTTONCORNER 1
#define BUTTONWIDTH 12
#else
#define TITLEWIDTH 18
#define EDGEWIDTH 1
#define TITLETOP 6
#define BARLEFT 3
#define BARRIGHT 3
#define BARTOP 0
#define BUTTONCORNER 1
#define BUTTONWIDTH 12
#endif

#define FRAMEWIDTH 6

#define LCF_ADJUSTRECT WM_USER + 1024
#define LCF_SETFLEX WM_USER + 1025
#define LCF_SETVERTICAL WM_USER + 1026
#define LCF_CONTAINER WM_USER + 1027
#define LCF_SETACTIVE WM_USER + 1028
#define LCF_ADDCONTROL WM_USER + 1029

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

#define DOCK_PRESENT 0x8000
#define DOCK_LEFT 1
#define DOCK_TOP 2
#define DOCK_RIGHT 4
#define DOCK_BOTTOM 8
#define DOCK_AMONG 16

#define FLEX_REQ 1
#define FLEX_UNREQ 2
#define FLEX_SMALL 4
#define FLEX_LARGE 8

typedef struct
{
    int flags;
    int oldflags;
    int hidden;
    int flexparams;
    int rowindex; // row for top & bottom, col for other
    int colindex; // col for top & bottom, row for other 
    int hiddenwidth; // adjusted width for top & bottom, height for other
    RECT oldsize;
    RECT position;
    RECT lastposition;
    RECT flexposition;
} CCD_params;

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

#define TABN_SELECTED (-1000)
#define TABN_CLOSED   (-1001)

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

#define LSREGULAR 1
#define LSTOOLBAR 2
#define LSMENU 3
#define LSCONTROL 4
#define LSGRIP 5
#define LSCONTAINER 6
#define LSBLANK 7
#define LSFRAME 8

typedef struct _ccw_params
{
    int type;
    int id;
    int vertical;
	HWND hwnd;
    struct _ccw_params *parent;
	struct _ccw_params *child;
    union
    {
        struct 
        {
            RECT position;
            CCD_params *representative_dock;
        } bw;
        struct
        {
            int flexed;
            int disabled;
		    int active;
            HWND closebutton;
            HWND flexbutton;
            HWND containerclosebutton;
        } cw;
		struct 
		{
		    RECT position;
		} fw;
        struct
        {
            int width;
            int height;
            TBBUTTON *buttons;
            HWND *children;
            int btncount;
            char **hints;
            WCHAR **whints;
            int bmpid;
            int bmpcount;
            int helpitem;
            HWND hWnd, vWnd;
            SIZE hsize, vsize;
            HWND notifyparent;
			HIMAGELIST imageList;
			HIMAGELIST disabledImageList;
        } tb;
    }
    u;
} CCW_params;

#define COLOR_MENUHIGHLIGHT 29
#define COLOR_MENUBAR 30

#pragma pack()
#endif