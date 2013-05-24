/* 
   prmsg.h

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
#define PSM_ADDPAGE (1127)
#define PSM_APPLY (1134)
#define PSM_CANCELTOCLOSE (1131)
#define PSM_CHANGED (1128)
#define PSM_GETCURRENTPAGEHWND (1142)
#define PSM_GETRESULT (1159)
#define PSM_GETTABCONTROL (1140)
#define PSM_HWNDTOINDEX (1153)
#define PSM_IDTOINDEX (1157)
#define PSM_INDEXTOHWND (1154)
#define PSM_INDEXTOID (1158)
#define PSM_INDEXTOPAGE (1156)
#define PSM_INSERTPAGE (1143)
#define PSM_ISDIALOGMESSAGE (1141)
#define PSM_PAGETOINDEX (1155)
#define PSM_PRESSBUTTON (1137)
#define PSM_QUERYSIBLINGS (1132)
#define PSM_REBOOTSYSTEM (1130)
#define PSM_RECALCPAGESIZES (1160)
#define PSM_REMOVEPAGE (1126)
#define PSM_RESTARTWINDOWS (1129)
#define PSM_SETCURSEL (1125)
#define PSM_SETCURSELID (1138)
#define PSM_SETFINISHTEXTA (1139)
#define PSM_SETFINISHTEXTW (1145)
#define PSM_SETHEADERSUBTITLEA (1151)
#define PSM_SETHEADERSUBTITLEW (1152)
#define PSM_SETHEADERTITLEA (1149)
#define PSM_SETHEADERTITLEW (1150)
#define PSM_SETTITLEA (1135)
#define PSM_SETTITLEW (1144)
#define PSM_SETWIZBUTTONS (1136)
#define PSM_UNCHANGED (1133)

#ifdef UNICODE
#define PSM_SETFINISHTEXT PSM_SETFINISHTEXTW
#define PSM_SETHEADERSUBTITLE PSM_SETHEADERSUBTITLEW
#define PSM_SETHEADERTITLE PSM_SETHEADERTITLEW
#define PSM_SETTITLE PSM_SETTITLEW
#else
#define PSM_SETFINISHTEXT PSM_SETFINISHTEXTA
#define PSM_SETHEADERSUBTITLE PSM_SETHEADERSUBTITLEA
#define PSM_SETHEADERTITLE PSM_SETHEADERTITLEA
#define PSM_SETTITLE PSM_SETTITLEA
#endif

#define PSN_APPLY (-202)
#define PSN_GETOBJECT (-210)
#define PSN_HELP (-205)
#define PSN_KILLACTIVE (-201)
#define PSN_QUERYCANCEL (-209)
#define PSN_QUERYINITIALFOCUS (-213)
#define PSN_RESET (-203)
#define PSN_SETACTIVE (-200)
#define PSN_TRANSLATEACCELERATOR (-212)
#define PSN_WIZBACK (-206)
#define PSN_WIZFINISH (-208)
#define PSN_WIZNEXT (-207)