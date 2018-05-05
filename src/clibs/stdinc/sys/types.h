/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

/*  types.h

    Types for dealing with time.

*/

#if !defined(__TYPES_H)
#define __TYPES_H

#ifdef __cplusplus
namespace __STD_NS__ {
#endif /* __cplusplus */

#ifndef _SIZE_T
#define _SIZE_T
#define _USING_TYPES_H_SIZE_T
typedef unsigned size_t;
typedef int ssize_t;
#endif

#ifndef  _TIME_T
#define _USING_TYPES_H_TIME_T
typedef long time_t;
#ifdef __cplusplus
#  define _TIME_T __STD_NS_QUALIFIER time_t
#else
#  define _TIME_T time_t
#endif /* __cplusplus */
#endif

#ifdef __CRTDLL_DLL
typedef short dev_t;
#else
typedef int   dev_t;
#endif
typedef short ino_t;
typedef short mode_t;
typedef int   nlink_t;
typedef short uid_t;
typedef short gid_t;
typedef long  off_t;


#ifdef __cplusplus
} ;
#endif

#endif  /* __TYPES_H */

#if defined( __cplusplus) && !defined(__USING_CNAME__) && !defined(__TYPES_H_USING_LIST)
#define __TYPES_H_USING_LIST
#ifdef _USING_TYPES_H_TIME_T
using __STD_NS_QUALIFIER time_t;
#endif
using __STD_NS_QUALIFIER dev_t;
using __STD_NS_QUALIFIER ino_t;
using __STD_NS_QUALIFIER mode_t;
using __STD_NS_QUALIFIER nlink_t;
using __STD_NS_QUALIFIER uid_t;
using __STD_NS_QUALIFIER gid_t;
using __STD_NS_QUALIFIER off_t;
#ifdef _USING_TYPES_H_SIZE_T
using __STD_NS_QUALIFIER size_t;
using __STD_NS_QUALIFIER ssize_t ;
#endif
#endif
