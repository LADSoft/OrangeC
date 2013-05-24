/*            
   prstruc.h

   Base definitions

   Copyright (C) 1996 Free Software Foundation, Inc.

   This file is part of the Windows32 API Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   If you are interested in a warranty or support for this source code,
   contact Scott Christley <scottc@net-community.com> for more information.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   -----------
   DAL 2003 - this file modified extensively for my compiler.  New
   definitionswnwn added as well.
*/ 

#pragma pack(4)
struct _PSP;
typedef struct _PSP FAR* HPROPSHEETPAGE;

struct _PROPSHEETPAGE;

typedef UINT (CALLBACK FAR * LPFNPSPCALLBACK)(HWND hwnd, UINT uMsg, struct _PROPSHEETPAGE FAR *ppsp);

#define PROPSHEETPAGE_V1_SIZE CCSIZEOF_STRUCT(PROPSHEETPAGE, pcRefParent)

typedef struct _PROPSHEETPAGE {
        DWORD           dwSize;
        DWORD           dwFlags;
        HINSTANCE       hInstance;
        union {
            LPCTSTR          pszTemplate;
            LPCDLGTEMPLATE  pResource;
        } u;
        union {
            HICON       hIcon;
            LPCTSTR      pszIcon;
        } u2;
        LPCTSTR          pszTitle;
        DLGPROC         pfnDlgProc;
        LPARAM          lParam;
        LPFNPSPCALLBACK pfnCallback;
        UINT FAR * pcRefParent;

        LPCTSTR pszHeaderTitle;    
        LPCTSTR pszHeaderSubTitle; 
} PROPSHEETPAGE, FAR *LPPROPSHEETPAGE;

typedef const PROPSHEETPAGE FAR *LPCPROPSHEETPAGE;

typedef int (CALLBACK *PFNPROPSHEETCALLBACK)(HWND, UINT, LPARAM);

#define PROPSHEETHEADERA_V1_SIZE CCSIZEOF_STRUCT(PROPSHEETHEADERA, pfnCallback)
#define PROPSHEETHEADERW_V1_SIZE CCSIZEOF_STRUCT(PROPSHEETHEADERW, pfnCallback)

typedef struct _PROPSHEETHEADER {
        DWORD           dwSize;
        DWORD           dwFlags;
        HWND            hwndParent;
        HINSTANCE       hInstance;
        union {
            HICON       hIcon;
            LPCTSTR      pszIcon;
        }u;
        LPCTSTR          pszCaption;

        UINT            nPages;
        union {
            UINT        nStartPage;
            LPCTSTR      pStartPage;
        }u2;
        union {
            LPCPROPSHEETPAGE ppsp;
            HPROPSHEETPAGE FAR *phpage;
        }u3;
        PFNPROPSHEETCALLBACK pfnCallback;

        union {
            HBITMAP hbmWatermark;
            LPCTSTR pszbmWatermark;
        } u4;
        HPALETTE hplWatermark;
        union {
            HBITMAP hbmHeader;     
            LPCTSTR pszbmHeader;
        } u5;
} PROPSHEETHEADER, FAR *LPPROPSHEETHEADER;

typedef const PROPSHEETHEADER FAR *LPCPROPSHEETHEADER;



typedef struct _PSHNOTIFY
{
    NMHDR hdr;
    LPARAM lParam;
} PSHNOTIFY, FAR *LPPSHNOTIFY;

#pragma pack()

