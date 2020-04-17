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

/*  malloc.h

    memory management functions and variables.

*/

#ifndef __MALLOC_H
#define __MALLOC_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#define _HEAPEMPTY      1
#define _HEAPOK         2
#define _FREEENTRY      3
#define _USEDENTRY      4
#define _HEAPEND        5
#define _HEAPCORRUPT    -1
#define _BADNODE        -2
#define _BADVALUE       -3

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CC386__
#define _fmalloc(size) farmalloc(size)
#define _ffree(size)  farfree(size)
#endif

void        _RTL_FUNC _IMPORT _MSIL_RTL *calloc(size_t __nitems, size_t __size);
void        _RTL_FUNC _IMPORT _MSIL_RTL free(void *__block);
void        _RTL_FUNC _IMPORT _MSIL_RTL *malloc(size_t __size);
void        _RTL_FUNC _IMPORT _MSIL_RTL *realloc(void *__block, size_t __size);
#ifdef __CC386__
void        _RTL_FUNC _IMPORT _FAR *farmalloc(size_t __size) ;
void        _RTL_FUNC _IMPORT farfree(void _FAR *__block) ;
#endif

#undef _alloca
#undef alloca
#define _alloca(x) __alloca((x))
#define alloca(x) __alloca((x))

#ifdef __cplusplus
} ;
#endif

#endif  /* __MALLOC_H */
