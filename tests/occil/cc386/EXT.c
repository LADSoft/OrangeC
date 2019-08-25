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
#include <stdio.h>
#include <string.h>
#include "cmdline.h"
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
/*
 * If no extension, add the one specified
 */
void AddExt(char *buffer, char *ext)
{
    char *pos = strrchr(buffer, '.');
    if (!pos || (*(pos - 1) == '.'))
        strcat(buffer, ext);
}

/*
 * Strip extension, if it has one
 */
void StripExt(char *buffer)
{
    char *pos = strrchr(buffer, '.');
    if (pos && (*(pos - 1) != '.'))
        *pos = 0;
}

/*
 * Return path of EXE file
 */
void EXEPath(char *buffer, char *filename)
{
    char *temp;
    strcpy(buffer, filename);
    if ((temp = strrchr(buffer, '\\')) != 0)
        *(temp + 1) = 0;
    else
        buffer[0] = 0;
}

//-------------------------------------------------------------------------

int HasExt(char *buffer, char *ext)
{
    int l = strlen(buffer), l1 = strlen(ext);
    if (l1 < l)
    {
        return !stricmp(buffer + l - l1, ext);
    }
    return 0;
}
