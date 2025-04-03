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

#include <errno.h>
#include <string.h>
#include <dir.h>
#include <ctype.h>

void _RTL_FUNC _splitpath(char* path, char* drive, char* dir, char* name, char* ext)
{
    char *v, *p, *x, *q;
    char temp[MAXPATH];
    if (!(path && *path))
    {
        if (drive)
            *drive = '\0';
        if (dir)
            *dir = '\0';
        if (name)
            *name = '\0';
        if (ext)
            *ext = '\0';
        return;
    }
    v = strchr(path, ':');
    p = path;
    x = drive ? drive : temp;
    while (p <= v)
        *x++ = *p++;
    *x = 0;
    v = strrchr(path, '\\');
    q = strrchr(path, '/');
    if (q > v)
        v = q;
    x = dir ? dir : temp;
    while (p <= v)
        *x++ = *p++;
    *x = 0;
    v = strrchr(path, '.');
    if (v == 0)
        v = path + strlen(path);
    x = name ? name : temp;
    while (p < v)
        *x++ = *p++;
    *x = 0;
    x = ext ? ext : temp;
    while (*p)
        *x++ = *p++;
    *x = 0;
}
void _RTL_FUNC _splitpath2(char* path, char* outpath, char** drive, char** dir, char** name, char** ext)
{
    char *v, *p, *x, *q;
    if (!(path && *path) || !outpath)
    {
        if (outpath)
        {
            x = outpath;
            *x = '\0';
            if (drive)
                *drive = x;
            *(++x) = '\0';
            if (dir)
                *dir = x;
            *(++x) = '\0';
            if (name)
                *name = x;
            *(++x) = '\0';
            if (ext)
                *ext = x;
        }
        else
        {
            if (drive)
                *drive = 0;
            if (dir)
                *dir = 0;
            if (name)
                *name = 0;
            if (ext)
                *ext = 0;
        }
        return;
    }
    v = strchr(path, ':');
    p = path;
    x = outpath;
    if (drive)
        *drive = x;
    while (p <= v)
        *x++ = *p++;
    *x++ = 0;
    v = strrchr(path, '\\');
    q = strrchr(path, '/');
    if (q > v)
        v = q;
    if (dir)
        *dir = x;
    while (p <= v)
        *x++ = *p++;
    *x++ = 0;
    v = strrchr(path, '.');
    if (v == 0)
        v = path + strlen(path);
    if (name)
        *name = x;
    while (p < v)
        *x++ = *p++;
    *x++ = 0;
    if (ext)
        *ext = x;
    while (*p)
        *x++ = *p++;
    *x = 0;
}
void _RTL_FUNC _makepath(char* path, char* drive, char* dir, char* name, char* ext)
{
    if (!path)
        return;
    path[0] = 0;
    if (drive && *drive)
    {
        strcat(path, drive);
        if (path[strlen(path) - 1] != ':')
            strcat(path, ":");
    }
    if (dir && *dir)
    {
        strcat(path, dir);
        if (path[strlen(path) - 1] != '\\')
            strcat(path, "\\");
    }
    if (name && *name)
        strcat(path, name);
    if (ext && *ext)
    {
        if (*ext != '.')
            strcat(path, ".");
        strcat(path, ext);
    }
}

char* _RTL_FUNC _fullpath(char* absPath, const char* relPath, size_t len)
{
    char result[MAXPATH], *p;
    int disk = getdisk();
    result[0] = disk + 'A';
    result[1] = ':';
    result[2] = '\\';
    if (getcurdir(disk + 1, result + 3) < 0)
    {
        errno = ENOENT;
        return NULL;
    }
    if (relPath && *relPath)
    {
        if (relPath[1] == ':')
        {
            disk = toupper(relPath[0]) - 'A';
            result[0] = disk + 'A';
            relPath += 2;
            if (getcurdir(disk + 1, result + 3) < 0)
            {
                errno = ENOENT;
                return NULL;
            }
        }
        if (*relPath == '\\' || *relPath == '/')
        {
            relPath++;
            p = result + 2;
        }
        else
        {
            p = result + strlen(result) - 1;
            if (*p != '\\')
                *++p = '\\';
        }
        while (*relPath)
        {
            if (relPath[0] == '.' && (relPath[1] == '\\' || relPath[1] == '/'))
                relPath += 2;
            else if (relPath[0] == '.' && relPath[1] == '.' && (relPath[2] == '\\' || relPath[2] == '/'))
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
                while (*relPath && *relPath != '\\' && *relPath != '/')
                    *++p = *relPath++;
                if (*relPath)
                {
                    *++p = '\\';
                    relPath++;
                }
            }
        }
        *++p = 0;
    }
    if (!absPath)
    {
        if (!(absPath = strdup(result)))
        {
            errno = ENOMEM;
            return NULL;
        }
    }
    else if (strlen(result) > len - 1)
    {
        errno = ERANGE;
        return NULL;
    }
    else
        strcpy(absPath, result);
    return absPath;
}
int _RTL_FUNC fnsplit(const char* __path, char* __drive, char* __dir, char* __name, char* __ext)
{
    char buf[MAXPATH], *p, *q = buf;
    char temp[MAXPATH];
    int flags = 0;
    if (!(__path && *__path))
        return 0;
    strcpy(buf, __path);
    p = strrchr(buf, '.');
    if (!__ext)
        __ext = temp;
    if (p && *(p - 1) != '.')
    {
        strcpy(__ext, p);
        *p = 0;
        flags |= EXTENSION;
    }
    else
        __ext[0] = 0;

    if (!__drive)
        __drive = temp;
    if (buf[1] == ':')
    {
        strncpy(__drive, buf, 2);
        __drive[2] = 0;
        q += 2;
        flags |= DRIVE;
    }
    else
        __drive[0] = 0;

    if (!__name)
        __name = temp;
    {
        char* r;
        p = strrchr(buf, '\\');
        r = strrchr(buf, '/');
        if (r > p)
            p = r;
        if (p)
        {
            strcpy(__name, p + 1);
            *(p + 1) = 0;
        }
        else
        {
            strcpy(__name, q);
            q = 0;
        }
        if (*__name)
            flags |= FILENAME;
    }
    if (!__dir)
        __dir = temp;
    if (q)
    {
        strcpy(__dir, q);
        flags |= DIRECTORY;
    }
    else
        __dir[0] = 0;
    return flags;
}
void _RTL_FUNC fnmerge(char* __path, const char* __drive, const char* __dir, const char* __name, const char* __ext)
{
    char* p;
    if (!__path)
        return;
    if (__drive[0])
    {
        __path[0] = __drive[0];
        __path[1] = ':';
        __path[2] = 0;
        p = __path + 2;
    }
    if (__dir[0])
    {
        if (__dir[0] != '\\')
            *p++ = '\\';
        strcpy(p, __dir);
        p = p + strlen(p);
    }
    strcpy(p, __name);
    p = p + strlen(p);
    if (__ext[0] && __ext[0] != '.')
        *p++ = '.';
    strcpy(p, __ext);
}
