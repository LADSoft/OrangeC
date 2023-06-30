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
#include "CmdSwitch.h"
#include "ioptimizer.h"
#include "ildata.h"

#include "config.h"
#include "ildata.h"
#include "occ.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif
namespace occx86
{

static const char* winflags[] = {
    "/T:CON32 ", "/T:GUI32 ", "/T:DLL32 ", "/T:PM ", "/T:DOS32 ", "/T:BIN ", "/T:CON32;sdpmist32.bin ", "/T:CON32;shdld32.bin ",
};
static const char* winc0[] = {"c0xpe.o", "c0pe.o", "c0dpe.o", "c0pm.o", "c0wat.o", "", "c0xpe.o", "c0hx.o",
                              "c0xls.o", "c0ls.o", "c0dls.o", "c0om.o", "c0wat.o", "", "c0xpe.o", "c0hx.o"};

static Optimizer::LIST *objlist, *asmlist, *liblist, *reslist, *rclist;
static char *asm_params, *rc_params, *link_params;

bool InsertOption(const char* name)
{
    char** p;
    switch (name[0])
    {
        case 'a':
            p = &asm_params;
            break;
        case 'r':
            p = &rc_params;
            break;
        case 'l':
            p = &link_params;
            break;
        default:
            return false;
    }
    int len = 0;
    if (*p)
        len = strlen(*p);
    *p = (char*)realloc(*p, 2 + len + strlen(name + 1));
    (*p)[len] = 0;
    strcat(*p, " ");
    strcat(*p, name + 1);
    return true;
}
static void InsertFile(Optimizer::LIST** r, const char* name, const char* ext, bool primary)
{
    Optimizer::LIST* lst;
    char buf[256], *newbuffer;
    strcpy(buf, name);
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

int InsertExternalFile(const char* name, bool primary)
{
    char buf[260];
    const char* p;
    if (Utils::HasExt(name, ".asm") || Utils::HasExt(name, ".nas") || Utils::HasExt(name, ".s"))
    {
        InsertFile(&objlist, name, ".o", primary);
        InsertFile(&asmlist, name, 0, primary);
        return 1; /* compiler shouldn't process it*/
    }
    else if (Utils::HasExt(name, ".l") || Utils::HasExt(name, ".a") || Utils::HasExt(name, ".lib") || Utils::HasExt(name, ".dll"))
    {
        InsertFile(&liblist, name, 0, primary);
        return 1;
    }
    else if (Utils::HasExt(name, ".rc"))
    {
        InsertFile(&reslist, name, ".res", primary);
        InsertFile(&rclist, name, 0, primary);
        return 1;
    }
    else if (Utils::HasExt(name, ".res"))
    {
        InsertFile(&reslist, name, 0, primary);
        return 1;
    }
    else if (Utils::HasExt(name, ".o"))
    {
        InsertFile(&objlist, name, 0, primary);
        return 1;
    }
    p = strrchr(name, '\\');
    if (!p)
        p = strrchr(name, '/');
    if (!p)
        p = name;
    else
        p++;
    strcpy(buf, p);
    InsertFile(&objlist, buf, ".o", primary);

    // compiling via assembly
    if (Optimizer::cparams.prm_asmfile && !Optimizer::cparams.prm_compileonly)
    {
        InsertFile(&asmlist, buf, ".asm", primary);
    }
    return 0; /* compiler should process it*/
}

/*-------------------------------------------------------------------------*/

int RunExternalFiles()
{
    char args[1024];
    const char* c0;
    char outName[260], *p;
    int rv;
    char temp[260];
    int i;
    char verbosityString[20];

    memset(verbosityString, 0, sizeof(verbosityString));
    if (Optimizer::cparams.verbosity > 1)
    {
        verbosityString[0] = '-';
        memset(verbosityString + 1, 'y',
               Optimizer::cparams.verbosity > sizeof(verbosityString) - 2 ? sizeof(verbosityString) - 2
                                                                          : Optimizer::cparams.verbosity);
    }
    temp[0] = 0;
    if (Optimizer::inputFiles.size())
        outputfile(outName, Optimizer::inputFiles.front().c_str(), ".exe", false);
    else if (objlist)
        outputfile(outName, (const char*)objlist->data, ".exe", false);
    else
        strcpy(outName, "");
    //    p = strrchr(outName, '.');
    //    if (p && p[1] != '\\')
    //        *p = 0;
    bool first = false;
    while (asmlist && !Optimizer::cparams.prm_asmfile)
    {
        if (Optimizer::cparams.prm_compileonly && Optimizer::outputFileName[0] && !first)
            rv = Utils::ToolInvoke("oasm.exe", Optimizer::cparams.verbosity ? "" : nullptr, "\"-o%s\" %s %s \"%s\"",
                                   Optimizer::outputFileName.c_str(), asm_params ? asm_params : "",
                                   !Optimizer::showBanner ? "-!" : "", (char*)asmlist->data);
        else
            rv = Utils::ToolInvoke("oasm.exe", Optimizer::cparams.verbosity ? "" : nullptr, "%s %s \"%s\"",
                                   asm_params ? asm_params : "", !Optimizer::showBanner ? "-!" : "", (char*)asmlist->data);
        first = true;
        if (rv)
            return rv;
        asmlist = asmlist->next;
    }
    if (!Optimizer::prm_include.empty())
        sprintf(args, "\"-i%s\"", Optimizer::prm_include.c_str());
    else
        args[0] = 0;
    while (rclist)
    {
        if (Optimizer::cparams.prm_compileonly && Optimizer::outputFileName[0] && !first)
            rv = Utils::ToolInvoke("orc.exe", Optimizer::cparams.verbosity ? "" : nullptr, "\"-o%s\" -r %s %s %s \"%s\"",
                                   Optimizer::outputFileName.c_str(), rc_params ? rc_params : "",
                                   !Optimizer::showBanner ? "-!" : "", args, (char*)rclist->data);
        else
            rv = Utils::ToolInvoke("orc.exe", Optimizer::cparams.verbosity ? "" : nullptr, "-r %s %s %s \"%s\"",
                                   rc_params ? rc_params : "", !Optimizer::showBanner ? "-!" : "", args, (char*)rclist->data);
        first = true;
        if (rv)
            return rv;
        rclist = rclist->next;
    }
    if (!Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile && objlist)
    {
        char with[50000];
        with[0] = 0;
        std::string tempName;
        FILE* fil = Utils::TempName(tempName);
        if (Optimizer::cparams.prm_makelib)
        {
            while (objlist)
            {
                fprintf(fil, " \"%s%s\"", temp, (char*)objlist->data);
                objlist = objlist->next;
            }
            fclose(fil);
            if (Optimizer::cparams.verbosity)
            {
                FILE* fil = fopen(tempName.c_str(), "r");
                if (fil)
                {
                    sprintf(with, "with %s=\n", tempName.c_str());
                    while (fgets(with + strlen(with), 1000, fil) != 0)
                        ;
                    fclose(fil);
                    strcat(with, "\n");
                }
            }
            Utils::StripExt(outName);
            strcat(outName, ".l");
            rv = Utils::ToolInvoke(
                "olib.exe", Optimizer::cparams.verbosity ? with : nullptr, "%s %s +- @%s", 
                !Optimizer::showBanner ? "-!" : "", 
                outName,
                tempName.c_str());
        }
        else
        {
            if (!Optimizer::prm_libPath.empty())
            {
                fprintf(fil, "\"/L%s\" ", Optimizer::prm_libPath.c_str());
            }

            strcpy(args, winflags[Optimizer::cparams.prm_targettype]);

            c0 = winc0[Optimizer::cparams.prm_targettype + Optimizer::cparams.prm_lscrtdll * 8];
            if (Optimizer::cparams.prm_debug)
            {
                //            strcat(args, " /DEB");
                if (Optimizer::cparams.prm_targettype == DOS)
                    c0 = "c0pmd.o";
                else if (Optimizer::cparams.prm_targettype == DOS32A)
                    c0 = "c0watd.o";
                if (!Optimizer::cparams.compile_under_dos)  // this because I don't want to vet sqlite3 under DOS at this time.
                    strcat(args, " /v");
            }
            fprintf(fil, "  %s", c0);
            while (objlist)
            {
                fprintf(fil, " \"%s%s\"", temp, (char*)objlist->data);
                objlist = objlist->next;
            }
            //        fprintf(fil, "  \"/o%s\" ", outName);
            while (liblist)
            {
                fprintf(fil, " \"%s\"", (char*)liblist->data);
                liblist = liblist->next;
            }
            if (Optimizer::cparams.prm_msvcrt)
                fprintf(fil, " climp.l msvcrt.l ");
            else if (Optimizer::cparams.prm_lscrtdll)
                fprintf(fil, " climp.l lscrtl.l ");
            else if (Optimizer::cparams.prm_crtdll)
                fprintf(fil, " climp.l crtdll.l ");
            else if (Optimizer::cparams.prm_targettype == DOS || Optimizer::cparams.prm_targettype == DOS32A ||
                 Optimizer::cparams.prm_targettype == RAW || Optimizer::cparams.prm_targettype == WHXDOS)
            {
                if (Optimizer::cparams.prm_farkeyword)
                    fprintf(fil, " farmem");
                fprintf(fil, " cldos.l ");
            }
            else
               fprintf(fil, " climp.l clwin.l ");
            while (reslist)
            {
                fprintf(fil, " \"%s\"", (char*)reslist->data);
                reslist = reslist->next;
            }
            fclose(fil);
            if (Optimizer::cparams.verbosity)
            {
                FILE* fil = fopen(tempName.c_str(), "r");
                if (fil)
                {
                    sprintf(with, "with %s=\n", tempName.c_str());
                    while (fgets(with + strlen(with), 1000, fil) != 0)
                        ;
                    fclose(fil);
                    strcat(with, "\n");
                }
            }
            rv = Utils::ToolInvoke(
                "olink.exe", Optimizer::cparams.verbosity ? with : nullptr, "%s %s %s /c+ \"/o%s\" %s %s %s %s @%s",
                link_params ? link_params : "", Optimizer::cparams.prm_targettype == WHXDOS ? "-DOBJECTALIGN=65536" : "",
                !Optimizer::showBanner ? "-!" : "", outName, args, verbosityString,
                !Optimizer::prm_OutputDefFile.empty() ? ("--output-def \"" + Optimizer::prm_OutputDefFile + "\"").c_str() : "",
                !Optimizer::prm_OutputImportLibraryFile.empty() ? ("--out-implib \"" + Optimizer::prm_OutputImportLibraryFile + "\"").c_str() : "",
                tempName.c_str());
        }
        _unlink(tempName.c_str());
        if (Optimizer::cparams.verbosity > 1)
            printf("Return code: %d\n", rv);

        if (rv)
            return rv;
        if (Optimizer::cparams.prm_targettype == WHXDOS && !Optimizer::cparams.prm_makelib)
        {
            rv = Utils::ToolInvoke("patchpe.exe", Optimizer::cparams.verbosity ? "" : nullptr, "%s",
                                   Optimizer::outputFileName.c_str());
            if (rv)
            {
                printf("Could not spawn patchpe.exe\n");
            }
        }
        if (rv)
            return rv;
    }
    return 0;
}
}  // namespace occx86