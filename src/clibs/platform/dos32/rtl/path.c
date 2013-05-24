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
#include <dir.h>

void _RTL_FUNC _splitpath(char*path, char *drive, char *dir, char*name, char *ext)
{
    char *v,*p, *x;
    char temp[256];
    v = strchr(path,':');
    p = path;
    x = drive ? drive : temp;
    while (p <=v)
        *x++ = *p++;
    *x = 0;
    v = strrchr(path,'\\');
    x = dir ? dir : temp;
    while (p <= v)
        *x++ = *p++;
    *x = 0;
    v = strrchr(path,'.');
    if (v == 0)
        v = path + strlen(path);
    x = name ? name : temp;
    while (p < v)
        *x++ = *p++;
    *x = 0;
    x = ext ? ext : temp;
    while (*p)
        *x++=*p++;
    *x = 0;
}
void _RTL_FUNC _makepath(char *path, char *drive, char *dir, char *name, char *ext)
{
    path[0] = 0;
    if (drive)
        strcat(path,drive);
    if (dir)
        strcat(path,dir);
    if (name)
        strcat(path,name);
    if (ext)
        strcat(path,ext);
}

char *_RTL_FUNC _fullpath(char *absPath, const char *relPath, size_t len)
{
    char result[MAXPATH], *p;
    int disk;
    if (relPath[1] == ':')
    {
        disk = toupper(relPath[0]) - 'A';
        relPath += 2;
    }
    else
        disk = getdisk();
    if (getcurdir(disk+1, result + 3) < 0)
        return NULL;
    result[0] = disk + 'A';
    result[1] = ':';
    result[2] = '\\';
    if (*relPath == '\\')
    {
        relPath++;
        p = result+2;
    }
    else
    {
        p = result + strlen(result);
        *p = '\\';
    }
    while (*relPath)
    {
    
        if (relPath[0] == '.' && relPath[1] == '\\')
            relPath+=2;
        else if (relPath[0] == '.' && relPath[1] == '.' && relPath[2] == '\\')
        {
            relPath += 3;
            p--; /* won't get here if p isn't pointing to a '\\' */
            while (*p != '\\' && *p != ':')
                p--;
            if (*p == ':')
                return NULL;
        }
        else
        {
            while (*relPath && *relPath != '\\')
                *++p = *relPath++;
            if (*relPath)
                *++p = *relPath++;
        }
    }
    *++p = 0;
    if (!absPath)
        absPath = strdup(result);
    else if (strlen(result) > len-1)
        return NULL;
    
    strcpy(absPath, result);
    return absPath;
}
int _RTL_FUNC         fnsplit(const char  *__path,
                                    char  *__drive,
                                    char  *__dir,
                                    char  *__name,
                                    char  *__ext )
{
    char buf[256],*p,*q =buf;
    strcpy(buf,__path);
    p = strrchr(buf,'.');
    if (__ext)
        if (p && *(p-1) != '.') {
            strcpy(__ext,p);
            *p=0;
        }
        else __ext[0] = 0;

    if (__drive)
        if (buf[1] == ':') {
            strncpy(__drive,buf,2);
            __drive[2] = 0;
            q += 2;
        }
        else
            __drive[0] = 0;
    
    if (__name)
    {
        p = strrchr(buf,'\\');
        if (p) {
            strcpy(__name,p+1);
            *(p+1) = 0;
        }
       else  {
          strcpy(__name,q) ;
          q = 0 ;
       }
    }
    if (__dir)
       if (q)
          strcpy(__dir,q);
       else
          __dir[0] = 0 ;
    return 0;
}
void _RTL_FUNC        fnmerge( char  *__path,
                                    const char  *__drive,
                                    const char  *__dir,
                                    const char  *__name,
                                    const char  *__ext )
{
    char *p;
    if (__drive[0]) {
        __path[0] = __drive[0];
        __path[1] = ':';
        __path[2] = 0;
        p = __path+2;
    }
    if (__dir[0]) {
        if (__dir[0] != '\\')
            *p++ = '\\';
        strcpy(p,__dir);
        p = p + strlen(p);
    }
    strcpy(p,__name);
    p = p + strlen(p);
    if (__ext[0] && __ext[0] != '.')
        *p++ = '.';
    strcpy(p,__ext);
}
