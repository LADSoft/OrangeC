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
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#ifndef __SEARCH_H
#define __SEARCH_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif
void *      _RTL_FUNC _IMPORT bsearch(const void * __key, const void * __base,
                           size_t __nelem, size_t __width,
                           int (*fcmp)(const void *, const void *));
void *      _RTL_FUNC _IMPORT lfind(const void * __key, const void * __base,
                                size_t * __num, size_t __width,
                                int (*fcmp)(const void *, const void *));
void *      _RTL_FUNC _IMPORT lsearch(const void * __key, void * __base,
                                size_t * __num, size_t __width,
                                int (*fcmp)(const void *, const void *));
void        _RTL_FUNC _IMPORT qsort(void * __base, size_t __nelem, size_t __width,
                         int (*__fcmp)(const void *, const void *));

void * _RTL_FUNC _IMPORT _lfind(const void *, const void *, unsigned int *, unsigned int,
        int (*)(const void *, const void *));
void * _RTL_FUNC _IMPORT _lsearch(const void *, void  *, unsigned int *, unsigned int,
                                int (*)(const void *, const void *));
                          
#ifdef __cplusplus
}
}
#endif

#endif  /* __SEARCH_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__SEARCH_H_USING_LIST)
#define __SEARCH_H_USING_LIST
using __STD_NS_QUALIFIER bsearch;
using __STD_NS_QUALIFIER lfind;
using __STD_NS_QUALIFIER lsearch;
using __STD_NS_QUALIFIER _lfind;
using __STD_NS_QUALIFIER _lsearch;
using __STD_NS_QUALIFIER qsort;
#endif /* __USING_CNAME__ */
