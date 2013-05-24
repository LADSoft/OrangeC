/* 
   ccstruc.h

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
#pragma pack(8) 
typedef struct tagINITCOMMONCONTROLSEX {
    DWORD dwSize;             
    DWORD dwICC;              
} INITCOMMONCONTROLSEX, *LPINITCOMMONCONTROLSEX;

typedef struct tagCOLORSCHEME {
   DWORD            dwSize;
   COLORREF         clrBtnHighlight;       
   COLORREF         clrBtnShadow;          
} COLORSCHEME, *LPCOLORSCHEME;

typedef struct tagNMTOOLTIPSCREATED
{
    NMHDR hdr;
    HWND hwndToolTips;
} NMTOOLTIPSCREATED, * LPNMTOOLTIPSCREATED;

typedef struct tagNMMOUSE {
    NMHDR   hdr;
    DWORD dwItemSpec;
    DWORD dwItemData;
    POINT   pt;
    LPARAM  dwHitInfo; 
} NMMOUSE, * LPNMMOUSE;

typedef NMMOUSE NMCLICK;
typedef LPNMMOUSE LPNMCLICK;


typedef struct tagNMOBJECTNOTIFY {
    NMHDR   hdr;
    int     iItem;
    const void *piid;
    void *pObject;
    HRESULT hResult;
    DWORD dwFlags;    
} NMOBJECTNOTIFY, *LPNMOBJECTNOTIFY;

typedef struct tagNMKEY
{
    NMHDR hdr;
    UINT  nVKey;
    UINT  uFlags;
} NMKEY,  *LPNMKEY;

typedef struct tagNMCHAR {
    NMHDR   hdr;
    UINT    ch;
    DWORD   dwItemPrev;     
    DWORD   dwItemNext;     
} NMCHAR, * LPNMCHAR;

typedef struct tagNMCUSTOMDRAWINFO
{
    NMHDR hdr;
    DWORD dwDrawStage;
    HDC hdc;
    RECT rc;
    DWORD dwItemSpec;  
    UINT  uItemState;
    LPARAM lItemlParam;
} NMCUSTOMDRAW,  * LPNMCUSTOMDRAW;

typedef struct tagNMTTCUSTOMDRAW
{
    NMCUSTOMDRAW nmcd;
    UINT uDrawFlags;
} NMTTCUSTOMDRAW,  * LPNMTTCUSTOMDRAW;

struct _IMAGELIST;
typedef struct _IMAGELIST NEAR* HIMAGELIST;

typedef struct _IMAGELISTDRAWPARAMS {
    DWORD       cbSize;
    HIMAGELIST  himl;
    int         i;
    HDC         hdcDst;
    int         x;
    int         y;
    int         cx;
    int         cy;
    int         xBitmap;        
    int         yBitmap;        
    COLORREF    rgbBk;
    COLORREF    rgbFg;
    UINT        fStyle;
    DWORD       dwRop;
} IMAGELISTDRAWPARAMS,  * LPIMAGELISTDRAWPARAMS;

typedef struct _IMAGEINFO
{
    HBITMAP hbmImage;
    HBITMAP hbmMask;
    int     Unused1;
    int     Unused2;
    RECT    rcImage;
} IMAGEINFO,  *LPIMAGEINFO;

typedef struct _HD_TEXTFILTERA
{
    LPSTR pszText;                      
    INT cchTextMax;                     
} HD_TEXTFILTERA,  * LPHD_TEXTFILTERA;

typedef struct _HD_TEXTFILTERW
{
    LPWSTR pszText;                     
    INT cchTextMax;                     
} HD_TEXTFILTERW,  * LPHD_TEXTFILTERW;

#define HD_ITEMA HDITEMA
#define HD_ITEMW HDITEMW
#define HD_ITEM HDITEM

typedef struct _HD_ITEMA
{
    UINT    mask;
    int     cxy;
    LPSTR   pszText;
    HBITMAP hbm;
    int     cchTextMax;
    int     fmt;
    LPARAM  lParam;
    int     iImage;        
    int     iOrder;        
    UINT    type;           
    LPVOID  pvFilter;       
} HDITEMA,  * LPHDITEMA;

#define HDITEMA_V1_SIZE CCSIZEOF_STRUCT(HDITEMA, lParam)
#define HDITEMW_V1_SIZE CCSIZEOF_STRUCT(HDITEMW, lParam)


typedef struct _HD_ITEMW
{
    UINT    mask;
    int     cxy;
    LPWSTR   pszText;
    HBITMAP hbm;
    int     cchTextMax;
    int     fmt;
    LPARAM  lParam;
    int     iImage;        
    int     iOrder;
    UINT    type;           
    LPVOID  pvFilter;       
} HDITEMW,  * LPHDITEMW;

#ifdef UNICODE
#define HDITEM HDITEMW
#define LPHDITEM LPHDITEMW
#define HDITEM_V1_SIZE HDITEMW_V1_SIZE
#else
#define HDITEM HDITEMA
#define LPHDITEM LPHDITEMA
#define HDITEM_V1_SIZE HDITEMA_V1_SIZE
#endif


typedef struct _HD_LAYOUT
{
    RECT * prc;
    WINDOWPOS * pwpos;
} HDLAYOUT,  *LPHDLAYOUT;

#define HD_HITTESTINFO HDHITTESTINFO

typedef struct _HD_HITTESTINFO
{
    POINT pt;
    UINT flags;
    int iItem;
} HDHITTESTINFO,  *LPHDHITTESTINFO;


#define HD_NOTIFYA              NMHEADERA
#define HD_NOTIFYW              NMHEADERW
#define HD_NOTIFY               NMHEADER

typedef struct tagNMHEADERA
{
    NMHDR   hdr;
    int     iItem;
    int     iButton;
    HDITEMA * pitem;
}  NMHEADERA, * LPNMHEADERA;


typedef struct tagNMHEADERW
{
    NMHDR   hdr;
    int     iItem;
    int     iButton;
    HDITEMW * pitem;
} NMHEADERW, * LPNMHEADERW;

#ifdef UNICODE
#define NMHEADER                NMHEADERW
#define LPNMHEADER              LPNMHEADERW
#else
#define NMHEADER                NMHEADERA
#define LPNMHEADER              LPNMHEADERA
#endif

typedef struct tagNMHDDISPINFOW
{
    NMHDR   hdr;
    int     iItem;
    UINT    mask;
    LPWSTR  pszText;
    int     cchTextMax;
    int     iImage;
    LPARAM  lParam;
} NMHDDISPINFOW, * LPNMHDDISPINFOW;

typedef struct tagNMHDDISPINFOA
{
    NMHDR   hdr;
    int     iItem;
    UINT    mask;
    LPSTR   pszText;
    int     cchTextMax;
    int     iImage;
    LPARAM  lParam;
} NMHDDISPINFOA, * LPNMHDDISPINFOA;


#ifdef UNICODE
#define NMHDDISPINFO            NMHDDISPINFOW
#define LPNMHDDISPINFO          LPNMHDDISPINFOW
#else
#define NMHDDISPINFO            NMHDDISPINFOA
#define LPNMHDDISPINFO          LPNMHDDISPINFOA
#endif

typedef struct tagNMHDFILTERBTNCLICK
{
    NMHDR hdr;
    INT iItem;
    RECT rc;
} NMHDFILTERBTNCLICK, * LPNMHDFILTERBTNCLICK;

typedef struct _TBBUTTON {
    int iBitmap;
    int idCommand;
    BYTE fsState;
    BYTE fsStyle;
    BYTE reserved[2] ;
    DWORD dwData;
    int iString;
} TBBUTTON, NEAR* PTBBUTTON, * LPTBBUTTON;
typedef const TBBUTTON * LPCTBBUTTON;


typedef struct _COLORMAP {
    COLORREF from;
    COLORREF to;
} COLORMAP, * LPCOLORMAP;


typedef struct _NMTBCUSTOMDRAW {
    NMCUSTOMDRAW nmcd;
    HBRUSH hbrMonoDither;
    HBRUSH hbrLines;                
    HPEN hpenLines;                 

    COLORREF clrText;               
    COLORREF clrMark;               
    COLORREF clrTextHighlight;      
    COLORREF clrBtnFace;            
    COLORREF clrBtnHighlight;       
    COLORREF clrHighlightHotTrack;  
                                    
    RECT rcText;                    

    int nStringBkMode;
    int nHLStringBkMode;
} NMTBCUSTOMDRAW, * LPNMTBCUSTOMDRAW;

typedef struct tagTBADDBITMAP {
        HINSTANCE       hInst;
        UINT        nID;
} TBADDBITMAP, *LPTBADDBITMAP;

typedef struct tagTBSAVEPARAMS {
    HKEY hkr;
    LPCTSTR pszSubKey;
    LPCTSTR pszValueName;
} TBSAVEPARAMS, * LPTBSAVEPARAMS;

typedef struct {
    HINSTANCE       hInstOld;
    UINT        nIDOld;
    HINSTANCE       hInstNew;
    UINT        nIDNew;
    int             nButtons;
} TBREPLACEBITMAP, *LPTBREPLACEBITMAP;

typedef struct {
    UINT cbSize;
    DWORD dwMask;
    int idCommand;
    int iImage;
    BYTE fsState;
    BYTE fsStyle;
    WORD cx;
    DWORD lParam;
    LPSTR pszText;
    int cchText;
} TBBUTTONINFOA, *LPTBBUTTONINFOA;

typedef struct {
    UINT cbSize;
    DWORD dwMask;
    int idCommand;
    int iImage;
    BYTE fsState;
    BYTE fsStyle;
    WORD cx;
    DWORD lParam;
    LPWSTR pszText;
    int cchText;
} TBBUTTONINFOW, *LPTBBUTTONINFOW;

#ifdef UNICODE
#define TBBUTTONINFO TBBUTTONINFOW
#define LPTBBUTTONINFO LPTBBUTTONINFOW
#else
#define TBBUTTONINFO TBBUTTONINFOA
#define LPTBBUTTONINFO LPTBBUTTONINFOA
#endif

typedef struct tagNMTBHOTITEM
{
    NMHDR   hdr;
    int     idOld;
    int     idNew;
    DWORD   dwFlags;           
} NMTBHOTITEM, * LPNMTBHOTITEM;

typedef struct tagNMTBSAVE
{
    NMHDR hdr;
    DWORD* pData;
    DWORD* pCurrent;
    UINT cbData;
    int iItem;
    int cButtons;
    TBBUTTON tbButton;
} NMTBSAVE, *LPNMTBSAVE;

typedef struct tagNMTBRESTORE
{
    NMHDR hdr;
    DWORD* pData;
    DWORD* pCurrent;
    UINT cbData;
    int iItem;
    int cButtons;
    int cbBytesPerRecord;
    TBBUTTON tbButton;
} NMTBRESTORE, *LPNMTBRESTORE;

typedef struct tagNMTBGETINFOTIPA
{
    NMHDR hdr;
    LPSTR pszText;
    int cchTextMax;
    int iItem;
    LPARAM lParam;
} NMTBGETINFOTIPA, *LPNMTBGETINFOTIPA;

typedef struct tagNMTBGETINFOTIPW
{
    NMHDR hdr;
    LPWSTR pszText;
    int cchTextMax;
    int iItem;
    LPARAM lParam;
} NMTBGETINFOTIPW, *LPNMTBGETINFOTIPW;

#ifdef UNICODE
#define TBN_GETINFOTIP          TBN_GETINFOTIPW
#define NMTBGETINFOTIP          NMTBGETINFOTIPW
#define LPNMTBGETINFOTIP        LPNMTBGETINFOTIPW
#else
#define TBN_GETINFOTIP          TBN_GETINFOTIPA
#define NMTBGETINFOTIP          NMTBGETINFOTIPA
#define LPNMTBGETINFOTIP        LPNMTBGETINFOTIPA
#endif

typedef struct {
    NMHDR  hdr;
    DWORD dwMask;     
    int idCommand;    
    DWORD lParam;  
    int iImage;       
    LPSTR pszText;    
    int cchText;      
} NMTBDISPINFOA, *LPNMTBDISPINFOA;

typedef struct {
    NMHDR hdr;
    DWORD dwMask;      
    int idCommand;    
    DWORD lParam;  
    int iImage;       
    LPWSTR pszText;   
    int cchText;      
} NMTBDISPINFOW, *LPNMTBDISPINFOW;


#ifdef UNICODE
#define TBN_GETDISPINFO       TBN_GETDISPINFOW
#define NMTBDISPINFO          NMTBDISPINFOW
#define LPNMTBDISPINFO        LPNMTBDISPINFOW
#else
#define TBN_GETDISPINFO       TBN_GETDISPINFOA
#define NMTBDISPINFO          NMTBDISPINFOA
#define LPNMTBDISPINFO        LPNMTBDISPINFOA
#endif

#define TBNOTIFYA NMTOOLBARA
#define TBNOTIFYW NMTOOLBARW
#define LPTBNOTIFYA LPNMTOOLBARA
#define LPTBNOTIFYW LPNMTOOLBARW

#define TBNOTIFY       NMTOOLBAR
#define LPTBNOTIFY     LPNMTOOLBAR

typedef struct tagNMTOOLBAR {
    NMHDR   hdr;
    int     iItem;
    TBBUTTON tbButton;
    int     cchText;
    LPTSTR   pszText;
    RECT    rcButton;
} NMTOOLBAR, * LPNMTOOLBAR;

typedef struct tagREBARINFO
{
    UINT        cbSize;
    UINT        fMask;
    HANDLE      himl;
}   REBARINFO,  *LPREBARINFO;

typedef struct tagREBARBANDINFO
{
    UINT        cbSize;
    UINT        fMask;
    UINT        fStyle;
    COLORREF    clrFore;
    COLORREF    clrBack;
    LPTSTR       lpText;
    UINT        cch;
    int         iImage;
    HWND        hwndChild;
    UINT        cxMinChild;
    UINT        cyMinChild;
    UINT        cx;
    HBITMAP     hbmBack;
    UINT        wID;
    UINT        cyChild;
    UINT        cyMaxChild;
    UINT        cyIntegral;
    UINT        cxIdeal;
    LPARAM      lParam;
    UINT        cxHeader;
}   REBARBANDINFO,  *LPREBARBANDINFO;
typedef REBARBANDINFO CONST  *LPCREBARBANDINFO;

#define REBARBANDINFO_V3_SIZE CCSIZEOF_STRUCT(REBARBANDINFO, wID)

typedef struct tagNMREBARCHILDSIZE
{
    NMHDR hdr;
    UINT uBand;
    UINT wID;
    RECT rcChild;
    RECT rcBand;
} NMREBARCHILDSIZE, *LPNMREBARCHILDSIZE;

typedef struct tagNMREBAR
{
    NMHDR   hdr;
    DWORD   dwMask;           
    UINT    uBand;
    UINT    fStyle;
    UINT    wID;
    LPARAM  lParam;
} NMREBAR, *LPNMREBAR;


typedef struct tagNMRBAUTOSIZE
{
    NMHDR hdr;
    BOOL fChanged;
    RECT rcTarget;
    RECT rcActual;
} NMRBAUTOSIZE, *LPNMRBAUTOSIZE;

typedef struct tagNMREBARCHEVRON
{
    NMHDR hdr;
    UINT uBand;
    UINT wID;
    LPARAM lParam;
    RECT rc;
    LPARAM lParamNM;
} NMREBARCHEVRON, *LPNMREBARCHEVRON;

typedef struct _RB_HITTESTINFO
{
    POINT pt;
    UINT flags;
    int iBand;
} RBHITTESTINFO,  *LPRBHITTESTINFO;

#define LPTOOLINFOA   LPTTTOOLINFOA
#define LPTOOLINFOW   LPTTTOOLINFOW
#define TOOLINFOA       TTTOOLINFOA
#define TOOLINFOW       TTTOOLINFOW
#define LPTOOLINFO    LPTTTOOLINFO
#define TOOLINFO        TTTOOLINFO

#define TTTOOLINFOA_V1_SIZE CCSIZEOF_STRUCT(TTTOOLINFOA, lpszText)
#define TTTOOLINFOW_V1_SIZE CCSIZEOF_STRUCT(TTTOOLINFOW, lpszText)

typedef struct tagTOOLINFO {
    UINT cbSize;
    UINT uFlags;
    HWND hwnd;
    UINT uId;
    RECT rect;
    HINSTANCE hinst;
    LPTSTR lpszText;
    LPARAM lParam;
} TTTOOLINFO, NEAR *PTOOLINFO,  *LPTTTOOLINFO;

#define LPHITTESTINFO     LPTTHITTESTINFO

typedef struct _TT_HITTESTINFO {
    HWND hwnd;
    POINT pt;
    TTTOOLINFO ti;
} TTHITTESTINFO,  * LPTTHITTESTINFO;

#define TOOLTIPTEXT    NMTTDISPINFO
#define LPTOOLTIPTEXT  LPNMTTDISPINFO

#define NMTTDISPINFO_V1_SIZE CCSIZEOF_STRUCT(NMTTDISPINFO, uFlags)

typedef struct tagNMTTDISPINFO {
    NMHDR hdr;
    LPTSTR lpszText;
    BCHAR szText[80];
    HINSTANCE hinst;
    UINT uFlags;
    LPARAM lParam;
} NMTTDISPINFO,  *LPNMTTDISPINFO;


typedef struct tagDRAGLISTINFO {
    UINT uNotification;
    HWND hWnd;
    POINT ptCursor;
} DRAGLISTINFO,  *LPDRAGLISTINFO;

typedef struct _UDACCEL {
    UINT nSec;
    UINT nInc;
} UDACCEL,  *LPUDACCEL;

typedef struct _NM_UPDOWN
{
    NMHDR hdr;
    int iPos;
    int iDelta;
} NMUPDOWN,  *LPNMUPDOWN;

#define I_INDENTCALLBACK        (-1)
#define LV_ITEMA LVITEMA
#define LV_ITEMW LVITEMW

#define LV_ITEM LVITEM

#define LVITEMA_V1_SIZE CCSIZEOF_STRUCT(LVITEMA, lParam)
#define LVITEMW_V1_SIZE CCSIZEOF_STRUCT(LVITEMW, lParam)

typedef struct tagLVITEMA
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
} LVITEMA, * LPLVITEMA;

typedef struct tagLVITEMW
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPWSTR pszText;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
    int iIndent;
} LVITEMW, * LPLVITEMW;


#ifdef UNICODE
#define LVITEM    LVITEMW
#define LPLVITEM  LPLVITEMW
#define LVITEM_V1_SIZE LVITEMW_V1_SIZE
#else
#define LVITEM    LVITEMA
#define LPLVITEM  LPLVITEMA
#define LVITEM_V1_SIZE LVITEMA_V1_SIZE
#endif

#define LV_FINDINFOA    LVFINDINFOA
#define LV_FINDINFOW    LVFINDINFOW
#define LV_FINDINFO  LVFINDINFO

typedef struct tagLVFINDINFOA
{
    UINT flags;
    LPCSTR psz;
    LPARAM lParam;
    POINT pt;
    UINT vkDirection;
} LVFINDINFOA, * LPFINDINFOA;

typedef struct tagLVFINDINFOW
{
    UINT flags;
    LPCWSTR psz;
    LPARAM lParam;
    POINT pt;
    UINT vkDirection;
} LVFINDINFOW, * LPFINDINFOW;

#ifdef UNICODE
#define  LVFINDINFO            LVFINDINFOW
#else
#define  LVFINDINFO            LVFINDINFOA
#endif

#define LV_HITTESTINFO LVHITTESTINFO
#define LVHITTESTINFO_V1_SIZE CCSIZEOF_STRUCT(LVHITTESTINFO, iItem)

typedef struct tagLVHITTESTINFO
{
    POINT pt;
    UINT flags;
    int iItem;
    int iSubItem;    
} LVHITTESTINFO, * LPLVHITTESTINFO;

#define LV_COLUMNA      LVCOLUMNA
#define LV_COLUMNW      LVCOLUMNW
#define LV_COLUMN       LVCOLUMN

#define LVCOLUMNA_V1_SIZE CCSIZEOF_STRUCT(LVCOLUMNA, iSubItem)
#define LVCOLUMNW_V1_SIZE CCSIZEOF_STRUCT(LVCOLUMNW, iSubItem)

typedef struct tagLVCOLUMNA
{
    UINT mask;
    int fmt;
    int cx;
    LPSTR pszText;
    int cchTextMax;
    int iSubItem;
    int iImage;
    int iOrder;
} LVCOLUMNA, * LPLVCOLUMNA;

typedef struct tagLVCOLUMNW
{
    UINT mask;
    int fmt;
    int cx;
    LPWSTR pszText;
    int cchTextMax;
    int iSubItem;
    int iImage;
    int iOrder;
} LVCOLUMNW, * LPLVCOLUMNW;

#ifdef UNICODE
#define  LVCOLUMN               LVCOLUMNW
#define  LPLVCOLUMN             LPLVCOLUMNW
#define LVCOLUMN_V1_SIZE LVCOLUMNW_V1_SIZE
#else
#define  LVCOLUMN               LVCOLUMNA
#define  LPLVCOLUMN             LPLVCOLUMNA
#define LVCOLUMN_V1_SIZE LVCOLUMNA_V1_SIZE
#endif

typedef int (CALLBACK *PFNLVCOMPARE)(LPARAM, LPARAM, LPARAM);

typedef struct tagLVBKIMAGEA
{
    ULONG ulFlags;              
    HBITMAP hbm;
    LPSTR pszImage;
    UINT cchImageMax;
    int xOffsetPercent;
    int yOffsetPercent;
} LVBKIMAGEA,  *LPLVBKIMAGEA;
typedef struct tagLVBKIMAGEW
{
    ULONG ulFlags;              
    HBITMAP hbm;
    LPWSTR pszImage;
    UINT cchImageMax;
    int xOffsetPercent;
    int yOffsetPercent;
} LVBKIMAGEW,  *LPLVBKIMAGEW;

#ifdef UNICODE
#define LVBKIMAGE               LVBKIMAGEW
#define LPLVBKIMAGE             LPLVBKIMAGEW
#define LVM_SETBKIMAGE          LVM_SETBKIMAGEW
#define LVM_GETBKIMAGE          LVM_GETBKIMAGEW
#else
#define LVBKIMAGE               LVBKIMAGEA
#define LPLVBKIMAGE             LPLVBKIMAGEA
#define LVM_SETBKIMAGE          LVM_SETBKIMAGEA
#define LVM_GETBKIMAGE          LVM_GETBKIMAGEA
#endif

typedef struct tagNMLISTVIEW
{
    NMHDR   hdr;
    int     iItem;
    int     iSubItem;
    UINT    uNewState;
    UINT    uOldState;
    UINT    uChanged;
    POINT   ptAction;
    LPARAM  lParam;
} NMLISTVIEW,  *LPNMLISTVIEW;


typedef struct tagNMITEMACTIVATE
{
    NMHDR   hdr;
    int     iItem;
    int     iSubItem;
    UINT    uNewState;
    UINT    uOldState;
    UINT    uChanged;
    POINT   ptAction;
    LPARAM  lParam;
    UINT    uKeyFlags;
} NMITEMACTIVATE,  *LPNMITEMACTIVATE;

#define NMLVCUSTOMDRAW_V3_SIZE CCSIZEOF_STRUCT(NMLVCUSTOMDRW, clrTextBk)

typedef struct tagNMLVCUSTOMDRAW
{
    NMCUSTOMDRAW nmcd;
    COLORREF clrText;
    COLORREF clrTextBk;
    int iSubItem;
} NMLVCUSTOMDRAW, *LPNMLVCUSTOMDRAW;

typedef struct tagNMLVCACHEHINT
{
    NMHDR   hdr;
    int     iFrom;
    int     iTo;
} NMLVCACHEHINT,  *LPNMLVCACHEHINT;

#define LPNM_CACHEHINT  LPNMLVCACHEHINT
#define PNM_CACHEHINT   LPNMLVCACHEHINT
#define NM_CACHEHINT    NMLVCACHEHINT

typedef struct tagNMLVFINDITEMA
{
    NMHDR   hdr;
    int     iStart;
    LVFINDINFOA lvfi;
} NMLVFINDITEMA,  *LPNMLVFINDITEMA;

typedef struct tagNMLVFINDITEMW
{
    NMHDR   hdr;
    int     iStart;
    LVFINDINFOW lvfi;
} NMLVFINDITEMW,  *LPNMLVFINDITEMW;

#define PNM_FINDITEMA   LPNMLVFINDITEMA
#define LPNM_FINDITEMA  LPNMLVFINDITEMA
#define NM_FINDITEMA    NMLVFINDITEMA

#define PNM_FINDITEMW   LPNMLVFINDITEMW
#define LPNM_FINDITEMW  LPNMLVFINDITEMW
#define NM_FINDITEMW    NMLVFINDITEMW

#ifdef UNICODE
#define PNM_FINDITEM    PNM_FINDITEMW
#define LPNM_FINDITEM   LPNM_FINDITEMW
#define NM_FINDITEM     NM_FINDITEMW
#define NMLVFINDITEM    NMLVFINDITEMW
#define LPNMLVFINDITEM  LPNMLVFINDITEMW
#else
#define PNM_FINDITEM    PNM_FINDITEMA
#define LPNM_FINDITEM   LPNM_FINDITEMA
#define NM_FINDITEM     NM_FINDITEMA
#define NMLVFINDITEM    NMLVFINDITEMA
#define LPNMLVFINDITEM  LPNMLVFINDITEMA
#endif

typedef struct tagNMLVODSTATECHANGE
{
    NMHDR hdr;
    int iFrom;
    int iTo;
    UINT uNewState;
    UINT uOldState;
} NMLVODSTATECHANGE,  *LPNMLVODSTATECHANGE;

#define PNM_ODSTATECHANGE   LPNMLVODSTATECHANGE
#define LPNM_ODSTATECHANGE  LPNMLVODSTATECHANGE
#define NM_ODSTATECHANGE    NMLVODSTATECHANGE

#define LV_DISPINFOA    NMLVDISPINFOA
#define LV_DISPINFOW    NMLVDISPINFOW
#define LV_DISPINFO     NMLVDISPINFO
typedef struct tagLVDISPINFO {
    NMHDR hdr;
    LVITEMA item;
} NMLVDISPINFOA,  *LPNMLVDISPINFOA;

typedef struct tagLVDISPINFOW {
    NMHDR hdr;
    LVITEMW item;
} NMLVDISPINFOW,  * LPNMLVDISPINFOW;

#ifdef UNICODE
#define  NMLVDISPINFO           NMLVDISPINFOW
#else
#define  NMLVDISPINFO           NMLVDISPINFOA
#endif

#pragma pack(1)

typedef struct tagLVKEYDOWN
{
    NMHDR hdr;
    WORD wVKey;
    UINT flags;
} NMLVKEYDOWN,  *LPNMLVKEYDOWN;

#pragma pack()

typedef struct tagNMLVGETINFOTIPA
{
    NMHDR hdr;
    DWORD dwFlags;
    LPSTR pszText;
    int cchTextMax;
    int iItem;
    int iSubItem;
    LPARAM lParam;
} NMLVGETINFOTIPA, *LPNMLVGETINFOTIPA;

typedef struct tagNMLVGETINFOTIPW
{
    NMHDR hdr;
    DWORD dwFlags;
    LPWSTR pszText;
    int cchTextMax;
    int iItem;
    int iSubItem;
    LPARAM lParam;
} NMLVGETINFOTIPW, *LPNMLVGETINFOTIPW;

#ifdef UNICODE
#define NMLVGETINFOTIP          NMLVGETINFOTIPW
#define LPNMLVGETINFOTIP        LPNMLVGETINFOTIPW
#else
#define NMLVGETINFOTIP          NMLVGETINFOTIPA
#define LPNMLVGETINFOTIP        LPNMLVGETINFOTIPA
#endif


typedef struct _TREEITEM * HTREEITEM;

#define LPTV_ITEMW              LPTVITEMW
#define LPTV_ITEMA              LPTVITEMA
#define TV_ITEMW                TVITEMW
#define TV_ITEMA                TVITEMA
#define LPTV_ITEM               LPTVITEM
#define TV_ITEM                 TVITEM

typedef struct tagTVITEM {
    UINT      mask;
    HTREEITEM hItem;
    UINT      state;
    UINT      stateMask;
    LPTSTR     pszText;
    int       cchTextMax;
    int       iImage;
    int       iSelectedImage;
    int       cChildren;
    LPARAM    lParam;
} TVITEM,  *LPTVITEM;

typedef struct tagTVITEMEX {
    UINT      mask;
    HTREEITEM hItem;
    UINT      state;
    UINT      stateMask;
    LPTSTR     pszText;
    int       cchTextMax;
    int       iImage;
    int       iSelectedImage;
    int       cChildren;
    LPARAM    lParam;
    int       iIntegral;
} TVITEMEX,  *LPTVITEMEX;


#define TVI_ROOT                ((HTREEITEM)(ULONG)-0x10000)
#define TVI_FIRST               ((HTREEITEM)(ULONG)-0x0FFFF)
#define TVI_LAST                ((HTREEITEM)(ULONG)-0x0FFFE)
#define TVI_SORT                ((HTREEITEM)(ULONG)-0x0FFFD)

#define TV_INSERTSTRUCT         TVINSERTSTRUCT
#define LPTV_INSERTSTRUCT       LPTVINSERTSTRUCT


#define TVINSERTSTRUCT_V1_SIZE CCSIZEOF_STRUCT(TVINSERTSTRUCT, item)

typedef struct tagTVINSERTSTRUCT {
    HTREEITEM hParent;
    HTREEITEM hInsertAfter;
    union
    {
        TVITEMEX itemex;
        TV_ITEM  item;
    } u;
} TVINSERTSTRUCT,  *LPTVINSERTSTRUCT;

typedef int (CALLBACK *PFNTVCOMPARE)(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

#define LPTV_HITTESTINFO   LPTVHITTESTINFO
#define   TV_HITTESTINFO     TVHITTESTINFO

typedef struct tagTVHITTESTINFO {
    POINT       pt;
    UINT        flags;
    HTREEITEM   hItem;
} TVHITTESTINFO, FAR *LPTVHITTESTINFO;

#define LPTV_SORTCB    LPTVSORTCB
#define   TV_SORTCB      TVSORTCB

typedef struct tagTVSORTCB
{
        HTREEITEM       hParent;
        PFNTVCOMPARE    lpfnCompare;
        LPARAM          lParam;
} TVSORTCB,  *LPTVSORTCB;


#define LPNM_TREEVIEW           LPNMTREEVIEW
#define NM_TREEVIEW             NMTREEVIEW

typedef struct tagNMTREEVIEW {
    NMHDR       hdr;
    UINT        action;
    TVITEM    itemOld;
    TVITEM    itemNew;
    POINT       ptDrag;
} NMTREEVIEW,  *LPNMTREEVIEW;

#define TV_DISPINFO             NMTVDISPINFO

typedef struct tagTVDISPINFO {
    NMHDR hdr;
    TVITEM item;
} NMTVDISPINFO,  *LPNMTVDISPINFO;

#pragma pack(1)
typedef struct tagTVKEYDOWN {
    NMHDR hdr;
    WORD wVKey;
    UINT flags;
} NMTVKEYDOWN,  *LPNMTVKEYDOWN;
#pragma pack()

#define NMTVCUSTOMDRAW_V3_SIZE CCSIZEOF_STRUCT(NMTVCUSTOMDRAW, clrTextBk)

typedef struct tagNMTVCUSTOMDRAW
{
    NMCUSTOMDRAW nmcd;
    COLORREF     clrText;
    COLORREF     clrTextBk;
    int iLevel;
} NMTVCUSTOMDRAW, *LPNMTVCUSTOMDRAW;


typedef struct tagNMTVGETINFOTIPA
{
    NMHDR hdr;
    LPSTR pszText;
    int cchTextMax;
    HTREEITEM hItem;
    LPARAM lParam;
} NMTVGETINFOTIPA, *LPNMTVGETINFOTIPA;

typedef struct tagNMTVGETINFOTIPW
{
    NMHDR hdr;
    LPWSTR pszText;
    int cchTextMax;
    HTREEITEM hItem;
    LPARAM lParam;
} NMTVGETINFOTIPW, *LPNMTVGETINFOTIPW;

#ifdef UNICODE
#define TVN_GETINFOTIP          TVN_GETINFOTIPW
#define NMTVGETINFOTIP          NMTVGETINFOTIPW
#define LPNMTVGETINFOTIP        LPNMTVGETINFOTIPW
#else
#define TVN_GETINFOTIP          TVN_GETINFOTIPA
#define NMTVGETINFOTIP          NMTVGETINFOTIPA
#define LPNMTVGETINFOTIP        LPNMTVGETINFOTIPA
#endif

typedef struct tagCOMBOBOXEXITEMA
{
    UINT mask;
    int iItem;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
    int iSelectedImage;
    int iOverlay;
    int iIndent;
    LPARAM lParam;
} COMBOBOXEXITEMA, *PCOMBOBOXEXITEMA;
typedef COMBOBOXEXITEMA CONST *PCCOMBOEXITEMA;


typedef struct tagCOMBOBOXEXITEMW
{
    UINT mask;
    int iItem;
    LPWSTR pszText;
    int cchTextMax;
    int iImage;
    int iSelectedImage;
    int iOverlay;
    int iIndent;
    LPARAM lParam;
} COMBOBOXEXITEMW, *PCOMBOBOXEXITEMW;
typedef COMBOBOXEXITEMW CONST *PCCOMBOEXITEMW;

#ifdef UNICODE
#define COMBOBOXEXITEM            COMBOBOXEXITEMW
#define PCOMBOBOXEXITEM           PCOMBOBOXEXITEMW
#define PCCOMBOBOXEXITEM          PCCOMBOBOXEXITEMW
#else
#define COMBOBOXEXITEM            COMBOBOXEXITEMA
#define PCOMBOBOXEXITEM           PCOMBOBOXEXITEMA
#define PCCOMBOBOXEXITEM          PCCOMBOBOXEXITEMA
#endif

typedef struct {
    NMHDR hdr;
    COMBOBOXEXITEMA ceItem;
} NMCOMBOBOXEXA, *PNMCOMBOBOXEXA;

typedef struct {
    NMHDR hdr;
    COMBOBOXEXITEMW ceItem;
} NMCOMBOBOXEXW, *PNMCOMBOBOXEXW;

#ifdef UNICODE
#define NMCOMBOBOXEX            NMCOMBOBOXEXW
#define PNMCOMBOBOXEX           PNMCOMBOBOXEXW
#else
#define NMCOMBOBOXEX            NMCOMBOBOXEXA
#define PNMCOMBOBOXEX           PNMCOMBOBOXEXA
#endif

typedef struct {
    NMHDR hdr;
    int   iItemid;
    WCHAR szText[CBEMAXSTRLEN];
}NMCBEDRAGBEGINW, *LPNMCBEDRAGBEGINW, *PNMCBEDRAGBEGINW;


typedef struct {
    NMHDR hdr;
    int   iItemid;
    char szText[CBEMAXSTRLEN];
}NMCBEDRAGBEGINA, *LPNMCBEDRAGBEGINA, *PNMCBEDRAGBEGINA;

#ifdef UNICODE
#define  NMCBEDRAGBEGIN NMCBEDRAGBEGINW
#define  LPNMCBEDRAGBEGIN LPNMCBEDRAGBEGINW
#define  PNMCBEDRAGBEGIN PNMCBEDRAGBEGINW
#else
#define  NMCBEDRAGBEGIN NMCBEDRAGBEGINA
#define  LPNMCBEDRAGBEGIN LPNMCBEDRAGBEGINA
#define  PNMCBEDRAGBEGIN PNMCBEDRAGBEGINA
#endif

typedef struct {
        NMHDR hdr;
        BOOL fChanged;
        int iNewSelection;
        WCHAR szText[CBEMAXSTRLEN];
        int iWhy;
} NMCBEENDEDITW, *LPNMCBEENDEDITW, *PNMCBEENDEDITW;

typedef struct {
        NMHDR hdr;
        BOOL fChanged;
        int iNewSelection;
        char szText[CBEMAXSTRLEN];
        int iWhy;
} NMCBEENDEDITA, *LPNMCBEENDEDITA,*PNMCBEENDEDITA;

#ifdef UNICODE
#define  NMCBEENDEDIT NMCBEENDEDITW
#define  LPNMCBEENDEDIT LPNMCBEENDEDITW
#define  PNMCBEENDEDIT PNMCBEENDEDITW
#else
#define  NMCBEENDEDIT NMCBEENDEDITA
#define  LPNMCBEENDEDIT LPNMCBEENDEDITA
#define  PNMCBEENDEDIT PNMCBEENDEDITA
#endif

#define TC_ITEMHEADERA         TCITEMHEADERA
#define TC_ITEMHEADERW         TCITEMHEADERW
#define TC_ITEMHEADER          TCITEMHEADER

typedef struct tagTCITEMHEADERA
{
    UINT mask;
    UINT lpReserved1;
    UINT lpReserved2;
    LPSTR pszText;
    int cchTextMax;
    int iImage;
} TCITEMHEADERA,  *LPTCITEMHEADERA;

typedef struct tagTCITEMHEADERW
{
    UINT mask;
    UINT lpReserved1;
    UINT lpReserved2;
    LPWSTR pszText;
    int cchTextMax;
    int iImage;
} TCITEMHEADERW,  *LPTCITEMHEADERW;

#ifdef UNICODE
#define  TCITEMHEADER          TCITEMHEADERW
#define  LPTCITEMHEADER        LPTCITEMHEADERW
#else
#define  TCITEMHEADER          TCITEMHEADERA
#define  LPTCITEMHEADER        LPTCITEMHEADERA
#endif


#define TC_ITEMA                TCITEMA
#define TC_ITEMW                TCITEMW
#define TC_ITEM                 TCITEM

typedef struct tagTCITEMA
{
    UINT mask;
    DWORD dwState;
    DWORD dwStateMask;
    LPSTR pszText;
    int cchTextMax;
    int iImage;

    LPARAM lParam;
} TCITEMA,  *LPTCITEMA;

typedef struct tagTCITEMW
{
    UINT mask;
    DWORD dwState;
    DWORD dwStateMask;
    LPWSTR pszText;
    int cchTextMax;
    int iImage;

    LPARAM lParam;
} TCITEMW,  *LPTCITEMW;

#ifdef UNICODE
#define  TCITEM                 TCITEMW
#define  LPTCITEM               LPTCITEMW
#else
#define  TCITEM                 TCITEMA
#define  LPTCITEM               LPTCITEMA
#endif


#define LPTC_HITTESTINFO        LPTCHITTESTINFO
#define TC_HITTESTINFO          TCHITTESTINFO

typedef struct tagTCHITTESTINFO
{
    POINT pt;
    UINT flags;
} TCHITTESTINFO,  * LPTCHITTESTINFO;

#pragma pack(1)

typedef struct tagTCKEYDOWN
{
    NMHDR hdr;
    WORD wVKey;
    UINT flags;
} NMTCKEYDOWN;
#pragma pack()


typedef struct {
        UINT cbSize;
        POINT pt;

        UINT uHit;   
        SYSTEMTIME st;
} MCHITTESTINFO, *PMCHITTESTINFO;

typedef struct tagNMSELCHANGE
{
    NMHDR           nmhdr;  

    SYSTEMTIME      stSelStart;
    SYSTEMTIME      stSelEnd;
} NMSELCHANGE,  * LPNMSELCHANGE;

typedef DWORD MONTHDAYSTATE, FAR * LPMONTHDAYSTATE;

typedef struct tagNMDAYSTATE
{
    NMHDR           nmhdr;  

    SYSTEMTIME      stStart;
    int             cDayState;

    LPMONTHDAYSTATE prgDayState; 
} NMDAYSTATE,  * LPNMDAYSTATE;

typedef NMSELCHANGE NMSELECT,  * LPNMSELECT;

typedef struct tagNMDATETIMECHANGE
{
    NMHDR       nmhdr;
    DWORD       dwFlags;    
    SYSTEMTIME  st;         
} NMDATETIMECHANGE,  * LPNMDATETIMECHANGE;

typedef struct tagNMDATETIMESTRINGA
{
    NMHDR      nmhdr;
    LPCSTR     pszUserString;  
    SYSTEMTIME st;             
    DWORD      dwFlags;        
} NMDATETIMESTRINGA,  * LPNMDATETIMESTRINGA;

typedef struct tagNMDATETIMESTRINGW
{
    NMHDR      nmhdr;
    LPCWSTR    pszUserString;  
    SYSTEMTIME st;             
    DWORD      dwFlags;        
} NMDATETIMESTRINGW,  * LPNMDATETIMESTRINGW;

#ifdef UNICODE
#define NMDATETIMESTRING        NMDATETIMESTRINGW
#define LPNMDATETIMESTRING      LPNMDATETIMESTRINGW
#else
#define NMDATETIMESTRING        NMDATETIMESTRINGA
#define LPNMDATETIMESTRING      LPNMDATETIMESTRINGA
#endif


typedef struct tagNMDATETIMEWMKEYDOWNA
{
    NMHDR      nmhdr;
    int        nVirtKey;  
    LPCSTR     pszFormat; 
    SYSTEMTIME st;        
} NMDATETIMEWMKEYDOWNA,  * LPNMDATETIMEWMKEYDOWNA;

typedef struct tagNMDATETIMEWMKEYDOWNW
{
    NMHDR      nmhdr;
    int        nVirtKey;  
    LPCWSTR    pszFormat; 
    SYSTEMTIME st;        
} NMDATETIMEWMKEYDOWNW,  * LPNMDATETIMEWMKEYDOWNW;

#ifdef UNICODE
#define NMDATETIMEWMKEYDOWN     NMDATETIMEWMKEYDOWNW
#define LPNMDATETIMEWMKEYDOWN   LPNMDATETIMEWMKEYDOWNW
#else
#define NMDATETIMEWMKEYDOWN     NMDATETIMEWMKEYDOWNA
#define LPNMDATETIMEWMKEYDOWN   LPNMDATETIMEWMKEYDOWNA
#endif


typedef struct tagNMDATETIMEFORMATA
{
    NMHDR nmhdr;
    LPCSTR  pszFormat;   
    SYSTEMTIME st;       
    LPCSTR pszDisplay;   
    CHAR szDisplay[64];  
} NMDATETIMEFORMATA,  * LPNMDATETIMEFORMATA;

typedef struct tagNMDATETIMEFORMATW
{
    NMHDR nmhdr;
    LPCWSTR pszFormat;   
    SYSTEMTIME st;       
    LPCWSTR pszDisplay;  
    WCHAR szDisplay[64]; 
} NMDATETIMEFORMATW,  * LPNMDATETIMEFORMATW;

#ifdef UNICODE
#define NMDATETIMEFORMAT        NMDATETIMEFORMATW
#define LPNMDATETIMEFORMAT      LPNMDATETIMEFORMATW
#else
#define NMDATETIMEFORMAT        NMDATETIMEFORMATA
#define LPNMDATETIMEFORMAT      LPNMDATETIMEFORMATA
#endif


typedef struct tagNMDATETIMEFORMATQUERYA
{
    NMHDR nmhdr;
    LPCSTR pszFormat;  
    SIZE szMax;        
} NMDATETIMEFORMATQUERYA,  * LPNMDATETIMEFORMATQUERYA;

typedef struct tagNMDATETIMEFORMATQUERYW
{
    NMHDR nmhdr;
    LPCWSTR pszFormat; 
    SIZE szMax;        
} NMDATETIMEFORMATQUERYW,  * LPNMDATETIMEFORMATQUERYW;

#ifdef UNICODE
#define NMDATETIMEFORMATQUERY   NMDATETIMEFORMATQUERYW
#define LPNMDATETIMEFORMATQUERY LPNMDATETIMEFORMATQUERYW
#else
#define NMDATETIMEFORMATQUERY   NMDATETIMEFORMATQUERYA
#define LPNMDATETIMEFORMATQUERY LPNMDATETIMEFORMATQUERYA
#endif

typedef struct tagNMIPADDRESS
{
        NMHDR hdr;
        int iField;
        int iValue;
} NMIPADDRESS, *LPNMIPADDRESS;

#pragma pack(1)
typedef struct {
    NMHDR hdr;
    WORD fwKeys;            
    RECT rcParent;          
    int  iDir;              
    int  iXpos;             
    int  iYpos;             
    int  iScroll;           
}NMPGSCROLL, *LPNMPGSCROLL;
#pragma pack()

typedef struct {
    NMHDR   hdr;
    DWORD   dwFlag;
    int     iWidth;
    int     iHeight;
}NMPGCALCSIZE, *LPNMPGCALCSIZE;

typedef struct tagTRACKMOUSEEVENT {
    DWORD cbSize;
    DWORD dwFlags;
    HWND  hwndTrack;
    DWORD dwHoverTime;
} TRACKMOUSEEVENT, *LPTRACKMOUSEEVENT;

#pragma pack()

