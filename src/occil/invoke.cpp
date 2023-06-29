/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "be.h"
#include "winmode.h"
#include "Utils.h"
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
    strcpy(buf, name);
    if (!outFileName[0])
    {
        strcpy(outFileName, name);
        Utils::StripExt(outFileName);
        strcat(outFileName, Optimizer::cparams.prm_targettype == DLL ? ".dll" : ".exe");
    }
    if (ext)
    {
        Utils::StripExt(buf);
        strcat(buf, ext);
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
    strcpy(newbuffer, buf);

    /* Insert file */
    while (*r)
        r = &(*r)->next;
    *r = (Optimizer::LIST*)malloc(sizeof(Optimizer::LIST));
    if (!r)
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
    strcpy(buf, p);
    InsertFile(&objlist, buf, ".ilo");

    return 0; /* compiler should process it*/
}

/*-------------------------------------------------------------------------*/

void InsertOutputFileName(const char* name) { strcpy(outFileName, name); }

/*-------------------------------------------------------------------------*/
static Optimizer::LIST* objPosition;
void GetOutputFileName(char* name, char* path, bool obj)
{
    if (obj)
    {
        char* p;
        if (!objPosition)
            objPosition = objlist;
        if (!objPosition)
            Utils::fatal("Cannot get object file name");
        strcpy(name, outFileName);
        p = (char*)strrchr(name, '\\');
        if (!p)
            p = name;
        else
            p++;
        strcpy(p, (char*)objPosition->data);
        strcpy(path, name);
    }
    else
    {
        path[0] = 0;
        strcpy(name, outFileName);
        if (objlist && name[0] && name[strlen(name) - 1] == '\\')
        {
            strcat(name, (char*)objlist->data);
            Utils::StripExt(name);
            strcat(name, ".exe");
            strcpy(path, outFileName);
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
    char args[1024], *c0;
    char spname[2048];
    char outName[260], *p;
    int rv;
    char temp[260];
    return 0;

    GetOutputFileName(outName, temp, false);
    Utils::StripExt(outName);
    Utils::AddExt(outName, ".il");
    while (rclist)
    {
        rv = Utils::ToolInvoke("orc.exe", nullptr, " -r %s \"%s\"", !Optimizer::showBanner ? "-!" : "", (char*)rclist->data);
        if (rv)
            return rv;
        rclist = rclist->next;
    }
    if (objlist)
    {
        std::string sdebug = Optimizer::cparams.prm_debug ? "/DEBUG" : "";
        std::string starget = Optimizer::cparams.prm_targettype == DLL ? "/DLL" : "";
        std::string resources;
        while (reslist)
        {
            resources += std::string(" /Resource:\"") + (char*)reslist->data + "\"";
            reslist = reslist->next;
        }
        sprintf(spname, "ilasm.exe /QUIET %s %s \"%s%s\" %s", sdebug.c_str(), starget.c_str(), temp, outName, resources.c_str());
        rv = system(spname);

        if (rv)
        {
            printf("assembly failed");
            return rv;
        }
    }
    return 0;
}
}  // namespace occmsil