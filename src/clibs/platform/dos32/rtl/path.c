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
