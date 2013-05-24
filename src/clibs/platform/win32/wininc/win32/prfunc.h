/* 
   prfunc.h

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
WINBOOL        PASCAL WINBASEAPI DestroyPropertySheetPage(HPROPSHEETPAGE);

#ifdef UNICODE
HPROPSHEETPAGE PASCAL WINBASEAPI CreatePropertySheetPageW(LPCPROPSHEETPAGE);
int            PASCAL WINBASEAPI PropertySheetW(LPCPROPSHEETHEADER);
#define CreatePropertySheetPage  CreatePropertySheetPageW
#define PropertySheet            PropertySheetW
#else
HPROPSHEETPAGE PASCAL WINBASEAPI CreatePropertySheetPageA(LPCPROPSHEETPAGE);
int            PASCAL WINBASEAPI PropertySheetA(LPCPROPSHEETHEADER);
#define CreatePropertySheetPage  CreatePropertySheetPageA
#define PropertySheet            PropertySheetA
#endif

#ifdef __cplusplus
} ;
#endif