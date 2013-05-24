/* 
   cdfunc.h

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

#ifdef __cplusplus
extern "C" {
#endif

WINBOOL  PASCAL WINBASEAPI     GetOpenFileNameW(LPOPENFILENAME);
WINBOOL  PASCAL WINBASEAPI     GetSaveFileNameW(LPOPENFILENAME);
short PASCAL WINBASEAPI     GetFileTitleW(LPCWSTR, LPWSTR, WORD);
WINBOOL  PASCAL WINBASEAPI ChooseColorW(LPCHOOSECOLOR);
HWND  PASCAL WINBASEAPI    FindTextW(LPFINDREPLACE);
WINBOOL PASCAL WINBASEAPI ChooseFontW(LPCHOOSEFONT);
WINBOOL  PASCAL WINBASEAPI  PrintDlgW(LPPRINTDLG);
HRESULT  PASCAL WINBASEAPI  PrintDlgExW(LPPRINTDLGEX);
WINBOOL PASCAL WINBASEAPI PageSetupDlgW( LPPAGESETUPDLG );

WINBOOL  PASCAL WINBASEAPI     GetOpenFileNameA(LPOPENFILENAME);
WINBOOL  PASCAL WINBASEAPI     GetSaveFileNameA(LPOPENFILENAME);
short PASCAL WINBASEAPI     GetFileTitleA(LPCSTR, LPSTR, WORD);
WINBOOL  PASCAL WINBASEAPI ChooseColorA(LPCHOOSECOLOR);
HWND  PASCAL WINBASEAPI    FindTextA(LPFINDREPLACE);
WINBOOL PASCAL WINBASEAPI ChooseFontA(LPCHOOSEFONT);
WINBOOL  PASCAL WINBASEAPI  PrintDlgA(LPPRINTDLG);
HRESULT  PASCAL WINBASEAPI  PrintDlgExA(LPPRINTDLGEX);
WINBOOL PASCAL WINBASEAPI PageSetupDlgA( LPPAGESETUPDLG );

#ifdef UNICODE
#define GetOpenFileName  GetOpenFileNameW
#define GetSaveFileName  GetSaveFileNameW
#define GetFileTitle  GetFileTitleW
#define ChooseColor  ChooseColorW
#define FindText  FindTextW
#define ChooseFont  ChooseFontW
#define PrintDlg  PrintDlgW
#define PrintDlgEx  PrintDlgExW
#define PageSetupDlg  PageSetupDlgW
#else
#define GetOpenFileName  GetOpenFileNameA
#define GetSaveFileName  GetSaveFileNameA
#define GetFileTitle  GetFileTitleA
#define ChooseColor  ChooseColorA
#define FindText  FindTextA
#define ChooseFont  ChooseFontA
#define PrintDlg  PrintDlgA
#define PrintDlgEx  PrintDlgExA
#define PageSetupDlg  PageSetupDlgA
#endif


#ifndef _MAC
HWND  PASCAL WINBASEAPI    ReplaceTextW(LPFINDREPLACE);
HWND  PASCAL WINBASEAPI    ReplaceTextA(LPFINDREPLACE);
#ifdef UNICODE
#define ReplaceText  ReplaceTextW
#else
#define ReplaceText  ReplaceTextA
#endif
#else

HWND  PASCAL WINBASEAPI    AfxReplaceTextW(LPFINDREPLACE);
HWND  PASCAL WINBASEAPI    AfxReplaceTextA(LPFINDREPLACE);
#ifdef UNICODE
#define AfxReplaceText  AfxReplaceTextW
#else
#define AfxReplaceText  AfxReplaceTextA
#endif
#endif

DWORD PASCAL WINBASEAPI     CommDlgExtendedError(void);

#ifdef __cplusplus
} ;
#endif

