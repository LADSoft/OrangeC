/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

/*  memory.h

    Memory manipulation functions

*/
#pragma GCC system_header

#if !defined(__MEM_H)
#    define __MEM_H

#        include <stddef.h>

#ifndef RC_INVOKED
#    ifdef __cplusplus
extern "C"
{
#    endif

    void* _RTL_FUNC _IMPORT memccpy(void* __dest, const void* __src, int __c, size_t __n);
    int _RTL_FUNC _IMPORT memcmp(const void* __s1, const void* __s2, size_t __n);
    void* _RTL_FUNC _IMPORT memcpy(void* __dest, const void* __src, size_t __n);
    int _RTL_FUNC _IMPORT memicmp(const void* __s1, const void* __s2, size_t __n);
    void* _RTL_FUNC _IMPORT memmove(void* __dest, const void* __src, size_t __n);
    void* _RTL_FUNC _IMPORT memset(void* __s, int __c, size_t __n);

#if defined(__cplusplus) || __STDC_VERSION__ >= 202311
    const void* _RTL_INTRINS _IMPORT memchr(const void* __s, int __c, size_t __n);
#else
    void* _RTL_INTRINS _IMPORT memchr(const void* __s, int __c, size_t __n);
#endif

    int _RTL_FUNC _IMPORT _memicmp(const void*, const void*, unsigned int);
    void* _RTL_FUNC _IMPORT _memccpy(void*, const void*, int, unsigned int);

#    ifdef __cplusplus
};
#    endif
#endif
#endif /* __MEM_H */
