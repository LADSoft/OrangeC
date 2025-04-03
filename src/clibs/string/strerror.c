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

static errno_t thunk(char* buffer, size_t size, const char* string, errno_t error)
{
    if (size == 0)
         return ENOSPC;
    char* str;
    if (error >= _sys_nerr || (int)error < 0)
        str = "Unknown error";
    else
        str = _sys_errlist[error];
    int needed = strlen(str) + (string ? strlen(string) + 2 : 0);
    int hasSpace = needed < size;
    if (string && *string)
    {
        strncpy(buffer, string, size);
        buffer[size-1] = 0;
        if (size - strlen(buffer) > 2)
        {
           strcat(buffer, ": ");
           char *p = buffer + strlen(buffer);
           if ((int)(size - strlen(buffer) -1) > 0)
           {
               strncpy(p, str, size-strlen(buffer)-1);
               size -= strlen(p);
           }
        }
    }
    else
    {
        strncpy(buffer, str, size);
        buffer[size-1] = 0;
    }
    if (!hasSpace && strlen(buffer) > 3)
       strcpy(buffer + strlen(buffer) -3, "...");

    return hasSpace ? 0 : ENOSPC;
}
char* _RTL_FUNC strerror(int error) 
{ 
        static char buf[256];
	thunk(buf, sizeof(buf),0, error); 
        return buf;
}
char* _RTL_FUNC _strerror(const char* str) 
{ 
        static char buf[1024];
	thunk(buf, sizeof(buf), str, errno); 
        return buf;
}
errno_t _RTL_FUNC strerror_s(char *buf, size_t size, errno_t error)
{
	return thunk(buf, size, 0, (int)error);
}
errno_t _RTL_FUNC _strerror_s(char *buf, size_t size, char *str)
{
	return thunk(buf, size, str, errno);
}