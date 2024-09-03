/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

/*
    assert.h
*/

#pragma GCC system_header

#ifndef __ASSERT_H__
#    define __ASSERT_H__

#    include <stddef.h>

#    ifdef __cplusplus
extern "C"
{
#    endif

#ifndef RC_INVOKED
    void _RTL_FUNC _IMPORT __assertfail(const char* __who, const char* __file, int __line, const char* __func, const char* __msg);
#endif
#    ifdef __cplusplus
};
#    endif

#    undef assert
#    if !defined(NDEBUG) && (!defined(__MSIL__) || defined(__MANAGED__))
#        if __STDC_VERSION__ >= 199901L
#            define assert(p) ((p) ? (void)0 : (void)__assertfail("Assertion failed", __FILE__, __LINE__, __func__, #            p))
#        else
#            define assert(p) ((p) ? (void)0 : (void)__assertfail("Assertion failed", __FILE__, __LINE__, 0, #            p))
#        endif
#    else
#        define assert(p) ((void)0)
#    endif

#    if !defined(__cplusplus) && __STDC_VERSION__ >= 201112L
#        define static_assert _Static_assert
#    endif

#endif /* __ASSERT_H__ */
