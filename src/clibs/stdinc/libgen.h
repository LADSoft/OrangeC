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

#pragma GCC system_header

#ifndef __LIBGEN_H
#define __LIBGEN_H

#    include <stddef.h>

#ifndef RC_INVOKED
#ifdef __cplusplus
extern "C"
{
#endif

    char* _RTL_FUNC basename(char* ZSTR);
    char* _RTL_FUNC dirname(char* ZSTR);

    /*
    extern char *ZSTR  __loc1;

    char  *regcmp(const char *ZSTR , ...);
    char  *regex(const char *ZSTR , const char *ZSTR , ...);
    */

#ifdef __cplusplus
};
#endif
#endif

#endif /* __LIBGEN_H */
