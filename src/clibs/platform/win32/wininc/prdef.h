/* 
   prdef.h

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
#define ID_PSREBOOTSYSTEM (ID_PSRESTARTWINDOWS | 1)
#define ID_PSRESTARTWINDOWS (2)

#define MAXPROPPAGES (100)

#define PROP_LG_CXDLG (252)
#define PROP_LG_CYDLG (218)
#define PROP_MED_CXDLG (227)
#define PROP_MED_CYDLG (215)
#define PROP_SM_CXDLG (212)
#define PROP_SM_CYDLG (188)

#define PSBTN_APPLYNOW (4)
#define PSBTN_BACK (0)
#define PSBTN_CANCEL (5)
#define PSBTN_FINISH (2)
#define PSBTN_HELP (6)
#define PSBTN_MAX (6)
#define PSBTN_NEXT (1)
#define PSBTN_OK (3)

#define PSCB_INITIALIZED (1)
#define PSCB_PRECREATE (2)

#define PSH_DEFAULT (0)
#define PSH_HASHELP (512)
#define PSH_HEADER (524288)
#define PSH_MODELESS (1024)
#define PSH_NOAPPLYNOW (128)
#define PSH_NOCONTEXTHELP (33554432)
#define PSH_PROPSHEETPAGE (8)
#define PSH_PROPTITLE (1)
#define PSH_RTLREADING (2048)
#define PSH_STRETCHWATERMARK (262144)
#define PSH_USECALLBACK (256)
#define PSH_USEHBMHEADER (1048576)
#define PSH_USEHBMWATERMARK (65536)
#define PSH_USEHICON (2)
#define PSH_USEHPLWATERMARK (131072)
#define PSH_USEICONID (4)
#define PSH_USEPAGELANG (2097152)
#define PSH_USEPSTARTPAGE (64)
#define PSH_WATERMARK (32768)
#define PSH_WIZARD (32)
#define PSH_WIZARD97 (8192)
/*#define PSH_WIZARD97 (16777216) */
#define PSH_WIZARDCONTEXTHELP (4096)
#define PSH_WIZARDHASFINISH (16)
#define PSH_WIZARD_LITE (4194304)

#define PSNRET_INVALID (1)
#define PSNRET_INVALID_NOCHANGEPAGE (2)
#define PSNRET_MESSAGEHANDLED (3)
#define PSNRET_NOERROR (0)

#define PSPCB_ADDREF (0)
#define PSPCB_CREATE (2)
#define PSPCB_RELEASE (1)

#define PSP_DEFAULT (0)
#define PSP_DLGINDIRECT (1)
#define PSP_HASHELP (32)
#define PSP_HIDEHEADER (2048)
#define PSP_PREMATURE (1024)
#define PSP_RTLREADING (16)
#define PSP_USECALLBACK (128)
#define PSP_USEHEADERSUBTITLE (8192)
#define PSP_USEHEADERTITLE (4096)
#define PSP_USEHICON (2)
#define PSP_USEICONID (4)
#define PSP_USEREFPARENT (64)
#define PSP_USETITLE (8)

#define PSWIZB_BACK (1)
#define PSWIZB_DISABLEDFINISH (8)
#define PSWIZB_FINISH (4)
#define PSWIZB_NEXT (2)

#define WIZ_BODYCX (184)
#define WIZ_BODYX (92)
#define WIZ_CXBMP (80)
#define WIZ_CXDLG (276)
#define WIZ_CYDLG (140)