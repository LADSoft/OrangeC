/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <errno.h>
/* must define errno and _dos_errno in the low-level code */


char _RTL_DATA *_sys_errlist[] ={
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
int _RTL_DATA _sys_nerr = sizeof (_sys_errlist) / sizeof(char *) ;

void _RTL_FUNC perror(const char *string)
{
   char *str ;
   if (errno >= _sys_nerr)
      str = "Unkown error" ;
   else
      str = _sys_errlist[errno] ;
   if (string && *string) {
      fputs(string,stderr) ;
      fputs(": ",stderr) ;
   }
   fputs(str,stderr) ;
   fputs("\n",stderr) ;
}