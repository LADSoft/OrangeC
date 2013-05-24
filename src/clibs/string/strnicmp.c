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
#include <string.h>
#include <ctype.h>

int _RTL_FUNC strnicmp(char *__s1, char *__s2, size_t __n)
{
   while (__n && *__s1 && *__s2) {
        if (toupper(*__s1) != toupper(*__s2))
            return toupper(*__s1) > toupper(*__s2) ? 1 : -1;
        __s1++;
        __s2++;
        __n--;
    }
    if (__n && toupper(*__s1) != toupper(*__s2))
        return toupper(*__s1) > toupper(*__s2) ? 1 : -1;
    return 0;
}
int _RTL_FUNC _strnicmp(char *__s1, char *__s2, size_t __n)
{
    return strnicmp(__s1, __s2, __n);
}
int _RTL_FUNC strncmpi(char *__s1, char *__s2, size_t __n)
{
    return strnicmp(__s1, __s2, __n);
}
int _RTL_FUNC _strncmpi(char *__s1, char *__s2, size_t __n)
{
    return strnicmp(__s1, __s2, __n);
}
