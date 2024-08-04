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

#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

#pragma netlib System.Windows.Forms

using namespace System::Windows::Forms;

void __ll_assertfail( const char *__who, const char *__file, int __line, 
                     const char *__func, const char *__msg ) 
{
   char buf[256] ;
   if (__func)
       sprintf( buf, "%s %s(%s:%d) : %s\n", __who, __file, __func, __line, __msg );
   else
       sprintf( buf, "%s %s(%d) : %s\n", __who, __file, __line, __msg );
   __string str(buf);
   MessageBox::Show(str,(__string)"Assertion Failed") ;
}
