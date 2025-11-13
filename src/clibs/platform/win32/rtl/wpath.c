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

static int wgetcurdir(int drive, wchar_t* buf)
{
   
   char *result = (char *)buf;
   int rv = getcurdir(drive, result);
   if (rv >= 0)
   {
       int len = strlen(result);
       for (int i= len; i >=0; i--)
          buf[i] = (wchar_t)result[i]; 
   }
   return rv;
}
void _RTL_FUNC _wsplitpath(wchar_t* path, wchar_t* drive, wchar_t* dir, wchar_t* name, wchar_t* ext)
{
    wchar_t *v, *p, *x, *q;
    wchar_t temp[MAXPATH];
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
    v = wcschr(path, ':');
    p = path;
    x = drive ? drive : temp;
    while (p <= v)
        *x++ = *p++;
    *x = 0;
    v = wcsrchr(path, '\\');
    q = wcsrchr(path, '/');
    if (q > v)
        v = q;
    x = dir ? dir : temp;
    while (p <= v)
        *x++ = *p++;
    *x = 0;
    v = wcsrchr(path, '.');
    if (v == 0)
        v = path + wcslen(path);
    x = name ? name : temp;
    while (p < v)
        *x++ = *p++;
    *x = 0;
    x = ext ? ext : temp;
    while (*p)
        *x++ = *p++;
    *x = 0;
}
void _RTL_FUNC _wsplitpath2(wchar_t* path, wchar_t* outpath, wchar_t** drive, wchar_t** dir, wchar_t** name, wchar_t** ext)
{
    wchar_t *v, *p, *x, *q;
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
    v = wcschr(path, ':');
    p = path;
    x = outpath;
    if (drive)
        *drive = x;
    while (p <= v)
        *x++ = *p++;
    *x++ = 0;
    v = wcsrchr(path, '\\');
    q = wcsrchr(path, '/');
    if (q > v)
        v = q;
    if (dir)
        *dir = x;
    while (p <= v)
        *x++ = *p++;
    *x++ = 0;
    v = wcsrchr(path, '.');
    if (v == 0)
        v = path + wcslen(path);
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
void _RTL_FUNC _wmakepath(wchar_t* path, wchar_t* drive, wchar_t* dir, wchar_t* name, wchar_t* ext)
{
    if (!path)
        return;
    path[0] = 0;
    if (drive && *drive)
    {
        wcscat(path, drive);
        if (path[wcslen(path) - 1] != ':')
            wcscat(path, ":");
    }
    if (dir && *dir)
    {
        wcscat(path, dir);
        if (path[wcslen(path) - 1] != '\\')
            wcscat(path, "\\");
    }
    if (name && *name)
        wcscat(path, name);
    if (ext && *ext)
    {
        if (*ext != '.')
            wcscat(path, ".");
        wcscat(path, ext);
    }
}

wchar_t* _RTL_FUNC _wfullpath(wchar_t* absPath, const wchar_t* relPath, size_t len)
{
    wchar_t result[MAXPATH], *p;
    int disk = getdisk();
    result[0] = disk + 'A';
    result[1] = ':';
    result[2] = '\\';
    if (wgetcurdir(disk + 1, result + 3) < 0)
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
            if (wgetcurdir(disk + 1, result + 3) < 0)
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
            p = result + wcslen(result) - 1;
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
        if (!(absPath = wcsdup(result)))
        {
            errno = ENOMEM;
            return NULL;
        }
    }
    else if (wcslen(result) > len - 1)
    {
        errno = ERANGE;
        return NULL;
    }
    else
        wcscpy(absPath, result);
    return absPath;
}
int _RTL_FUNC wfnsplit(const wchar_t* __path, wchar_t* __drive, wchar_t* __dir, wchar_t* __name, wchar_t* __ext)
{
    wchar_t buf[MAXPATH], *p, *q = buf;
    wchar_t temp[MAXPATH];
    int flags = 0;
    if (!(__path && *__path))
        return 0;
    wcscpy(buf, __path);
    p = wcsrchr(buf, '.');
    if (!__ext)
        __ext = temp;
    if (p && *(p - 1) != '.')
    {
        wcscpy(__ext, p);
        *p = 0;
        flags |= EXTENSION;
    }
    else
        __ext[0] = 0;

    if (!__drive)
        __drive = temp;
    if (buf[1] == ':')
    {
        wcsncpy(__drive, buf, 2);
        __drive[2] = 0;
        q += 2;
        flags |= DRIVE;
    }
    else
        __drive[0] = 0;

    if (!__name)
        __name = temp;
    {
        wchar_t* r;
        p = wcsrchr(buf, '\\');
        r = wcsrchr(buf, '/');
        if (r > p)
            p = r;
        if (p)
        {
            wcscpy(__name, p + 1);
            *(p + 1) = 0;
        }
        else
        {
            wcscpy(__name, q);
            q = 0;
        }
        if (*__name)
            flags |= FILENAME;
    }
    if (!__dir)
        __dir = temp;
    if (q)
    {
        wcscpy(__dir, q);
        flags |= DIRECTORY;
    }
    else
        __dir[0] = 0;
    return flags;
}
void _RTL_FUNC wfnmerge(wchar_t* __path, const wchar_t* __drive, const wchar_t* __dir, const wchar_t* __name, const wchar_t* __ext)
{
    wchar_t* p;
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
        wcscpy(p, __dir);
        p = p + wcslen(p);
    }
    wcscpy(p, __name);
    p = p + wcslen(p);
    if (__ext[0] && __ext[0] != '.')
        *p++ = '.';
    wcscpy(p, __ext);
}
