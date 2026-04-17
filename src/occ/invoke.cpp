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
#include "CmdSwitch.h"
#include "ioptimizer.h"
#include "ildata.h"

#include "config.h"
#include "ildata.h"
#include "occ.h"
#include "ioptimizer.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#    define _unlink unlink
#else
#    include <io.h>
#endif
namespace occx86
{

static const char* winflags[] = {
    "/T:CON32 ", "/T:GUI32 ", "/T:DLL32 ", "/T:PM ", "/T:DOS32 ", "/T:BIN ", "/T:CON32;sdpmist32.bin ", "/T:CON32;shdld32.bin ",
};

static std::list<std::string> objlist, asmlist, liblist, reslist, rclist;
static std::string asm_params, rc_params, link_params;

bool InsertOption(const std::string& name)
{
    switch (name[0])
    {
        case 'a':
            asm_params += " " + name.substr(1);
            break;
        case 'r':
            rc_params += " " + name.substr(1);
            break;
        case 'l':
            link_params += " " + name.substr(1);
            break;
        default:
            return false;
    }
    return true;
}
bool IsCompilerSource(const char* buffer)
{
    bool found = false;
    static std::list<std::string> acceptedExtensions = {".c", ".cc", ".cpp", ".cxx"};
    for (auto& str : acceptedExtensions)
    {
        if (Utils::HasExt(buffer, str.c_str()) || Utils::iequal(buffer, str.c_str()))
        {
            found = true;
            break;
        }
    }
    return found;
}
bool IsAssemblerSource(const char* buffer)
{
    bool found = false;
    static std::list<std::string> acceptedExtensions = {".asm", ".s", ".nas"};
    for (auto& str : acceptedExtensions)
    {
        if (Utils::HasExt(buffer, str.c_str()) || Utils::iequal(buffer, str.c_str()))
        {
            found = true;
            break;
        }
    }
    return found;
}
static void InsertFile(std::list<std::string>& target, const std::string& name)
{
    int index = name.find_last_of("#");
    if (index >= 0)
        return;
    for (auto&& i : target)
    {

        if (Utils::iequal(i, name))
            return;
    }
    target.push_back(name);
}

/*-------------------------------------------------------------------------*/

int InsertExternalFile(const std::string& name)
{
    int index = name.find_last_of('#');
    if (index < 0)
        return 1; // shouldn't process, this is a safety net.
    std::string fileToCompile = name.substr(0, index);
    std::string as = name.substr(index + 1);
    std::string stem = fileToCompile;
    if (!as.empty() && (Utils::HasExt(stem.c_str(), as.c_str()) || (IsCompilerSource(fileToCompile.c_str()) && IsCompilerSource(("a" + as).c_str())) || (IsAssemblerSource(fileToCompile.c_str()) && IsAssemblerSource(("a" + as).c_str()))))
    {
        index = stem.find_last_of('.');
        stem = stem.substr(0, index);
    }
    if (!as.empty())
    {
        if (IsCompilerSource(("a" + as).c_str()))
        {
            index = stem.find_last_of("\\");
            if (index < 0)
                 index = stem .find_last_of("/");
            if (index > 0)
                 stem = stem.substr(index+1);
            // compiling via assembly
            if (Optimizer::cparams.prm_viaassembly && !Optimizer::cparams.prm_compileonly)
            {
                // if compiling via assembly we also have to assemble it
                InsertFile(asmlist, stem + ".s");
            }
            InsertFile(objlist, stem + ".o");
            return 1; /* compiler shouldn't process it*/
        }
        else if (IsAssemblerSource(("a" + as).c_str()))
        {
            InsertFile(objlist, stem + ".o");
            InsertFile(asmlist, fileToCompile);
            return 1; /* compiler shouldn't process it*/
        }
        else if (Utils::iequal(as, ".l") || Utils::iequal(as, ".a") || Utils::iequal(as, ".lib") ||
                 Utils::iequal(as, ".dll"))
        {
            InsertFile(liblist, fileToCompile);
            return 1;
        }
        else if (Utils::iequal(as, ".rc"))
        {
            // if compiling an RC file we also have to link its res file
            InsertFile(reslist, stem + ".res");
            InsertFile(rclist, fileToCompile);
            return 1;
        }
        else if (Utils::iequal(as, ".res"))
        {
            InsertFile(reslist, fileToCompile);
            return 1;
        }
        else if (Utils::iequal(as, ".o"))
        {
            InsertFile(objlist, fileToCompile);
            return 1;
        }
    }
    // no extenstion or an unknown one, just use the name intact (sans directory specification)
    index = fileToCompile.find_last_of("\\");
    if (index < 0)
        index = fileToCompile .find_last_of("/");
    if (index > 0)
        fileToCompile = fileToCompile.substr(index+1);
    InsertFile(objlist, fileToCompile);
    
    return 0; /* compiler should process it*/
}

/*-------------------------------------------------------------------------*/

int RunExternalFiles()
{
    char args[40000];
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
    if (Optimizer::inputFiles.size())
    {
        outputfile(outName, sizeof(outName), Optimizer::inputFiles.front().c_str(), ".exe", false);
    }
    else if (objlist.size())
    {
        Utils::StrCpy(temp, objlist.front().c_str());
        Utils::StripExt(temp);
        outputfile(outName, sizeof(outName), temp, ".exe", false);
    }
    else
        Utils::StrCpy(outName, "");
    temp[0] = 0;
    //    p = strrchr(outName, '.');
    //    if (p && p[1] != '\\')
    //        *p = 0;
    bool first = false;
    if (!Optimizer::cparams.prm_asmfile || Optimizer::cparams.prm_viaassembly)
    {
        for (auto&& a : asmlist)
        {
            if (Optimizer::cparams.prm_compileonly && Optimizer::outputFileName[0] && !first)
                rv = ToolChain::ToolInvoke("oasm.exe", Optimizer::cparams.verbosity ? "" : nullptr, "\"-o%s\" %s %s \"%s\"",
                                           Optimizer::outputFileName.c_str(), asm_params.c_str(),
                                           !Optimizer::showBanner ? "-!" : "", a.c_str());
            else
                rv = ToolChain::ToolInvoke("oasm.exe", Optimizer::cparams.verbosity ? "" : nullptr, "%s %s \"%s\"",
                                           asm_params.c_str(), !Optimizer::showBanner ? "-!" : "", a.c_str());
            first = true;
            if (rv)
                return rv;
        }
    }
    if (!Optimizer::prm_include.empty())
        sprintf(args, "\"-i%s\"", Optimizer::prm_include.c_str());
    else
        args[0] = 0;
    for (auto&& r : rclist)
    {
        if (Optimizer::cparams.prm_compileonly && Optimizer::outputFileName[0] && !first)
            rv = ToolChain::ToolInvoke("orc.exe", Optimizer::cparams.verbosity ? "" : nullptr, "\"-o%s\" -r %s %s %s \"%s\"",
                                       Optimizer::outputFileName.c_str(), rc_params.c_str(),
                                       !Optimizer::showBanner ? "-!" : "", args, r.c_str());
        else
            rv = ToolChain::ToolInvoke("orc.exe", Optimizer::cparams.verbosity ? "" : nullptr, "-r %s %s %s \"%s\"",
                                       rc_params.c_str(), !Optimizer::showBanner ? "-!" : "", args, r.c_str());
        first = true;
        if (rv)
            return rv;
    }
    if (!Optimizer::cparams.prm_compileonly && (!Optimizer::cparams.prm_asmfile || Optimizer::cparams.prm_viaassembly) && objlist.size())
    {
        char with[50000];
        with[0] = 0;
        std::string tempName;
        FILE* fil = Utils::TempName(tempName);
        if (Optimizer::cparams.prm_makelib)
        {
            for (auto &&o : objlist)
            {
                fprintf(fil, " \"%s%s\"", temp, o.c_str());
            }
            fclose(fil);
            if (Optimizer::cparams.verbosity)
            {
                FILE* fil = fopen(tempName.c_str(), "r");
                if (fil)
                {
                    sprintf(with, "%s=\n", tempName.c_str());
                    while (fgets(with + strlen(with), 1000, fil) != 0)
                        ;
                    fclose(fil);
                    Utils::StrCat(with, "\n");
                }
            }
            Utils::StripExt(outName);
            Utils::StrCat(outName, ".l");
            rv = ToolChain::ToolInvoke("olib.exe", Optimizer::cparams.verbosity ? with : nullptr, "%s \"%s\" +- @%s",
                                       !Optimizer::showBanner ? "-!" : "", outName, tempName.c_str());
        }
        else
        {
            if (!Optimizer::prm_libPath.empty())
            {
                fprintf(fil, "\"/L%s\" ", Optimizer::prm_libPath.c_str());
            }

            Utils::StrCpy(args, winflags[Optimizer::cparams.prm_targettype]);

            if (Optimizer::cparams.prm_debug)
            {
                if (!Optimizer::cparams.compile_under_dos)  // this because I don't want to vet sqlite3 under DOS at this time.
                    Utils::StrCat(args, " /g /DFIXED=1");
            }
            for (auto&& o : objlist)
            {
                fprintf(fil, " \"%s%s\"", temp, o.c_str());
            }
            //        fprintf(fil, "  \"/o%s\" ", outName);
            for (auto&& lib : liblist)
            {
                fprintf(fil, " \"%s\"", lib.c_str());
            }
            for (auto&& s : Utils::split(Optimizer::specifiedLibs))
            {
                fprintf(fil, " \"-l%s\"", s.c_str());
            }
            if (Optimizer::cparams.prm_msvcrt)
                fprintf(fil, " climp.l msvcrt.l ");
            else if (Optimizer::cparams.prm_lscrtdll)
                fprintf(fil, " climp.l lscrtl.l ");
            else if (Optimizer::cparams.prm_crtdll)
                fprintf(fil, " climp.l crtdll.l ");
            else
                fprintf(fil, " climp.l clwin.l ");
            for (auto && r : reslist)
            {
                fprintf(fil, " \"%s\"", r.c_str());
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
                    Utils::StrCat(with, "\n");
                }
            }
            rv = ToolChain::ToolInvoke(
                "olink.exe", Optimizer::cparams.verbosity ? with : nullptr, "%s %s %s /c+ \"/o%s\" %s %s %s %s %s %s @%s",
                link_params.c_str(), Optimizer::cparams.prm_targettype == WHXDOS ? "-DOBJECTALIGN=65536" : "",
                !Optimizer::showBanner ? "-!" : "", outName, args, verbosityString,
                !Optimizer::prm_OutputDefFile.empty() ? ("--output-def \"" + Optimizer::prm_OutputDefFile + "\"").c_str() : "",
                !Optimizer::prm_OutputImportLibraryFile.empty()
                    ? ("--out-implib \"" + Optimizer::prm_OutputImportLibraryFile + "\"").c_str()
                    : "",
                (Optimizer::cparams.prm_stackprotect & HEAP_CHECK) ? "-D__HEAP_CHECK=1" : "", 
                (Optimizer::cparams.prm_exportAll) ? "--export-all-symbols" : "", 
                tempName.c_str());
        }
        _unlink(tempName.c_str());
        if (Optimizer::cparams.verbosity > 1)
            printf("Return code: %d\n", rv);

        if (rv)
            return rv;
        if (Optimizer::cparams.prm_targettype == WHXDOS && !Optimizer::cparams.prm_makelib)
        {
            rv = ToolChain::ToolInvoke("patchpe.exe", Optimizer::cparams.verbosity ? "" : nullptr, "%s",
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