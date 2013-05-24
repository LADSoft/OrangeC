/* 
   cdmacro.h

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
#ifndef CDSIZEOF_STRUCT
#define CDSIZEOF_STRUCT(structname, member) \
    (((int)((LPBYTE)(&((structname*)0)->member) - ((LPBYTE)((structname*)0)))) + sizeof(((structname*)0)->member))
#endif

#define CommDlg_OpenSave_GetFilePathA(_hdlg, _psz, _cbmax) \
    (int)SNDMSG(_hdlg, CDM_GETFILEPATH, (WPARAM)_cbmax, (LPARAM)(LPSTR)_psz)
#define CommDlg_OpenSave_GetFilePathW(_hdlg, _psz, _cbmax) \
    (int)SNDMSG(_hdlg, CDM_GETFILEPATH, (WPARAM)_cbmax, (LPARAM)(LPWSTR)_psz)
#define CommDlg_OpenSave_GetFolderIDList(_hdlg, _pidl, _cbmax) \
    (int)SNDMSG(_hdlg, CDM_GETFOLDERIDLIST, (WPARAM)_cbmax, (LPARAM)(LPVOID)_pidl)
#define CommDlg_OpenSave_GetFolderPathA(_hdlg, _psz, _cbmax) \
    (int)SNDMSG(_hdlg, CDM_GETFOLDERPATH, (WPARAM)_cbmax, (LPARAM)(LPSTR)_psz)
#define CommDlg_OpenSave_GetFolderPathW(_hdlg, _psz, _cbmax) \
    (int)SNDMSG(_hdlg, CDM_GETFOLDERPATH, (WPARAM)_cbmax, (LPARAM)(LPWSTR)_psz)
#define CommDlg_OpenSave_GetSpecA(_hdlg, _psz, _cbmax) \
    (int)SNDMSG(_hdlg, CDM_GETSPEC, (WPARAM)_cbmax, (LPARAM)(LPSTR)_psz)
#define CommDlg_OpenSave_GetSpecW(_hdlg, _psz, _cbmax) \
    (int)SNDMSG(_hdlg, CDM_GETSPEC, (WPARAM)_cbmax, (LPARAM)(LPWSTR)_psz)
#define CommDlg_OpenSave_HideControl(_hdlg, _id) \
    (void)SNDMSG(_hdlg, CDM_HIDECONTROL, (WPARAM)_id, 0)
#define CommDlg_OpenSave_SetControlText(_hdlg, _id, _text) \
    (void)SNDMSG(_hdlg, CDM_SETCONTROLTEXT, (WPARAM)_id, (LPARAM)(LPSTR)_text)
#define CommDlg_OpenSave_SetDefExt(_hdlg, _pszext) \
    (void)SNDMSG(_hdlg, CDM_SETDEFEXT, 0, (LPARAM)(LPSTR)_pszext)

#ifdef UNICODE
#define CommDlg_OpenSave_GetSpec  CommDlg_OpenSave_GetSpecW
#else
#define CommDlg_OpenSave_GetSpec  CommDlg_OpenSave_GetSpecA
#endif /* !UNICODE */

#ifdef UNICODE
#define CommDlg_OpenSave_GetFilePath  CommDlg_OpenSave_GetFilePathW
#else
#define CommDlg_OpenSave_GetFilePath  CommDlg_OpenSave_GetFilePathA
#endif /* !UNICODE */

#ifdef UNICODE
#define CommDlg_OpenSave_GetFolderPath  CommDlg_OpenSave_GetFolderPathW
#else
#define CommDlg_OpenSave_GetFolderPath  CommDlg_OpenSave_GetFolderPathA
#endif /* !UNICODE */
