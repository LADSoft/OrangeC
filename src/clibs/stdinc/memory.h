/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

/*  memory.h

    Memory manipulation functions

*/

#if !defined(__MEM_H)
#    define __MEM_H

#    ifndef __STDDEF_H
#        include <stddef.h>
#    endif

#    ifdef __cplusplus
namespace __STD_NS__
{
extern "C"
{
#    endif

    void* _RTL_FUNC _IMPORT memccpy(void* __dest, const void* __src, int __c, size_t __n);
    int _RTL_FUNC _IMPORT memcmp(const void* __s1, const void* __s2, size_t __n);
    void* _RTL_FUNC _IMPORT memcpy(void* __dest, const void* __src, size_t __n);
    int _RTL_FUNC _IMPORT memicmp(const void* __s1, const void* __s2, size_t __n);
    void* _RTL_FUNC _IMPORT memmove(void* __dest, const void* __src, size_t __n);
    void* _RTL_FUNC _IMPORT memset(void* __s, int __c, size_t __n);

    void* _RTL_FUNC _IMPORT memchr(const void* __s, int __c, size_t __n);

    int _RTL_FUNC _IMPORT _memicmp(const void*, const void*, unsigned int);
    void* _RTL_FUNC _IMPORT _memccpy(void*, const void*, int, unsigned int);

#    ifdef __cplusplus
};
};
#    endif

#endif /* __MEM_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__MEMORY_H_USING_LIST)
#    define __MEMORY_H_USING_LIST
using __STD_NS_QUALIFIER memccpy;
using __STD_NS_QUALIFIER memcmp;
using __STD_NS_QUALIFIER memcpy;
using __STD_NS_QUALIFIER memicmp;
using __STD_NS_QUALIFIER memmove;
using __STD_NS_QUALIFIER memset;
using __STD_NS_QUALIFIER memchr;
#endif
