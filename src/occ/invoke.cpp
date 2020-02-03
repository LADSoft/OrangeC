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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "be.h"
#include "winmode.h"
#include "Utils.h"
#include "CmdSwitch.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif

extern bool compile_under_dos;

extern COMPILER_PARAMS cparams;
extern int showBanner;
extern int verbosity;
extern std::string prm_libPath;
extern std::string prm_include;
extern std::string prm_OutputDefFile;
extern std::string outputFileName;
extern std::list<std::string> inputFiles;

const char* winflags[] = {
    "/T:CON32 ", "/T:GUI32 ", "/T:DLL32 ", "/T:PM ", "/T:DOS32 ", "/T:BIN ", "/T:CON32;sdpmist32.bin ", "/T:CON32;shdld32.bin ",
};
const char* winc0[] = {"c0xpe.o", "c0pe.o", "c0dpe.o", "c0pm.o", "c0wat.o", "", "c0xpe.o", "c0hx.o",
                       "c0xls.o", "c0ls.o", "c0dls.o", "c0om.o", "c0wat.o", "", "c0xpe.o", "c0hx.o"};

LIST *objlist, *asmlist, *liblist, *reslist, *rclist;
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
static void InsertFile(LIST** r, const char* name, const char* ext, bool primary)
{
    LIST* lst;
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
    *r = (LIST*)malloc(sizeof(LIST));
    if (!r)
        return;
    (*r)->next = 0;
    (*r)->data = newbuffer;
}

/*-------------------------------------------------------------------------*/

int InsertExternalFile(const char* name, bool primary)
{
    char buf[260];
    const char *p;
    if (Utils::HasExt(name, ".asm") || Utils::HasExt(name, ".nas") || Utils::HasExt(name, ".s"))
    {
        InsertFile(&objlist, name, ".o", primary);
        InsertFile(&asmlist, name, 0, primary);
        return 1; /* compiler shouldn't process it*/
    }
    else if (Utils::HasExt(name, ".l") || Utils::HasExt(name, ".a") || Utils::HasExt(name, ".lib"))
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
    if (cparams.prm_asmfile && !cparams.prm_compileonly)
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
    if (verbosity > 1)
    {
        verbosityString[0] = '-';
        memset(verbosityString + 1, 'y', verbosity > sizeof(verbosityString) - 2 ? sizeof(verbosityString) - 2 : verbosity);
    }
    temp[0] = 0;
    if (inputFiles.size())
        outputfile(outName, inputFiles.front().c_str(), ".exe", false);
    else
        outputfile(outName, (const char *)objlist->data, ".exe", false);
    //    p = strrchr(outName, '.');
    //    if (p && p[1] != '\\')
    //        *p = 0;
    bool first = false;
    while (asmlist && !cparams.prm_asmfile)
    {
        if (cparams.prm_compileonly && outputFileName[0] && !first)
            rv = Utils::ToolInvoke("oasm.exe", verbosity?"":nullptr, "\"-o%s\" %s %s \"%s\"", outputFileName.c_str(), asm_params ? asm_params : "",
                    !showBanner ? "-!" : "", (char*)asmlist->data);
        else
            rv = Utils::ToolInvoke("oasm.exe", verbosity ? "" : nullptr, "%s %s \"%s\"", asm_params ? asm_params : "", !showBanner ? "-!" : "",
                    (char*)asmlist->data);
        first = true;
        if (rv)
            return rv;
        asmlist = asmlist->next;
    }
    if (!prm_include.empty())
        sprintf(args, "\"-i%s\"", prm_include.c_str());
    else
        args[0] = 0;
    while (rclist)
    {
        if (cparams.prm_compileonly && outputFileName[0] && !first)
            rv = Utils::ToolInvoke("orc.exe", verbosity ? "" : nullptr, "\"-o%s\" -r %s %s %s \"%s\"", outputFileName.c_str(), rc_params ? rc_params : "",
                    !showBanner ? "-!" : "", args, (char*)rclist->data);
        else
            rv = Utils::ToolInvoke("orc.exe", verbosity ? "" : nullptr, "-r %s %s %s \"%s\"", rc_params ? rc_params : "", !showBanner ? "-!" : "", args,
                    (char*)rclist->data);
        first = true;
        if (rv)
            return rv;
        rclist = rclist->next;
    }
    if (!cparams.prm_compileonly && !cparams.prm_asmfile && objlist)
    {
        std::string tempName;
        FILE *fil = Utils::TempName(tempName);
        if (!prm_libPath.empty())
        {
            fprintf(fil, "\"/L%s\" ", prm_libPath.c_str());
        }

        strcpy(args, winflags[cparams.prm_targettype]);

        c0 = winc0[cparams.prm_targettype + cparams.prm_lscrtdll * 8];
        if (cparams.prm_debug)
        {
            //            strcat(args, " /DEB");
            if (cparams.prm_targettype == DOS)
                c0 = "c0pmd.o";
            else if (cparams.prm_targettype == DOS32A)
                c0 = "c0watd.o";
            if (!cparams.compile_under_dos)  // this because I don't want to vet sqlite3 under DOS at this time.
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
        if (cparams.prm_msvcrt)
            fprintf(fil, " climp.l msvcrt.l ");
        else if (cparams.prm_lscrtdll)
            fprintf(fil, " climp.l lscrtl.l ");
        else if (cparams.prm_crtdll)
            fprintf(fil, " climp.l crtdll.l ");
        else if (cparams.prm_targettype == DOS || cparams.prm_targettype == DOS32A || cparams.prm_targettype == RAW || cparams.prm_targettype == WHXDOS)
        {
            if (cparams.prm_farkeyword)
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
        char with[50000];
        with[0] = 0;
        if (verbosity)
        {
            FILE* fil = fopen(tempName.c_str(), "r");
            if (fil)
            {
                sprintf(with, "with %s=\n", tempName.c_str());
                while (fgets(with + strlen(with), 1000, fil) != 0);
                fclose(fil);
                strcat(with, "\n");
            }
        }
        rv = Utils::ToolInvoke("olink.exe", verbosity?with:nullptr, "%s %s %s /c+ \"/o%s\" %s %s %s @%s", link_params ? link_params : "",
            cparams.prm_targettype == WHXDOS ? "-DOBJECTALIGN=65536" : "", !showBanner ? "-!" : "", outName, args, verbosityString, 
            !prm_OutputDefFile.empty() ? ("--output-def \"" + prm_OutputDefFile + "\"").c_str() : "", tempName.c_str());
        unlink(tempName.c_str());
        if (verbosity > 1)
            printf("Return code: %d\n", rv);

        if (rv)
            return rv;
        if (cparams.prm_targettype == WHXDOS)
        {
            rv = Utils::ToolInvoke("patchpe.exe", verbosity ? "" : nullptr, "%s", outputFileName.c_str());
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
