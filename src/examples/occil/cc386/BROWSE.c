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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "gen386.h"
#include "diag.h"
#include "lists.h"                       
#include "browse.h"
#include <dir.h>

extern int prm_browse;
extern int sym_charindex;
extern int sys_inc;

void abspath(char *name)
{
    char projname[256],  *p,  *nname = name;
    getcwd(projname, 256);
    if (name[1] == ':')
        return ;
    if (!strchr(name, '\\'))
    {
        getcwd(projname, 256);
        strcat(projname, "\\");
        strcat(projname, name);
        strcpy(name, projname);
    }
    else if (strstr(name, "..\\"))
    {
        getcwd(projname, 256);
        //   p = strrchr(projname,'\\') ;
        p = projname + strlen(projname);
        if (!p)
            return ;
        p--;
        while (!strncmp(name, "..\\", 3))
        {
            while (p > projname &&  *p-- != '\\')
                ;
            name += 3;
        }
        *++p = '\\';
        p++;
        strcpy(p, name);
        strcpy(nname, projname);
    }
    else
    {
        projname[0] = getdisk() + 'A';
        projname[1] = ':';
        strcpy(projname + 2, name);
        strcpy(name, projname);
    }
}

//-------------------------------------------------------------------------

void browse_init(void)
{
    if (!prm_browse)
        return ;
    dump_browsedata(BRS_SIGNATURE, 4);
}

//-------------------------------------------------------------------------

void browse_startfunc(SYM *func, int lineno)
{
    char name[256];
    unsigned char buf1[256];
    if (!prm_browse)
        return ;
    unmangle(name, func->name);
    buf1[0] = BRS_STARTFUNC;
    *(long*)(buf1 + 1) = lineno;
    *(short*)(buf1 + 5) = 0; // space for char pos
    *(short*)(buf1 + 7) = func->storage_class == sc_static ? 2 : 0;
    buf1[9] = strlen(name);
    memcpy(buf1 + 10, name, buf1[9]);
    buf1[10+buf1[9]] = 0; // space for an ascii version of the type
    dump_browsedata(buf1, buf1[9] + buf1[10+buf1[9]] + 11);
}

//-------------------------------------------------------------------------

void browse_endfunc(SYM *func, int lineno)
{
    char name[256];
    unsigned char buf1[256];
    if (!prm_browse)
        return ;
    unmangle(name, func->name);
    buf1[0] = BRS_ENDFUNC;
    *(long*)(buf1 + 1) = lineno;
    buf1[5] = strlen(name);
    memcpy(buf1 + 6, name, buf1[5]);
    dump_browsedata(buf1, buf1[5] + 6);
}

//-------------------------------------------------------------------------

void browse_startfile(char *name)
{
    unsigned char buf1[256], exname[256];
    if (!prm_browse)
        return ;
    strcpy(exname, name);
    abspath(exname);
    buf1[0] = BRS_STARTFILE;
    buf1[1] = strlen(exname);
    memcpy(buf1 + 2, exname, buf1[1]);
    dump_browsedata(buf1, buf1[1] + 2);
}

//-------------------------------------------------------------------------

void browse_variable(SYM *var, int lineno)
{
    char name[256];
    unsigned char buf1[256];
    if (!prm_browse)
        return ;
	if (sys_inc)
		return;
    unmangle(name, var->name);
    buf1[0] = BRS_VARIABLE;
    *(long*)(buf1 + 1) = lineno;
    *(short*)(buf1 + 5) = sym_charindex; // space for character pos
    *(short*)(buf1 + 7) = (var->storage_class == sc_external ? 1 : 0) | (var
        ->storage_class == sc_static ? 2 : 0) | (var->storage_class == sc_type ? 4 : 0);
    buf1[9] = strlen(name);
    memcpy(buf1 + 10, name, buf1[9]);
    buf1[10+buf1[9]] = 0; // space for an ascii version of the type
    dump_browsedata(buf1, buf1[9] + buf1[10+buf1[9]] + 11);
}

//-------------------------------------------------------------------------

void browse_define(char *name, int lineno)
{
    unsigned char buf1[256];
    if (!prm_browse)
        return ;
	if (sys_inc)
		return;
    buf1[0] = BRS_DEFINE;
    *(long*)(buf1 + 1) = lineno;
    *(short*)(buf1 + 5) = sym_charindex; // space for character pos
    buf1[7] = strlen(name);
    memcpy(buf1 + 8, name, buf1[7]);
    buf1[8+buf1[7]] = 0; // space for an ascii version of the type
    dump_browsedata(buf1, buf1[7] +  + buf1[8+buf1[7]] + 9);
}

//-------------------------------------------------------------------------

void browse_blockstart(int lineno)
{
    unsigned char buf1[256];
    if (!prm_browse)
        return ;
    buf1[0] = BRS_BLOCKSTART;
    *(long*)(buf1 + 1) = lineno;
    dump_browsedata(buf1, 5);
}

//-------------------------------------------------------------------------

void browse_blockend(int lineno)
{
    unsigned char buf1[256];
    if (!prm_browse)
        return ;
    buf1[0] = BRS_BLOCKEND;
    *(long*)(buf1 + 1) = lineno;
    dump_browsedata(buf1, 5);
}
