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

/*  regex.h

    Struct and function declarations for dealing with regular expressions

*/

#pragma GCC system_header

#ifndef __REGEX_H
#define __REGEX_H

#    include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define REG_EXTENDED 1
#define REG_ICASE 2
#define REG_NOSUB 4
#define REG_NEWLINE 8

#define REG_NOTBOL 1
#define REG_NOTEOL 2

#define REG_NOMATCH 1
#define REG_BADPAT 2
#define REG_ECOLLATE 3
#define REG_ECTYPE 4
#define REG_EESCAPE 5
#define REG_ESUBREG 6
#define REG_EBRACK 7
#define REG_ENOSYS 8
#define REG_EPAREN 9
#define REG_EBRACE 10
#define REG_BADBR 11
#define REG_ERANGE 12
#define REG_ESPACE 13
#define REG_BADRPT 14

#ifndef RC_INVOKED
    typedef struct _regex
    {
        size_t re_nsub;
        void* re_innerctx;
    } regex_t;

    typedef int regoff_t;

    typedef struct _regmatch
    {
        regoff_t rm_so;
        regoff_t rm_eo;
    } regmatch_t;

    int _RTL_FUNC regcomp(regex_t*, const char* ZSTR, int);
    int _RTL_FUNC regexec(regex_t*, const char* ZSTR, size_t, regmatch_t[], int);
    size_t _RTL_FUNC regerror(int, const regex_t*, char* ZSTR, size_t);
    void _RTL_FUNC regfree(regex_t*);
#endif

#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* __REGEX_H */
