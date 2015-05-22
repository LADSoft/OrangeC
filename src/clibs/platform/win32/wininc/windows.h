/* 
   windows.h

   Include this file if you wish to use the Windows32 API Library

   Copyright (C) 1996 Free Software Foundation

   Author:  Scott Christley <scottc@net-community.com>
   Date: 1996
   
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

--------------------------------------------------------------------------

   These WIN32 library headers have been extensively modified to work with the
   CC386 compiler.  However, they are still covered by GNU licensing.
   The GNU licensing covers only the WIN32 headers and does not apply to
   the rest of the run time library supplied with CC386.  DAL July 2000
*/ 

#ifndef _GNU_H_WINDOWS_H
#define _GNU_H_WINDOWS_H

#ifndef RC_INVOKED
#include <limits.h>
#include <stdarg.h>

/* microsoft includes these in WINNT.H and we have to too for compatibility */
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* Base definitions */
#include <win32/Base.h>

/* WIN32 messages */
#include <win32/Messages.h>

#endif /* RC_INVOKED */

/* WIN32 definitions */
#include <win32/Defines.h>

#ifndef RC_INVOKED
/* WIN32 structures */
#include <win32/Structur.h>

/* WIN32 functions */
#include <win32/Function.h>

/* WIN32 error codes */
#include <win32/Errors.h>

/* How do we get the VM page size on NT? */
#ifndef vm_page_size
#define vm_page_size 4096
#endif

#endif /* RC_INVOKED */

/* common dialogs */
#include <commdlg.h>

#ifdef RC_INVOKED
#include <commctrl.h>
#endif

#endif /* _GNU_H_WINDOWS_H */
