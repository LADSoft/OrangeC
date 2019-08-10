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

#ifndef _RCGUI_H
#define _RCGUI_H

#include <windows.h>

#define IL_CHECKBOX 0
#define IL_COMBOBOX 1
#define IL_EDIT 2
#define IL_FRAME 3
#define IL_GROUPBOX 4
#define IL_HSCROLL 5
#define IL_ICON 6
#define IL_LISTBOX 7
#define IL_PUSHBUTTON 8
#define IL_RADIOBUTTON 9
#define IL_RECTANGLE 10
#define IL_STATICTEXT 11
#define IL_VSCROLL 12
#define IL_ETCHED 13

struct keypair
{
    struct keypair* next;
    int index;
    char* key;
    char* value;
};
struct resRes
{
    enum pi_type type;  // shared with project setting
    struct resRes* next;
    char* name;
    RESOURCE* resource;
    HTREEITEM hTreeItem;
    HWND activeHwnd;
    struct guiData
    {
        struct keypair* versionKeys;
        void* undoData;
        HWND childWindow;
        HWND editWindow;
        MENUITEM* selectedMenu;
        struct _imagedata* images;
        HFONT font;
        POINT scrollPos;
        POINT scrollMax;
        POINT origin;
        int fontHeight;
        unsigned short dragStart;
        unsigned short selectedRow;
        unsigned short selectedColumn;
#define DA_NONE 0
#define DA_CREATION 1
#define DA_TAB 2
#define DA_GROUP 3

        int dlgAltMode : 3;
        int bDragging : 1;
        int dragInView : 1;
        int lvscroll : 3;
        int cantClearUndo : 1;
    } gd;
};
struct resDir
{
    struct resDir* next;
    int type;
    struct resRes* children;
    HTREEITEM hTreeItem;
};
struct resData
{
    RESOURCE_DATA* resources;
    FILETIME time;
    struct resDir* dirs;
};

struct propertyFuncs
{
    void (*draw)(HWND lv, void* data);
    void (*getText)(char* buf, HWND lv, void* data, int row);
    HWND (*startEdit)(HWND lv, int row, void* data);
    void (*finishEdit)(HWND lv, int row, HWND editWnd, void* data);
};
struct ctlData
{
    WNDPROC oldWndProcRedir;
    HWND hwndParent, hwndRedir;
    HWND hwndPrev;
    struct resRes* dlg;
    CONTROL* data;
    struct ctlDB* db;
    int sizing : 1;
};

#ifndef LVIF_GROUPID
typedef struct tagLVITEMA_x
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
    int iIndent;
    int iGroupId;
    UINT cColumns;  // tile view columns
    PUINT puColumns;
} LVITEMA_x, *LPLVITEMA_x;

typedef struct tagLVHITTESTINFO_x
{
    POINT pt;
    UINT flags;
    int iItem;
    int iSubItem;  // this is was NOT in win95.  valid only for LVM_SUBITEMHITTEST
    int iGroup;
} LVHITTESTINFO_x, *LPLVHITTESTINFO_x;

#    ifndef LVIF_GROUPID
#        define LVIF_GROUPID 0x100
#        define LVIF_COLUMNS 0x0200

#        define LVGF_NONE 0x00000000
#        define LVGF_HEADER 0x00000001
#        define LVGF_FOOTER 0x00000002
#        define LVGF_STATE 0x00000004
#        define LVGF_ALIGN 0x00000008
#        define LVGF_GROUPID 0x00000010

#        define LVGS_NORMAL 0x00000000
#        define LVGS_COLLAPSED 0x00000001
#        define LVGS_HIDDEN 0x00000002

#        define LVGA_HEADER_LEFT 0x00000001
#        define LVGA_HEADER_CENTER 0x00000002
#        define LVGA_HEADER_RIGHT 0x00000004  // Don't forget to validate exclusivity
#        define LVGA_FOOTER_LEFT 0x00000008
#        define LVGA_FOOTER_CENTER 0x00000010
#        define LVGA_FOOTER_RIGHT 0x00000020  // Don't forget to validate exclusivity

typedef struct tagLVGROUP
{
    UINT cbSize;
    UINT mask;
    LPWSTR pszHeader;
    int cchHeader;

    LPWSTR pszFooter;
    int cchFooter;

    int iGroupId;

    UINT stateMask;
    UINT state;
    UINT uAlign;
} LVGROUP, *PLVGROUP;

#        ifndef LVM_INSERTGROUP
#            define LVM_INSERTGROUP (LVM_FIRST + 145)
#            define ListView_InsertGroup(hwnd, index, pgrp) SNDMSG((hwnd), LVM_INSERTGROUP, (WPARAM)index, (LPARAM)pgrp)

#            define LVM_SETGROUPINFO (LVM_FIRST + 147)
#            define ListView_SetGroupInfo(hwnd, iGroupId, pgrp) SNDMSG((hwnd), LVM_SETGROUPINFO, (WPARAM)iGroupId, (LPARAM)pgrp)

#            define LVM_GETGROUPINFO (LVM_FIRST + 149)
#            define ListView_GetGroupInfo(hwnd, iGroupId, pgrp) SNDMSG((hwnd), LVM_GETGROUPINFO, (WPARAM)iGroupId, (LPARAM)pgrp)

#            define LVM_REMOVEGROUP (LVM_FIRST + 150)
#            define ListView_RemoveGroup(hwnd, iGroupId) SNDMSG((hwnd), LVM_REMOVEGROUP, (WPARAM)iGroupId, 0)

#            define LVM_MOVEGROUP (LVM_FIRST + 151)
#            define ListView_MoveGroup(hwnd, iGroupId, toIndex) SNDMSG((hwnd), LVM_MOVEGROUP, (WPARAM)iGroupId, (LPARAM)toIndex)

#            define LVM_ENABLEGROUPVIEW (LVM_FIRST + 157)
#            define ListView_EnableGroupView(hwnd, fEnable) SNDMSG((hwnd), LVM_ENABLEGROUPVIEW, (WPARAM)fEnable, 0)
#        endif
#    endif
#else
typedef struct tagLVITEMA LVITEMA_x, *LPLVITEMA_x;
typedef struct tagLVHITTESTINFO LVHITTESTINFO_x, *LPLVHITTESTINFO_x;

#endif

#endif