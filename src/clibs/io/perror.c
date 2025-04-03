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
/* must define errno and _dos_errno in the low-level code */

char _RTL_DATA* _sys_errlist[] = {
    "Error 0",
    "Invalid function number",
    "No such file or directory",
    "Path not found",
    "Too many open files",
    "Permission denied",
    "Bad file number",
    "Memory arena trashed",
    "Not enough memory",
    "Invalid memory block address",
    "Invalid environment",
    "Invalid format",
    "Invalid access code",
    "Invalid data",
    "Bad address",
    "No such device",
    "Attempted to remove current directory",
    "Not same device",
    "No more files",
    "Invalid argument",
    "Arg list too big",
    "Exec format error",
    "Cross-device link",
    "Too many open files",
    "No child processes",
    "Inappropriate I/O control operation",
    "Executable file in use",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Math argument",
    "Result too large",
    "File already exists",
    "Possible deadlock",
    "Operation not permitted",
    "No such process",
    "Interrupted function call",
    "Input/output error",
    "No such device or address",
    "Resource temporarily unavailable",
    "Block device required",
    "Resource busy",
    "Not a directory",
    "Is a directory",
    "",
    "Filename too long",
};
int _RTL_DATA _sys_nerr = sizeof(_sys_errlist) / sizeof(char*);

void _RTL_FUNC perror(const char* string)
{
    char* str;
    if (errno >= _sys_nerr)
        str = "Unkown error";
    else
        str = _sys_errlist[errno];
    if (string && *string)
    {
        fputs(string, stderr);
        fputs(": ", stderr);
    }
    fputs(str, stderr);
    fputs("\n", stderr);
}