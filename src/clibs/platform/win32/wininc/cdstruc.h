/* 
   cdstruc.h

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

typedef UINT (CALLBACK *LPOFNHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

#pragma pack(1)

typedef struct tagOFN {
   DWORD        lStructSize;
   HWND         hwndOwner;
   HINSTANCE    hInstance;
   LPCTSTR       lpstrFilter;
   LPTSTR        lpstrCustomFilter;
   DWORD        nMaxCustFilter;
   DWORD        nFilterIndex;
   LPTSTR        lpstrFile;
   DWORD        nMaxFile;
   LPTSTR        lpstrFileTitle;
   DWORD        nMaxFileTitle;
   LPCTSTR       lpstrInitialDir;
   LPCTSTR       lpstrTitle;
   DWORD        Flags;
   WORD         nFileOffset;
   WORD         nFileExtension;
   LPCTSTR       lpstrDefExt;
   LPARAM       lCustData;
   LPOFNHOOKPROC lpfnHook;
   LPCTSTR       lpTemplateName;
   void *       pvReserved;
   DWORD        dwReserved;
   DWORD        FlagsEx;
} OPENFILENAME, *LPOPENFILENAME;

#define OPENFILENAME_SIZE_VERSION_400  CDSIZEOF_STRUCT(OPENFILENAME,lpTemplateName)

typedef UINT (CALLBACK *LPCCHOOKPROC) (HWND, UINT, WPARAM, LPARAM);
typedef struct 
_OFNOTIFY
{
        NMHDR           hdr;
        LPOPENFILENAME lpOFN;
        LPSTR           pszFile;        
} OFNOTIFY, FAR *LPOFNOTIFY;

typedef struct _OFNOTIFYEX
{
        NMHDR           hdr;
        LPOPENFILENAME lpOFN;
        LPVOID          psf;
        LPVOID          pidl;          
} OFNOTIFYEX, FAR *LPOFNOTIFYEX;

typedef struct tagCHOOSECOLOR {
   DWORD        lStructSize;
   HWND         hwndOwner;
   HWND         hInstance;
   COLORREF     rgbResult;
   COLORREF*    lpCustColors;
   DWORD        Flags;
   LPARAM       lCustData;
   LPCCHOOKPROC lpfnHook;
   LPCTSTR       lpTemplateName;
} CHOOSECOLOR, *LPCHOOSECOLOR;

typedef UINT (CALLBACK *LPFRHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef struct tagFINDREPLACE {
   DWORD        lStructSize;        
   HWND         hwndOwner;          
   HINSTANCE    hInstance;          
                                    
   DWORD        Flags;              
   LPTSTR        lpstrFindWhat;      
   LPTSTR        lpstrReplaceWith;   
   WORD         wFindWhatLen;       
   WORD         wReplaceWithLen;    
   LPARAM       lCustData;          
   LPFRHOOKPROC lpfnHook;           
   LPCSTR       lpTemplateName;     
} FINDREPLACE, *LPFINDREPLACE;

typedef UINT (CALLBACK *LPCFHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef struct tagCHOOSEFONT {
   DWORD           lStructSize;
   HWND            hwndOwner;          
   HDC             hDC;                
   LPLOGFONT       lpLogFont;          
   INT             iPointSize;         
   DWORD           Flags;              
   COLORREF        rgbColors;          
   LPARAM          lCustData;          
   LPCFHOOKPROC    lpfnHook;           
   LPCSTR          lpTemplateName;     
   HINSTANCE       hInstance;          
                                 
   LPTSTR           lpszStyle;          
                                       
   WORD            nFontType;          
                                                                              
   WORD            ___MISSING_ALIGNMENT__;
   INT             nSizeMin;           
   INT             nSizeMax;           
                                       
} CHOOSEFONT, *LPCHOOSEFONT;

typedef UINT (CALLBACK *LPPRINTHOOKPROC) (HWND, UINT, WPARAM, LPARAM);
typedef UINT (CALLBACK *LPSETUPHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef struct tagPD {
   DWORD            lStructSize;
   HWND             hwndOwner;
   HGLOBAL          hDevMode;
   HGLOBAL          hDevNames;
   HDC              hDC;
   DWORD            Flags;
   WORD             nFromPage;
   WORD             nToPage;
   WORD             nMinPage;
   WORD             nMaxPage;
   WORD             nCopies;
   HINSTANCE        hInstance;
   LPARAM           lCustData;
   LPPRINTHOOKPROC  lpfnPrintHook;
   LPSETUPHOOKPROC  lpfnSetupHook;
   LPCTSTR           lpPrintTemplateName;
   LPCTSTR           lpSetupTemplateName;
   HGLOBAL          hPrintTemplate;
   HGLOBAL          hSetupTemplate;
} PRINTDLG, *LPPRINTDLG;

typedef struct tagPRINTPAGERANGE {
   DWORD  nFromPage;
   DWORD  nToPage;
} PRINTPAGERANGE, *LPPRINTPAGERANGE;

typedef struct tagPDEX {
   DWORD                 lStructSize;          
   HWND                  hwndOwner;            
   HGLOBAL               hDevMode;             
   HGLOBAL               hDevNames;            
   HDC                   hDC;                  
   DWORD                 Flags;                
   DWORD                 Flags2;               
   DWORD                 ExclusionFlags;       
   DWORD                 nPageRanges;          
   DWORD                 nMaxPageRanges;       
   LPPRINTPAGERANGE      lpPageRanges;         
   DWORD                 nMinPage;             
   DWORD                 nMaxPage;             
   DWORD                 nCopies;              
   HINSTANCE             hInstance;            
   LPCTSTR                lpPrintTemplateName;  
   LPUNKNOWN             lpCallback;           
   DWORD                 nPropertyPages;       
   HPROPSHEETPAGE       *lphPropertyPages;     
   DWORD                 nStartPage;           
   DWORD                 dwResultAction;       
} PRINTDLGEX, *LPPRINTDLGEX;
typedef struct tagDEVNAMES {
   WORD wDriverOffset;
   WORD wDeviceOffset;
   WORD wOutputOffset;
   WORD wDefault;
} DEVNAMES, *LPDEVNAMES;

typedef UINT (CALLBACK* LPPAGEPAINTHOOK)( HWND, UINT, WPARAM, LPARAM );
typedef UINT (CALLBACK* LPPAGESETUPHOOK)( HWND, UINT, WPARAM, LPARAM );

typedef struct tagPSD
{
    DWORD           lStructSize;
    HWND            hwndOwner;
    HGLOBAL         hDevMode;
    HGLOBAL         hDevNames;
    DWORD           Flags;
    POINT           ptPaperSize;
    RECT            rtMinMargin;
    RECT            rtMargin;
    HINSTANCE       hInstance;
    LPARAM          lCustData;
    LPPAGESETUPHOOK lpfnPageSetupHook;
    LPPAGEPAINTHOOK lpfnPagePaintHook;
    LPCTSTR          lpPageSetupTemplateName;
    HGLOBAL         hPageSetupTemplate;
} PAGESETUPDLG, * LPPAGESETUPDLG;

#pragma pack()