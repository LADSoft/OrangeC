/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

/*
 * Search for a file along a path list
 */
#include <string.h>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <float.h>
#include <richedit.h>
#include "header.h"
/*
 * Pull the next path off the path search list
 */
static char* parsepath(char* path, char* buffer)
{
    char* pos = path;

    /* Quit if hit a ';' */
    while (*pos)
    {
        if (*pos == ';')
        {
            pos++;
            break;
        }
        *buffer++ = *pos++;
    }
    *buffer = 0;

    /* Return a null pointer if no more data */
    if (*pos)
        return (pos);

    return (0);
}

/*
 * For each library:
 * Search local directory and all directories in the search path
 *  until it is found or run out of directories
 */
FILE* MySearchPath(char* string, char* searchpath, char* mode)
{
    FILE* in;
    char* newpath = searchpath;

    /* Search local path */
    in = fopen((char*)string, mode);
    if (in)
    {
        return (in);
    }
    else
    {
        char buffer[MAX_PATH];
        char name[MAX_PATH];
        while (newpath)
        {
            /* Create a file name along this path */
            newpath = parsepath(newpath, buffer);
            if (buffer[strlen(buffer) - 1] != '\\')
                strcat(buffer, "\\");
            strcat(buffer, "hi");
            strcpy(name, string);
            abspath(name, buffer);
            /* Check this path */
            in = fopen(name, mode);
            if (in)
            {
                strcpy(string, name);
                return (in);
            }
        }
    }
    return (0);
}
int pstrncmp(const WCHAR* str1, const WCHAR* str2, int n)
{
    while (n && *str1++ == *str2++)
        n--;
    if (!n)
        return 0;
    return (--str1 > --str2) ? 1 : -1;
}

//-------------------------------------------------------------------------

int pstrcmp(const WCHAR* str1, const WCHAR* str2)
{
    while (*str1 && *str1 == *str2)
    {
        str1++;
        str2++;
    }
    if (*(str1) == 0)
    {
        if (*(str2) == 0)
            return 0;
        else
            return -1;
    }
    return str1 > str2 ? 1 : -1;
}

//-------------------------------------------------------------------------

void pstrcpy(WCHAR* str1, const WCHAR* str2)
{
    while (*str2)
        *str1++ = *str2++;
    *str1 = 0;
}

//-------------------------------------------------------------------------

void pstrncpy(WCHAR* str1, const WCHAR* str2, int len) { memcpy(str1, str2, len * sizeof(WCHAR)); }

//-------------------------------------------------------------------------

void pstrcat(WCHAR* str1, const WCHAR* str2)
{
    while (*str1++)
        ;
    while (*str2)
        *str1++ = *str2++;
    *str1++ = 0;
}

//-------------------------------------------------------------------------

WCHAR* pstrchr(WCHAR* str, WCHAR ch)
{
    while (*str && *str != ch)
        str++;
    if (*str)
        return str;
    return 0;
}

//-------------------------------------------------------------------------

WCHAR* pstrrchr(WCHAR* str, WCHAR ch)
{
    WCHAR* start = str;
    while (*str++)
        ;
    str--;
    while (str != start - 1 && *str != ch)
        str--;
    if (str != start - 1)
        return str;
    return 0;
}

//-------------------------------------------------------------------------

int pstrlen(const WCHAR* s)
{
    int len = 0;
    while (*s++)
        len++;
    return len;
}

//-------------------------------------------------------------------------

WCHAR* pstrstr(WCHAR* str1, const WCHAR* str2)
{
    while (1)
    {
        WCHAR* pt = pstrchr(str1, str2[0]);
        if (!pt)
            return 0;
        if (!pstrcmp(pt, str2))
            return pt;
        str1 = pt + 1;
    }
}
