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
#include <errno.h>
#include <string.h>

static errno_t thunk(wchar_t* buffer, size_t size, const wchar_t* string, errno_t error)
{
    if (size == 0)
         return ENOSPC;
    char* str;
    if (error >= _sys_nerr || (int)error < 0)
        str = "Unknown error";
    else
        str = _sys_errlist[error];
    int needed = wcslen(str) + (string ? wcslen(string) + 2 : 0);
    int hasSpace = needed < size;
    if (string && *string)
    {
        int i;
        for (i =0; i < size-1 && i < wcslen(string); i++)
            buffer[i] = string[i];
        buffer[i] = 0;
        if (size - wcslen(buffer) > 2)
        {
           wcscat(buffer, L": ");
           wchar_t *p = buffer + wcslen(buffer);
           if ((int)(size - wcslen(buffer) -1) > 0)
           {
               int n = size - wcslen(buffer) - 1;
               for (i =0; i < n; i++)
                   p[i] = str[i];
               p[i] = 0;
               size -= wcslen(p);
           }
        }
    }
    else
    {
        int i, n = wcslen(str);
        for (i =0; i < size-1 && i < n ; i++)
            buffer[i] = str[i];
        buffer[i] = 0;
        buffer[size-1] = 0;
    }
    if (!hasSpace && wcslen(buffer) > 3)
       wcscpy(buffer + wcslen(buffer) -3, L"...");

    return hasSpace ? 0 : ENOSPC;
}
wchar_t* _RTL_FUNC wcserror(int error) 
{ 
        static wchar_t buf[256];
	thunk(buf, sizeof(buf)/sizeof(wchar_t),0, error); 
        return buf;
}
wchar_t* _RTL_FUNC _wcserror(const wchar_t* str) 
{ 
        static wchar_t buf[1024];
	thunk(buf, sizeof(buf)/sizeof(wchar_t), str, errno); 
        return buf;
}
errno_t _RTL_FUNC wcserror_s(wchar_t *buf, size_t size, errno_t error)
{
	return thunk(buf, size, 0, (int)error);
}
errno_t _RTL_FUNC _wcserror_s(wchar_t *buf, size_t size, wchar_t *str)
{
	return thunk(buf, size, str, errno);
}
