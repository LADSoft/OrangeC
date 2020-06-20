/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#    ifndef _SIZE_T
#        define _SIZE_T
#        define _USING_TYPES_H_SIZE_T
typedef unsigned size_t;
typedef int ssize_t;
#    endif

#    ifndef _TIME_T
#        define _USING_TYPES_H_TIME_T
#        define _TIME_T
typedef long time_t;
#    endif

#    ifdef __CRTDLL_DLL
typedef short dev_t;
#    else
typedef int dev_t;
#    endif
typedef short ino_t;
typedef short mode_t;
typedef int nlink_t;
typedef short uid_t;
typedef short gid_t;
typedef long off_t;
typedef long pid_t;
typedef long pthread_t;
typedef long pthread_attr_t;

#ifdef MY_UNSIGNED_TYPES
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned u_int32_t;

typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
#endif

#endif /* __TYPES_H */
