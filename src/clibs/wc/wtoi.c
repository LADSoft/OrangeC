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

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"


int _RTL_FUNC wtoi(const wchar_t* s) 
{
	char buf[256], *q = buf;
	while (*s)
		*q++ = *s++;
	*q = *s;
	return atoi(buf);
}
int _RTL_FUNC _wtoi(const wchar_t* s) { return wtoi(s); }

long _RTL_FUNC wtol(const wchar_t* s) 
{
	char buf[256], *q = buf;
	while (*s)
		*q++ = *s++;
	*q = *s;
	return atol(buf);
}
long _RTL_FUNC _wtol(const wchar_t* s) { return wtoi(s); }
long long _RTL_FUNC wtoll(const wchar_t* s) 
{
	char buf[256], *q = buf;
	while (*s)
		*q++ = *s++;
	*q = *s;
	return atoll(buf);
}
long long _RTL_FUNC _wtoll(const wchar_t* s) { return wtoi(s); }