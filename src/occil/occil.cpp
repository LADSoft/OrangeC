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

/*
 * C configuration when we are using ICODE code generator output
 */
#include <cstdio>
#include <cstring>
#include <ctime>
#include "be.h"
#include "beinterfdefs.h"
#include "Utils.h"
#include "winmode.h"
#include "CmdSwitch.h"
#include "ildata.h"
#include "SharedMemory.h"
#include "DotNetPELib.h"
#include "config.h"
#include "ildata.h"
#include "iblock.h"
#include "output.h"
#include "msilInit.h"
#include "using.h"
#include "igen.h"
#include "invoke.h"
#include "MsilProcess.h"
#include "gen.h"
#include "ilunstream.h"

Optimizer::SimpleSymbol* currentFunction;
int usingEsp;
void regInit() {}
void diag(const char* fmt, ...) {}
void Import() {}
namespace Parser
{
int anonymousNotAlloc;
bool IsCompiler() { return true; }
}  // namespace Parser
namespace Optimizer
{
unsigned termCount;
};

using namespace DotNetPELib;
PELib* peLib;

namespace occmsil
{
CmdSwitchParser SwitchParser;
CmdSwitchBool single(SwitchParser, 's', false, {"single"});

const char* usageText =
    "[options] inputfile\n"
    "\n"
    "--single     don't open internal file list\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;

char infile[260];
char outFile[260];

int dbgblocknum;

static const char* occil_verbosity = nullptr;

static const int MAX_SHARED_REGION = 240 * 1024 * 1024;

void flush_peep() {}

void ResolveMSILExterns()
{
    for (auto&& sp : Optimizer::externals)
    {
        if (sp->msil)
        {
            if (sp->tp->type == Optimizer::st_func)
            {
                std::vector<Optimizer::SimpleSymbol*> params;
                for (auto v = sp->syms; v != nullptr; v = v->next)
                {
                    Optimizer::SimpleSymbol* sym = (Optimizer::SimpleSymbol*)v->data;
                    if (!sym->thisPtr && sym->tp->type != Optimizer::st_void)
                        params.push_back(sym);
                }
                std::map<Optimizer::SimpleSymbol*, Param*, byName> paramList;
                LoadParams(sp, params, paramList);

                std::vector<Type*> types;
                for (auto v : paramList)
                {
                    types.push_back(v.second->GetType());
                }
                Method* rv = nullptr;
                peLib->Find(sp->msil, &rv, types, nullptr, nullptr, nullptr, true);
                if (rv)
                    sp->msil = (const char*)rv;
                else
                    sp->msil = nullptr;  // error!!!!!!!
            }
            else  // field
            {
                void* rv = nullptr;
                if (peLib->Find(sp->msil, &rv) == PELib::s_field)
                {
                    sp->msil = (const char*)rv;
                }
                else
                {
                    sp->msil = nullptr;  ////   errror!!!!!!!
                }
            }
        }
    }
    for (auto it = Optimizer::externals.begin(); it != Optimizer::externals.end();)
    {
        if ((*it)->msil)
            it = Optimizer::externals.erase(it);
        else
            ++it;
    }
}
void outputfile(char* buf, const char* name, const char* ext)
{
    strcpy(buf, Optimizer::outputFileName.c_str());
    if (buf[strlen(buf) - 1] == '\\')
    {
        // output file is a path specification rather than a file name
        // just add our name and ext
        strcat(buf, name);
        Utils::StripExt(buf);
        Utils::AddExt(buf, ext);
    }
    else if (buf[0] == 0)  // no output file specified, put the output wherever the input was...
    {
        strcpy(buf, name);
        char* p = (char*)strrchr(buf, '\\');
        char* q = (char*)strrchr(buf, '/');
        if (q > p)
            p = q;
        if (p)
            strcpy(buf, p + 1);
        Utils::StripExt(buf);
        Utils::AddExt(buf, ext);
    }
    else
    {
        // use specified output file name
    }
}

void global(Optimizer::SimpleSymbol* sym, int flags) {}
void ProcessData(Optimizer::BaseData* v)
{
    switch (v->type)
    {
        case Optimizer::DT_SEG:
            msil_oa_enterseg((Optimizer::e_sg)v->i);
            break;
        case Optimizer::DT_SEGEXIT:
            break;
        case Optimizer::DT_DEFINITION:
            if (v->symbol.sym->tp->type == Optimizer::st_func)
                currentFunction = v->symbol.sym;
            msil_oa_gen_strlab(v->symbol.sym);
            global(v->symbol.sym, v->symbol.i);
            if (v->symbol.sym->tp->type == Optimizer::st_func)
                currentFunction = nullptr;
            break;
        case Optimizer::DT_LABELDEFINITION:
            msil_oa_put_string_label(v->i, 0);
            break;
        case Optimizer::DT_RESERVE:
            // msil_oa_genstorage(v->i);
            break;
        case Optimizer::DT_SYM:
            // msil_oa_genref(v->symbol.sym, v->symbol.i);
            break;
        case Optimizer::DT_SRREF:
            msil_oa_gensrref(v->symbol.sym, v->symbol.i, 0);
            break;
        case Optimizer::DT_PCREF:
            // msil_oa_genpcref(v->symbol.sym, v->symbol.i);
            break;
        case Optimizer::DT_FUNCREF:
            // Optimizer::gen_funcref(v->symbol.sym);
            // global(v->symbol.sym, v->symbol.i);
            break;
        case Optimizer::DT_LABEL:
            Optimizer::gen_labref(v->i);
            break;
        case Optimizer::DT_LABDIFFREF:
            Optimizer::gen_labdifref(v->diff.l1, v->diff.l2);
            break;
        case Optimizer::DT_STRING:
            msil_oa_genstring(v->astring.str, v->astring.i);
            break;
        case Optimizer::DT_BIT:
            break;
        case Optimizer::DT_BOOL:
            //        msil_oa_genint(chargen, v->i);
            break;
        case Optimizer::DT_BYTE:
            //        msil_oa_genint(chargen, v->i);
            break;
        case Optimizer::DT_USHORT:
            //        msil_oa_genint(shortgen, v->i);
            break;
        case Optimizer::DT_UINT:
            //        msil_oa_genint(intgen, v->i);
            break;
        case Optimizer::DT_ULONG:
            //        msil_oa_genint(longgen, v->i);
            break;
        case Optimizer::DT_ULONGLONG:
            //        msil_oa_genint(longlonggen, v->i);
            break;
        case Optimizer::DT_16:
            //        msil_oa_genint(u16gen, v->i);
            break;
        case Optimizer::DT_32:
            //       msil_oa_genint(u32gen, v->i);
            break;
        case Optimizer::DT_ENUM:
            //        msil_oa_genint(intgen, v->i);
            break;
        case Optimizer::DT_FLOAT:
            //        msil_oa_genfloat(floatgen, &v->f);
            break;
        case Optimizer::DT_DOUBLE:
            //        msil_oa_genfloat(doublegen, &v->f);
            break;
        case Optimizer::DT_LDOUBLE:
            //        msil_oa_genfloat(longdoublegen, &v->f);
            break;
        case Optimizer::DT_CFLOAT:
            //        msil_oa_genfloat(floatgen, &v->c.r);
            //        msil_oa_genfloat(floatgen, &v->c.i);
            break;
        case Optimizer::DT_CDOUBLE:
            //      msil_oa_genfloat(doublegen, &v->c.r);
            //        msil_oa_genfloat(doublegen, &v->c.i);
            break;
        case Optimizer::DT_CLONGDOUBLE:
            //        msil_oa_genfloat(longdoublegen, &v->c.r);
            //        msil_oa_genfloat(longdoublegen, &v->c.i);
            break;
        case Optimizer::DT_ADDRESS:
            Optimizer::genaddress(v->i);
            break;
        case Optimizer::DT_VIRTUAL:
            //        msil_oa_gen_virtual(v->symbol.sym, v->symbol.i);
            break;
        case Optimizer::DT_ENDVIRTUAL:
            //        msil_oa_gen_endvirtual(v->symbol.sym);
            break;
        case Optimizer::DT_ALIGN:
            //        msil_oa_align(v->i);
            break;
        case Optimizer::DT_VTT:
            msil_oa_gen_vtt(v->symbol.i, v->symbol.sym);
            break;
        case Optimizer::DT_IMPORTTHUNK:
            msil_oa_gen_importThunk(v->symbol.sym);
            break;
        case Optimizer::DT_VC1:
            msil_oa_gen_vc1(v->symbol.sym);
            break;
        case Optimizer::DT_AUTOREF:
            //        msil_oa_gen_int(0);
            break;
    }
}
bool ProcessData(const char* name)
{
    for (auto v : Optimizer::baseData)
    {
        if (v->type == Optimizer::DT_FUNC)
        {
            Optimizer::temporarySymbols = v->funcData->temporarySymbols;
            Optimizer::functionVariables = v->funcData->variables;
            //            blockCount = v->funcData->blockCount;
            //            exitBlock = v->funcData->exitBlock;
            //            tempCount = v->funcData->tempCount;
            //            functionHasAssembly = v->funcData->hasAssembly;
            Optimizer::intermed_head = v->funcData->instructionList;
            Optimizer::intermed_tail = Optimizer::intermed_head;
            while (Optimizer::intermed_tail && Optimizer::intermed_tail->fwd)
                Optimizer::intermed_tail = Optimizer::intermed_tail->fwd;
            Optimizer::fltexp = v->funcData->fltexp;
            Optimizer::fastcallAlias = v->funcData->fastcallAlias;
            currentFunction = v->funcData->name;
            // Optimizer::SetUsesESP(currentFunction->usesEsp);
            generate_instructions(Optimizer::intermed_head);
            msil_flush_peep(currentFunction, nullptr);
            currentFunction = nullptr;
        }
        else
        {
            ProcessData(v);
        }
    }
    return true;
}

bool LoadFile(SharedMemory* parserMem, std::string fileName)
{
    Optimizer::InitIntermediate();
    bool rv = Optimizer::InputIntermediate(parserMem);
    Optimizer::SelectBackendData();
    Optimizer::oinit();
    Optimizer::SelectBackendData();
    if (fileName.empty())
    {
        for (auto&& s : Optimizer::prm_Using)
            _add_global_using(s.c_str());
    }
    if (fileName.empty() && Optimizer::inputFiles.size())
    {
        if (!Optimizer::cparams.prm_compileonly || Optimizer::cparams.prm_asmfile)
        {
            if (!Optimizer::outputFileName.empty())
                outputfile(outFile, Optimizer::outputFileName.c_str(), Optimizer::chosenAssembler->objext);
            else
                outputfile(outFile, Optimizer::inputFiles.front().c_str(), Optimizer::chosenAssembler->objext);
            InsertExternalFile(outFile, false);
        }
        fileName = Optimizer::inputFiles.front();
    }
    msil_main_preprocess((char*)fileName.c_str());
    ResolveMSILExterns();
    return rv;
}
bool SaveFile(const char* name)
{
    if (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile)
    {
        strcpy(infile, name);
        outputfile(outFile, name, Optimizer::chosenAssembler->objext);
        InsertExternalFile(outFile, false);
        Optimizer::outputFile = fopen(outFile, "wb");
        if (!Optimizer::outputFile)
            return false;
        for (auto v : Optimizer::externals)
        {
            if (v)
            {
                msil_oa_put_extern(v, 0);
            }
        }
        //        msil_oa_setalign(2, dataAlign, bssAlign, constAlign);
        msil_end_generation(outFile);
        fclose(Optimizer::outputFile);
        Optimizer::outputFile = nullptr;
    }
    return true;
}

int InvokeParser(int argc, char** argv, SharedMemory* parserMem)
{
    std::string args;
    for (int i = 1; i < argc; i++)
    {
        if (args.size())
            args += " ";
        std::string curArg = argv[i];
        if (curArg[curArg.size() - 1] == '\\')
            curArg += "\\";
        args += std::string("\"") + curArg + "\"";
    }

    return Utils::ToolInvoke("occparse", occil_verbosity, "-! --architecture \"msil;%s\" %s", parserMem->Name().c_str(),
                             args.c_str());
}
int InvokeOptimizer(SharedMemory* parserMem, SharedMemory* optimizerMem)
{
    return Utils::ToolInvoke("occopt", occil_verbosity, "-! -S %s %s", parserMem->Name().c_str(), optimizerMem->Name().c_str());
}
}  // namespace occmsil
int main(int argc, char* argv[])
{
    using namespace occmsil;
    bool showBanner = true;
    for (int i = 0; i < argc; i++)
        if (argv[i][0] == '-' || argv[i][0] == '/')
            if (!strcmp(&argv[i][1], "M") || !strcmp(&argv[i][1], "MM"))
            {
                showBanner = false;
            }
    if (showBanner)
        Utils::banner(argv[0]);
    fflush(stdout);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    unsigned startTime, stopTime;

    if (!Utils::HasLocalExe("occopt") || !Utils::HasLocalExe("occparse"))
    {
        Utils::fatal("cannot find 'occopt.exe' or 'occparse.exe'");
    }

    for (auto p = argv; *p; p++)
    {
        if (strstr(*p, "/y") || strstr(*p, "-y"))
            occil_verbosity = "";
    }
    auto optimizerMem = new SharedMemory(MAX_SHARED_REGION);
    optimizerMem->Create();
    int rv = 0;
    if (argc == 2 && Utils::HasExt(argv[1], ".icf"))
    {
        FILE* fil = fopen(argv[1], "rb");
        if (fil)
        {
            Optimizer::ReadMappingFile(optimizerMem, fil);
            fclose(fil);
        }
        else
        {
            Utils::fatal("cannot open input file");
        }
    }
    else
    {
        auto parserMem = new SharedMemory(MAX_SHARED_REGION);
        parserMem->Create();
        rv = InvokeParser(argc, argv, parserMem);
        if (!rv)
            rv = InvokeOptimizer(parserMem, optimizerMem);
        delete parserMem;
    }
    if (!rv)
    {
        char fileName[260];
        if (!LoadFile(optimizerMem, ""))
        {
            Utils::fatal("internal error: could not load intermediate file");
        }
        if (Optimizer::cparams.prm_displaytiming)
        {
            startTime = clock();
        }
        for (auto f : Optimizer::backendFiles)
        {
            InsertExternalFile(f.c_str(), false);
        }
        std::list<std::string> files = Optimizer::inputFiles;
        if (files.size())
        {
            if (!ProcessData(files.front().c_str()) || !SaveFile(files.front().c_str()))
                Utils::fatal("File I/O error");
            files.pop_front();
        }
        if (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile)
        {
            for (auto p : files)
            {
                if (!LoadFile(optimizerMem, p))
                    Utils::fatal("internal error: could not load intermediate file");
                if (!ProcessData(p.c_str()))
                    Utils::fatal("File I/O error");
                if (!SaveFile(p.c_str()))
                    Utils::fatal("Cannot open '%s' for write", outFile);
            }
        }
        if (Optimizer::cparams.prm_displaytiming)
        {
            stopTime = clock();
            printf("occ timing: %d.%03d\n", (stopTime - startTime) / 1000, (stopTime - startTime) % 1000);
        }
        if (!Optimizer::cparams.prm_compileonly)
        {
            rv = RunExternalFiles();
        }
        if (!rv)
            msil_main_postprocess(false);
    }
    if (rv == 255)
        rv = 0;
    return rv;
}
