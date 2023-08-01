/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

/*  stddef.h

    Definitions for common types, and NULL

*/

#ifndef __STDDEF_H
#define __STDDEF_H

#ifndef __DEFS_H__
#    include <_defs.h>
#endif

#if defined(__cplusplus) || __STDC_VERSION__ >= 201112L
typedef long double max_align_t;
#endif

#ifndef RC_INVOKED
#ifndef _PTRDIFF_T
#    define _PTRDIFF_T
typedef int ptrdiff_t;
#endif

#ifndef _SIZE_T
#    define _SIZE_T
typedef unsigned size_t;
typedef int ssize_t;
#endif

#ifndef __ERRNO_T_DEFINED
#define __ERRNO_T_DEFINED
typedef int errno_t;
#endif

#ifndef _WCHAR_T_DEFINED
#    define _WCHAR_T_DEFINED
#    ifndef __cplusplus
typedef unsigned short wchar_t;
#    endif
#endif
#endif
/*
#ifndef FAR
#define FAR _FAR
#endif
*/
#ifndef NULL
#    define NULL 0
#endif

#ifndef offsetof
#    define offsetof(s_name, m_name) (size_t) __offsetof(s_name, m_name)
#endif

#ifndef RC_INVOKED
#ifndef _WCTYPE_T_DEFINED
typedef unsigned int wctype_t;
#    define _WCTYPE_T_DEFINED
#endif

#ifndef _WINT_T_DEFINED
#    define _WINT_T_DEFINED
typedef unsigned int wint_t;
#endif
#endif

#if __STDC_VERSION__ >= 202311L
typedef typeof(nullptr) nullptr_t;
#endif

#endif /* __STDDEF_H */