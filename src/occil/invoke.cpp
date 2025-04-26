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
#include <cstring>
#include <cstdlib>
#include "be.h"
#include "winmode.h"
#include "Utils.h"
#include "ToolChain.h"
#include "config.h"
#include "ildata.h"
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif

#define TEMPFILE "$$$OCC.TMP"
namespace occmsil
{
static Optimizer::LIST *objlist, *reslist, *rclist;
static char outFileName[260];

static void InsertFile(Optimizer::LIST** r, const char* name, const char* ext)
{

    char buf[256], *newbuffer;
    Optimizer::LIST* lst;
    Utils::StrCpy(buf, name);
    if (!outFileName[0])
    {
        Utils::StrCpy(outFileName, name);
        Utils::StripExt(outFileName);
        Utils::StrCat(outFileName, Optimizer::cparams.prm_targettype == DLL ? ".dll" : ".exe");
    }
    if (ext)
    {
        Utils::StripExt(buf);
        Utils::StrCat(buf, ext);
    }
    lst = *r;
    while (lst)
    {
        if (Utils::iequal((char*)lst->data, buf))
            return;
        lst = lst->next;
    }
    newbuffer = (char*)malloc(strlen(buf) + 1);
    if (!newbuffer)
        return;
    Utils::StrCpy(newbuffer, strlen(buf) + 1, buf);

    /* Insert file */
    while (*r)
        r = &(*r)->next;
    *r = (Optimizer::LIST*)malloc(sizeof(Optimizer::LIST));
    if (!*r)
        return;
    (*r)->next = 0;
    (*r)->data = newbuffer;
}

/*-------------------------------------------------------------------------*/

int InsertExternalFile(const char* name, bool)
{
    char buf[260];
    const char* p;

    if (Utils::HasExt(name, ".rc"))
    {
        InsertFile(&reslist, name, ".res");
        InsertFile(&rclist, name, nullptr);
        return 1;
    }
    else if (Utils::HasExt(name, ".res"))
    {
        InsertFile(&reslist, name, nullptr);
        return 1;
    }
    else if (Utils::HasExt(name, ".ilo"))
    {
        InsertFile(&objlist, name, nullptr);
        return 1;
    }
    p = strrchr(name, '\\');
    if (!p)
        p = name;
    else
        p++;
    Utils::StrCpy(buf, p);
    InsertFile(&objlist, buf, ".ilo");

    return 0; /* compiler should process it*/
}

/*-------------------------------------------------------------------------*/

void InsertOutputFileName(const char* name) { Utils::StrCpy(outFileName, name); }

/*-------------------------------------------------------------------------*/
static Optimizer::LIST* objPosition;
void GetOutputFileName(char* name, int len, char* path, int len2, bool obj)
{
    if (obj)
    {
        char* p;
        if (!objPosition)
            objPosition = objlist;
        if (!objPosition)
            Utils::Fatal("Cannot get object file name");
        Utils::StrCpy(name, len, outFileName);
        p = (char*)strrchr(name, '\\');
        if (!p)
            p = name;
        else
            p++;
        Utils::StrCpy(p, len - (p - name), (char*)objPosition->data);
        Utils::StrCpy(path, len2, name);
    }
    else
    {
        path[0] = 0;
        Utils::StrCpy(name, len, outFileName);
        if (objlist && name[0] && name[strlen(name) - 1] == '\\')
        {
            Utils::StrCat(name, len, (char*)objlist->data);
            Utils::StripExt(name);
            Utils::StrCat(name, len, ".exe");
            Utils::StrCpy(path, len2, outFileName);
        }
    }
}
void NextOutputFileName()
{
    if (objPosition)
        objPosition = objPosition->next;
}
int RunExternalFiles()
{
    return 0;
}
}  // namespace occmsil