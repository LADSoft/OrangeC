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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#undef errno
#include <wchar.h>
#include <locale.h>
#include "libp.h"


static struct __rtl_data __rtl_data;

int *_RTL_FUNC __GetSignGam(void)
{
    return & __rtl_data.x_signgam ;
}
struct __rtl_data *__getRtlData(void)
{
    return &__rtl_data;
}
int *_RTL_FUNC __GetErrno(void)
{
    return & __rtl_data.x_errno ;
}
int *_RTL_FUNC __GetDosErrno(void)
{
    return & __rtl_data.x_doserrno ;
}
