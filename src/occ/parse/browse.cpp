/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#include "browse.h"
#ifndef HAVE_UNISTD_H
#    include <direct.h>
#else
#    include <unistd.h>
#endif
extern COMPILER_PARAMS cparams;
extern ARCH_DEBUG* chosenDebugger;
extern INCLUDES* includes;
extern int funcNesting;

static int currentFile = 0;
static BROWSEFILE* files;

// this function isn't very portable..
void abspath(char* name)
{
    char projname[256], *p, *nname = name;
    getcwd(projname, 256);
    if (name[1] == ':')
        return;
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
            return;
        p--;
        while (!strncmp(name, "..\\", 3))
        {
            while (p > projname && *p-- != '\\')
                ;
            name += 3;
        }
        *++p = '\\';
        p++;
        strcpy(p, name);
        strcpy(nname, projname);
    }
#ifdef WIN32
    else
    {
        projname[0] = _getdrive() + 'A' - 1;
        projname[1] = ':';
        strcpy(projname + 2, name);
        strcpy(name, projname);
    }
#endif
}

/*-------------------------------------------------------------------------*/

void browse_init(void)
{
    if (!cparams.prm_browse && !cparams.prm_debug)
        return;
    currentFile = 0;
    files = NULL;
}

static void getBrowseName(char* buf, SYMBOL* sp) { my_sprintf(buf, "%s", sp->decoratedName); }
static void addBrowseRecord(BROWSEINFO* bri) { chosenDebugger->browsedata(bri); }
/*-------------------------------------------------------------------------*/

void browse_startfunc(SYMBOL* func, int lineno)
{
    char name[4096];
    BROWSEINFO* bri;
    if (!cparams.prm_browse || !chosenDebugger)
        return;
    bri = (BROWSEINFO*)Alloc(sizeof(BROWSEINFO));
    bri->type = BRS_STARTFUNC;
    getBrowseName(name, func);
    bri->name = litlate(name);
    bri->lineno = lineno;
    bri->charpos = 0;
    bri->flags = func->storage_class == sc_static ? BRF_STATIC : 0;
    bri->filenum = currentFile;
    addBrowseRecord(bri);
}

/*-------------------------------------------------------------------------*/

void browse_endfunc(SYMBOL* func, int lineno)
{
    char name[4096];
    BROWSEINFO* bri;
    if (!cparams.prm_browse || !chosenDebugger)
        return;
    bri = (BROWSEINFO*)Alloc(sizeof(BROWSEINFO));
    bri->type = BRS_ENDFUNC;
    getBrowseName(name, func);
    bri->name = litlate(name);
    bri->lineno = lineno;
    bri->charpos = 0;
    bri->flags = 0;
    bri->filenum = currentFile;
    addBrowseRecord(bri);
}

/*-------------------------------------------------------------------------*/

void browse_startfile(char* name, int index)
{
    char exname[260];
    BROWSEFILE *bf, *bff, **bffp;
    BROWSEINFO* bri;
    if ((!cparams.prm_browse && !cparams.prm_debug) || !chosenDebugger || !chosenDebugger->browsefile )
        return;
    currentFile = index;

    bri = (BROWSEINFO*)Alloc(sizeof(BROWSEINFO));
    bri->type = BRS_STARTFILE;
    bri->filenum = currentFile;
    bri->name = "";
    addBrowseRecord(bri);

    strcpy(exname, name);
    abspath(exname);
    bff = files;
    while (bff)
    {
        if (bff->filenum == index)
        {
            return;
        }
        bff = bff->next;
    }
    bf = (BROWSEFILE*)Alloc(sizeof(BROWSEFILE));
    bf->name = litlate(exname);
    bf->filenum = index;
    if (!files)
        chosenDebugger->browsefile(bf);
    bffp = &files;
    while (*bffp)
        bffp = &(*bffp)->next;
    *bffp = bf;
}

/*-------------------------------------------------------------------------*/

void browse_variable(SYMBOL* var)
{
    char name[4096];
    BROWSEINFO* bri;
    if (!cparams.prm_browse || !chosenDebugger)
        return;
    if (var->thisPtr)
        return;

    if (!var->decoratedName)
        return;
    if (strstr(var->name, "++") || strstr(var->name, "$anontype"))
        return;
    switch (var->storage_class)
    {
        case sc_global:
        case sc_external:
        case sc_localstatic:
        case sc_static:
        case sc_type:
            break;
        default:
            if (!isfunction(var->tp))
                return;
            break;
    }
    bri = (BROWSEINFO*)Alloc(sizeof(BROWSEINFO));
    bri->type = isfunction(var->tp) ? BRS_PROTOTYPE : BRS_VARIABLE;
    getBrowseName(name, var);
    bri->name = litlate(name);
    bri->lineno = var->declline;
    bri->charpos = var->declcharpos >= 0 ? var->declcharpos : 0;
    bri->flags = (var->storage_class == sc_external || isfunction(var->tp) ? BRF_EXTERNAL : 0) |
                 (var->storage_class == sc_static ? BRF_STATIC : 0) | (var->storage_class == sc_type ? BRF_TYPE : 0);
    bri->filenum = var->declfilenum;
    addBrowseRecord(bri);
}
void browse_usage(SYMBOL* var, int file)
{
    char name[4096];
    BROWSEINFO* bri;
    if (!cparams.prm_browse || !chosenDebugger  || funcNesting > 1)
        return;
    if (var->thisPtr)
        return;

    if (!var->decoratedName)
        return;
    if (strstr(var->name, "++") || strstr(var->name, "$anontype"))
        return;
    bri = (BROWSEINFO*)Alloc(sizeof(BROWSEINFO));
    bri->type = BRS_USAGE;
    getBrowseName(name, var);
    bri->name = litlate(name);
    bri->lineno = includes->line;
    bri->charpos = includes->lptr - includes->inputline - strlen(var->name);
    if (bri->charpos < 0)
        bri->charpos = 0;
    bri->flags = 0;
    bri->filenum = file;
    addBrowseRecord(bri);
}

/*-------------------------------------------------------------------------*/

void browse_define(char* name, int lineno, int charindex)
{
    BROWSEINFO* bri;
    if (!cparams.prm_browse || !chosenDebugger)
        return;
    bri = (BROWSEINFO*)Alloc(sizeof(BROWSEINFO));
    bri->type = BRS_DEFINE;
    bri->name = litlate(name);
    bri->lineno = lineno;
    bri->charpos = charindex;
    bri->flags = 0;
    bri->filenum = currentFile;
    addBrowseRecord(bri);
}

/*-------------------------------------------------------------------------*/

void browse_blockstart(int lineno)
{
    BROWSEINFO* bri;
    if (!cparams.prm_browse || !chosenDebugger)
        return;
    bri = (BROWSEINFO*)Alloc(sizeof(BROWSEINFO));
    bri->type = BRS_BLOCKSTART;
    bri->name = "";
    bri->lineno = lineno;
    bri->charpos = 0;
    bri->flags = 0;
    bri->filenum = currentFile;
    addBrowseRecord(bri);
}

/*-------------------------------------------------------------------------*/

void browse_blockend(int lineno)
{
    BROWSEINFO* bri;
    if (!cparams.prm_browse || !chosenDebugger)
        return;
    bri = (BROWSEINFO*)Alloc(sizeof(BROWSEINFO));
    bri->type = BRS_BLOCKEND;
    bri->name = "";
    bri->lineno = lineno;
    bri->charpos = 0;
    bri->flags = 0;
    bri->filenum = currentFile;
    addBrowseRecord(bri);
}
