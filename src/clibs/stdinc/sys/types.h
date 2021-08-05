/* Software License Agreement
 * 
 *     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

/*  types.h

    Types for dealing with time.

*/

#if !defined(__TYPES_H)
#    define __TYPES_H

#ifndef RC_INVOKED
#    ifndef _SIZE_T
#        define _SIZE_T
#        define _USING_TYPES_H_SIZE_T
typedef unsigned size_t;
#        ifndef	_SSIZE_T_
#        define _SSIZE_T
typedef int ssize_t;
#        endif
#    endif

#    ifndef _TIME_T
#        define _USING_TYPES_H_TIME_T
#        define _TIME_T
typedef long time_t;
#    endif

#ifndef __dev_t_defined
#define __dev_t_defined
#    ifdef __CRTDLL_DLL
typedef short dev_t;
#    else
typedef int dev_t;
#    endif
#endif

#if !defined(__ino_t_defined) && !defined(_INO_T_)
#define __ino_t_defined
#define _INO_T_
typedef short ino_t;
#endif

#if !defined(__mode_t_defined) && !defined(_MODE_T_)
#define __mode_t_defined
#define _MODE_T_
typedef short mode_t;
#endif

#ifndef __nlink_t_defined
#define __nlink_t_defined
typedef int nlink_t;
#endif

#ifndef __uid_t_defined
#define __uid_t_defined
typedef short uid_t;
#endif

#ifndef __gid_t_defined
#define __gid_t_defined
typedef short gid_t;
#endif

#if !defined(_OFFT_DEFINED) && !defined(__off_t_defined)
#define _OFFT_DEFINED
#define __off_t_defined
typedef long off_t;
#endif

#if !defined(__pid_t_defined) && !defined(_PID_T_)
#define __pid_t_defined
#define _PID_T_
typedef long pid_t;
#endif

typedef long pthread_t;
typedef long pthread_attr_t;

#    ifdef MY_UNSIGNED_TYPES
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned u_int32_t;

typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
#    endif
#endif

#endif /* __TYPES_H */
