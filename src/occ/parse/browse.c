/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#include "browse.h"
#include <direct.h>
  
extern COMPILER_PARAMS cparams;
extern ARCH_DEBUG *chosenDebugger;
extern INCLUDES *includes;
extern int funcNesting;

static int currentFile = 0;
static BROWSEFILE *files;


// this function isn't very portable..
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
        projname[0] = _getdrive() + 'A' - 1;
        projname[1] = ':';
        strcpy(projname + 2, name);
        strcpy(name, projname);
    }
}

/*-------------------------------------------------------------------------*/

void browse_init(void)
{
    if (!cparams.prm_browse && !cparams.prm_debug)
        return ;
    currentFile = 0;
    files = NULL;
}

static void getBrowseName(char *buf, SYMBOL *sp)
{
    if (sp->storage_class == sc_localstatic)
        my_sprintf(buf, "_%s", sp->name);
    else
        my_sprintf(buf, "%s", sp->decoratedName);
}
static void addBrowseRecord(BROWSEINFO *bri)
{
    chosenDebugger->browsedata(bri);
}
/*-------------------------------------------------------------------------*/

void browse_startfunc(SYMBOL *func, int lineno)
{
    char name[4096];
    BROWSEINFO *bri;
    if (!cparams.prm_browse || !chosenDebugger || !addBrowseRecord)
        return ;
    bri = (BROWSEINFO *)Alloc(sizeof(BROWSEINFO));
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

void browse_endfunc(SYMBOL *func, int lineno)
{
    char name[4096];
    BROWSEINFO *bri;
    if (!cparams.prm_browse || !chosenDebugger || !addBrowseRecord)
        return ;
    bri = (BROWSEINFO *)Alloc(sizeof(BROWSEINFO));
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

void browse_startfile(char *name, int index)
{
    char exname[260];
    BROWSEFILE *bf, *bff , **bffp;
    BROWSEINFO *bri;
    if ((!cparams.prm_browse && !cparams.prm_debug) || !chosenDebugger || !chosenDebugger->browsefile || !addBrowseRecord)
        return ;
    currentFile = index;

    bri = (BROWSEINFO *)Alloc(sizeof(BROWSEINFO));
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
    bf = (BROWSEFILE *)Alloc(sizeof(BROWSEFILE));
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

void browse_variable(SYMBOL *var)
{
    char name[4096];
    BROWSEINFO *bri;
    if (!cparams.prm_browse || !chosenDebugger || !addBrowseRecord)
        return ;
    if (var->thisPtr)
        return ;
        
    if (!var->decoratedName)
        return ;
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
    bri = (BROWSEINFO *)Alloc(sizeof(BROWSEINFO));
    bri->type = isfunction(var->tp) ? BRS_PROTOTYPE : BRS_VARIABLE;
    getBrowseName(name, var);
    bri->name = litlate(name);
    bri->lineno = var->declline;
    bri->charpos = var->declcharpos >= 0 ? var->declcharpos : 0;
    bri->flags = (var->storage_class == sc_external  || isfunction(var->tp) ? BRF_EXTERNAL : 0) | (var
        ->storage_class == sc_static ? BRF_STATIC : 0) | (var->storage_class == sc_type ? BRF_TYPE : 0);
    bri->filenum = var->declfilenum;
    addBrowseRecord(bri);
}
void browse_usage(SYMBOL *var, int file)
{
    char name[4096];
    BROWSEINFO *bri;
    if (!cparams.prm_browse || !chosenDebugger || !addBrowseRecord || funcNesting > 1)
        return ;
    if (var->thisPtr)
        return ;
        
    if (!var->decoratedName)
        return ;
    if (strstr(var->name, "++") || strstr(var->name, "$anontype"))
        return;
    bri = (BROWSEINFO *)Alloc(sizeof(BROWSEINFO));
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

void browse_define(char *name, int lineno, int charindex)
{
    BROWSEINFO *bri;
    if (!cparams.prm_browse || !chosenDebugger || !addBrowseRecord)
        return ;
    bri = (BROWSEINFO *)Alloc(sizeof(BROWSEINFO));
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
    BROWSEINFO *bri;
    if (!cparams.prm_browse || !chosenDebugger || !addBrowseRecord)
        return ;
    bri = (BROWSEINFO *)Alloc(sizeof(BROWSEINFO));
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
    BROWSEINFO *bri;
    if (!cparams.prm_browse || !chosenDebugger || !addBrowseRecord)
        return ;
    bri = (BROWSEINFO *)Alloc(sizeof(BROWSEINFO));
    bri->type = BRS_BLOCKEND;
    bri->name = "";
    bri->lineno = lineno;
    bri->charpos = 0;
    bri->flags = 0;
    bri->filenum = currentFile;
    addBrowseRecord(bri);
}
