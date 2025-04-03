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

/*  malloc.h

    memory management functions and variables.

*/
#pragma GCC system_header

#ifndef __MALLOC_H
#define __MALLOC_H

#    include <stddef.h>

#define _HEAPEMPTY 1
#define _HEAPOK 2
#define _FREEENTRY 3
#define _USEDENTRY 4
#define _HEAPEND 5
#define _HEAPCORRUPT -1
#define _BADNODE -2
#define _BADVALUE -3

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __CC386__
#    define _fmalloc(size) farmalloc(size)
#    define _ffree(size) farfree(size)
#endif
#ifndef RC_INVOKED

    void _RTL_FUNC _IMPORT _MSIL_RTL* calloc(size_t __nitems, size_t __size);
    void _RTL_FUNC _IMPORT _MSIL_RTL free(void* __block);
    void _RTL_FUNC _IMPORT _MSIL_RTL* malloc(size_t __size);
    void _RTL_FUNC _IMPORT _MSIL_RTL* realloc(void* __block, size_t __size);
    void * _RTL_FUNC _IMPORT _mm_malloc(size_t, size_t);
    void _RTL_FUNC _IMPORT _mm_free(void *);
    void* _RTL_FUNC _IMPORT aligned_alloc(size_t __align, size_t __size);
    void* _RTL_FUNC _IMPORT _aligned_malloc(size_t __align, size_t __size);
    void _RTL_FUNC _IMPORT _aligned_free(void* __block);
#ifdef __CC386__
    void _RTL_FUNC _IMPORT _FAR* farmalloc(size_t __size);
    void _RTL_FUNC _IMPORT farfree(void _FAR* __block);
#endif
#endif

#undef _alloca
#undef alloca
#define _alloca(x) __alloca((x))
#define alloca(x) __alloca((x))

#ifdef __cplusplus
};
#endif

#endif /* __MALLOC_H */
