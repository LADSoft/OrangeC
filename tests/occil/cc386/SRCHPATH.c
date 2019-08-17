/*
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by
Matthew Brandt (mailto::mattb@walkingdog.net)

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
/*
 * Search for a file along a path list
 */
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "cmdline.h"
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "lists.h"
#include "expr.h"
#include "c.h"

#include <io.h>
/*
 * Pull the next path off the path search list
 */
static char *parsepath(char *path, char *buffer)
{
    char *pos = path;

    /* Quit if hit a ';' */
    while (*pos)
    {
        if (*pos == ';')
        {
            pos++;
            break;
        }
        *buffer++ =  *pos++;
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
int SearchPath(char *string, char *searchpath, int mode)
{
    FILE *in;
    char *newpath = searchpath;

    /* If no path specified we search along the search path */
    if (string[0] != '\\' && string[1] != ':')
    {
        char buffer[260];
        int n;
        while (newpath)
        {
            /* Create a file name along this path */
            newpath = parsepath(newpath, buffer);
            n = strlen (buffer);
            if (n && buffer[n - 1] != '\\' && buffer[n - 1] != '/')
                strcat(buffer, "\\");
            strcat(buffer, (char*)string);

            /* Check this path */
            in = open(buffer, mode, S_IREAD | S_IWRITE);
            if (in !=  - 1)
            {
                strcpy(string, buffer);
                return (in);
            }
        }
    }
    else
    {
        in = open((char*)string, mode, S_IREAD | S_IWRITE);
        if (in !=  - 1)
        {
            return (in);
        }
    }
    return ( - 1);
}
