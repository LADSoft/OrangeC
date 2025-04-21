/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include "compiler.h"
#include "browsedefs.h"
#include "PreProcessor.h"
#include "config.h"
#include "ildata.h"
#include "stmt.h"
#include "occparse.h"
#include "lex.h"
#include "memory.h"
#include "OptUtils.h"
#include "types.h"

#ifndef HAVE_UNISTD_H
#    include <direct.h>
#else
#    include <unistd.h>
#    define _getcwd getcwd
#endif

namespace Parser
{
static int currentFile = 0;

// this function isn't very portable..
template <size_t n>
void abspath(char (&name)[n])
{
    char projname[256], *p;
    _getcwd(projname, 256);
    if (name[1] == ':')
        return;
    if (!strchr(name, '\\'))
    {
        _getcwd(projname, 256);
        Utils::StrCat(projname, "\\");
        Utils::StrCat(projname, name);
        Utils::StrCpy(name, projname);
    }
    else if (strstr(name, "..\\"))
    {
        projname[0] = 0;
        _getcwd(projname, 256);
        p = projname + strlen(projname);
        if (p == projname)
            return;
        p--;
        char* nname = name;
        while (!strncmp(nname, "..\\", 3))
        {
            while (p > projname && *p-- != '\\')
                ;
            nname += 3;
        }
        *++p = '\\';
        p++;
        Utils::StrCpy(p, sizeof(projname) - (p - projname), nname);
        Utils::StrCpy(name, projname);
    }
#ifdef WIN32
    else
    {
        projname[0] = _getdrive() + 'A' - 1;
        projname[1] = ':';
        Utils::StrCpy(projname + 2, sizeof(projname)-2, name);
        Utils::StrCpy(name, projname);
    }
#endif
}

/*-------------------------------------------------------------------------*/

void browse_init(void)
{
    if (!Optimizer::cparams.prm_browse && !Optimizer::cparams.prm_debug)
        return;
    currentFile = 0;
}

void browsedata(Optimizer::BROWSEINFO* bi) { Optimizer::browseInfo.push_back(bi); }
void browsefile(Optimizer::BROWSEFILE* bf) { Optimizer::browseFiles.push_back(bf); }

static void getBrowseName(char* buf, SYMBOL* sp) { Optimizer::my_sprintf(buf, "%s", sp->sb->decoratedName); }
static void addBrowseRecord(Optimizer::BROWSEINFO* bri) { browsedata(bri); }
/*-------------------------------------------------------------------------*/

void browse_startfunc(SYMBOL* func, int lineno)
{
    char name[4096];
    Optimizer::BROWSEINFO* bri;
    if (!Optimizer::cparams.prm_browse || func->sb->declline <= 0)
        return;
    bri = Allocate<Optimizer::BROWSEINFO>();
    bri->type = BRS_STARTFUNC;
    getBrowseName(name, func);
    bri->name = litlate(name);
    bri->lineno = lineno;
    bri->charpos = 0;
    bri->flags = func->sb->storage_class == StorageClass::static_ ? BRF_STATIC : 0;
    bri->filenum = currentFile;
    addBrowseRecord(bri);
}

/*-------------------------------------------------------------------------*/

void browse_endfunc(SYMBOL* func, int lineno)
{
    char name[4096];
    Optimizer::BROWSEINFO* bri;
    if (!Optimizer::cparams.prm_browse || func->sb->declline <= 0)
        return;
    bri = Allocate<Optimizer::BROWSEINFO>();
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

void browse_startfile(const char* name, int index)
{
    char exname[260];
    Optimizer::BROWSEFILE *bf, **bffp;
    Optimizer::BROWSEINFO* bri;
    if (!Optimizer::cparams.prm_browse && !Optimizer::cparams.prm_debug)
        return;
    currentFile = index;

    bri = Allocate<Optimizer::BROWSEINFO>();
    bri->type = BRS_STARTFILE;
    bri->filenum = currentFile;
    bri->name = "";
    addBrowseRecord(bri);
    for (auto bff : Optimizer::browseFiles)
    {
        if (bff->filenum == index)
            return;
    }

    Utils::StrCpy(exname, name);
    abspath(exname);

    bf = Allocate<Optimizer::BROWSEFILE>();
    bf->name = litlate(exname);
    bf->filenum = index;
    browsefile(bf);
}

/*-------------------------------------------------------------------------*/

void browse_variable(SYMBOL* var)
{
    char name[4096];
    Optimizer::BROWSEINFO* bri;
    if (!Optimizer::cparams.prm_browse || !var->sb || var->sb->declline <= 0)
        return;
    if (var->sb->thisPtr)
        return;

    if (!var->sb->decoratedName)
        return;
    if (strstr(var->name, "++") || strstr(var->name, "$anontype"))
        return;
    switch (var->sb->storage_class)
    {
        case StorageClass::global_:
        case StorageClass::external_:
        case StorageClass::localstatic_:
        case StorageClass::static_:
        case StorageClass::type_:
            break;
        default:
            if (!var->tp->IsFunction())
                return;
            break;
    }
    bri = Allocate<Optimizer::BROWSEINFO>();
    bri->type = var->tp->IsFunction() ? BRS_PROTOTYPE : BRS_VARIABLE;
    getBrowseName(name, var);
    bri->name = litlate(name);
    bri->lineno = var->sb->declline;
    bri->charpos = var->sb->declcharpos >= 0 ? var->sb->declcharpos : 0;
    bri->flags = (var->sb->storage_class == StorageClass::external_ || var->tp->IsFunction() ? BRF_EXTERNAL : 0) |
                 (var->sb->storage_class == StorageClass::static_ ? BRF_STATIC : 0) |
                 (var->sb->storage_class == StorageClass::type_ ? BRF_TYPE : 0);
    bri->filenum = var->sb->declfilenum;
    addBrowseRecord(bri);
}
void browse_usage(SYMBOL* var, int file)
{
    char name[4096];
    Optimizer::BROWSEINFO* bri;
    if (!Optimizer::cparams.prm_browse || !var->sb || var->sb->declline <= 0 || funcNesting > 1)
        return;
    if (var->sb->thisPtr)
        return;

    if (!var->sb->decoratedName)
        return;
    if (strstr(var->name, "++") || strstr(var->name, "$anontype"))
        return;
    bri = Allocate<Optimizer::BROWSEINFO>();
    bri->type = BRS_USAGE;
    getBrowseName(name, var);
    bri->name = litlate(name);
    bri->lineno = preProcessor->GetRealLineNo();
    bri->charpos = charIndex - strlen(bri->name);
    if (bri->charpos < 0)
        bri->charpos = 0;
    bri->flags = 0;
    bri->filenum = file;
    addBrowseRecord(bri);
}

/*-------------------------------------------------------------------------*/

void browse_define(char* name, int lineno, int charindex)
{
    Optimizer::BROWSEINFO* bri;
    if (!Optimizer::cparams.prm_browse || lineno <= 0)
        return;
    bri = Allocate<Optimizer::BROWSEINFO>();
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
    Optimizer::BROWSEINFO* bri;
    if (!Optimizer::cparams.prm_browse || lineno <= 0)
        return;
    bri = Allocate<Optimizer::BROWSEINFO>();
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
    Optimizer::BROWSEINFO* bri;
    if (!Optimizer::cparams.prm_browse || lineno <= 0)
        return;
    bri = Allocate<Optimizer::BROWSEINFO>();
    bri->type = BRS_BLOCKEND;
    bri->name = "";
    bri->lineno = lineno;
    bri->charpos = 0;
    bri->flags = 0;
    bri->filenum = currentFile;
    addBrowseRecord(bri);
}
}  // namespace Parser