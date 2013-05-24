/* 
   mmsmacro.h

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
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
        ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
        ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define mmioFOURCC(ch0, ch1, ch2, ch3)  MAKEFOURCC(ch0, ch1, ch2, ch3)
#define MKFOURCC mmioFOURCC

#define MCI_MSF_MINUTE(t) ((BYTE)(t))
#define MCI_MSF_SECOND(t) ((BYTE)(((WORD)(t))>>8))
#define MCI_MSF_FRAME(t) ((BYTE)((t)>>16))
#define MCI_MAKE_MSF(m,s,f) ((DWORD)(((BYTE)(m)|((WORD)(s)<<8))|(((DWORD)(BYTE)(f))<<16)))
#define MCI_TMSF_TRACK(t) ((BYTE)(t))
#define MCI_TMSF_MINUTE(t) ((BYTE)(((WORD)(t))>>8))
#define MCI_TMSF_SECOND(t) ((BYTE)((t)>>16))
#define MCI_TMSF_FRAME(t) ((BYTE)((t)>>24))
#define MCI_MAKE_TMSF(t,m,s,f) ((DWORD)(((BYTE)(t)|((WORD)(m)<<8))|(((DWORD)(BYTE)(s)|((WORD)(f)<<8))<<16)))
#define MCI_HMS_HOUR(t) ((BYTE)(t))
#define MCI_HMS_MINUTE(t) ((BYTE)(((WORD)(t))>>8))
#define MCI_HMS_SECOND(t) ((BYTE)((t)>>16))
#define MCI_MAKE_HMS(h,m,s) ((DWORD)(((BYTE)(h)|((WORD)(m)<<8))|(((DWORD)(BYTE)(s))<<16)))
