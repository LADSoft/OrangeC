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
#include <ctime>
#include "be.h"
#include "winmode.h"
#include "Utils.h"
#include "CmdSwitch.h"
#include "ildata.h"
#include "SharedMemory.h"
#include <sstream>
#include <iostream>
#include "../version.h"
#include "config.h"
#include "ildata.h"
#include "iblock.h"
#include "output.h"
#include "configx86.h"
#include "invoke.h"
#include "gen.h"
#include "outasm.h"
#include "peep.h"
#include "outcode.h"
#include "igen.h"
#include "ilunstream.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif

extern bool IsSymbolCharRoutine(const char*, bool);
bool (*Tokenizer::IsSymbolChar)(const char*, bool) = IsSymbolCharRoutine;

int usingEsp;
Optimizer::SimpleSymbol* currentFunction;

void diag(const char*, ...) {}
void regInit() {}

namespace Parser
{
int anonymousNotAlloc;
char outFile[260];
InstructionParser* instructionParser;
bool IsCompiler() { return true; }
}  // namespace Parser
namespace Optimizer
{
unsigned termCount;
void SymbolManager::clear() { globalSymbols.clear(); }
};  // namespace Optimizer

namespace occx86
{

char infile[260];

static const char* occ_verbosity = nullptr;
static Optimizer::FunctionData* lastFunc;

static const int MAX_SHARED_REGION = 240 * 1024 * 1024;

/*-------------------------------------------------------------------------*/

void outputfile(char* buf, const char* name, const char* ext, bool obj)
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
    else if (buf[0] == 0 || (obj && !Optimizer::cparams.prm_compileonly &&
                             !Optimizer::assembling))  // no output file specified, put the output wherever the input was...
    {
        strcpy(buf, name);
        char* p = (char*)strrchr(buf, '\\');
        char* q = (char*)strrchr(buf, '/');
        if (q > p)
            p = q;
        if (p)
            strcpy(buf, p + 1);
        Utils::StripExt(buf);
        strcat(buf, ext);
    }
    else
    {
        // use specified output file name
        if (obj)
            Utils::AddExt(buf, ext);
    }
}

void global(Optimizer::SimpleSymbol* sym, int flags)
{
    if (!sym->isinternal)
    {
        omf_globaldef(sym);
        if (Optimizer::cparams.prm_asmfile)
        {
            if (sym->storage_class != Optimizer::scc_localstatic && sym->storage_class != Optimizer::scc_static)
                Optimizer::bePrintf("[global %s]\n", sym->outputName);
        }

        if (flags & Optimizer::BaseData::DF_EXPORT)
        {
            oa_put_expfunc(sym);
        }
    }
    else
    {
        omf_localdef(sym);
    }
}
void ProcessData(Optimizer::BaseData* v)
{
    switch (v->type)
    {
        case Optimizer::DT_SEG:
            oa_enterseg((Optimizer::e_sg)v->i);
            break;
        case Optimizer::DT_SEGEXIT:
            break;
        case Optimizer::DT_DEFINITION:
            global(v->symbol.sym, v->symbol.i);
            oa_gen_strlab(v->symbol.sym);
            break;
        case Optimizer::DT_LABELDEFINITION:
            oa_put_label(v->i);
            break;
        case Optimizer::DT_RESERVE:
            oa_genstorage(v->i);
            break;
        case Optimizer::DT_SYM:
            oa_genref(v->symbol.sym, v->symbol.i);
            break;
        case Optimizer::DT_SRREF:
            oa_gensrref(v->symbol.sym, v->symbol.i, 0);
            break;
        case Optimizer::DT_PCREF:
            oa_genpcref(v->symbol.sym, v->symbol.i);
            break;
        case Optimizer::DT_FUNCREF:
            //        global(v->symbol.sym, v->symbol.i);
            break;
        case Optimizer::DT_LABEL:
            oa_gen_labref(v->i);
            break;
        case Optimizer::DT_LABDIFFREF:
            outcode_gen_labdifref(v->diff.l1, v->diff.l2);
            break;
        case Optimizer::DT_STRING:
            oa_genstring(v->astring.str, v->astring.i);
            break;
        case Optimizer::DT_BIT:
            break;
        case Optimizer::DT_BOOL:
            oa_genint(Optimizer::chargen, v->i);
            break;
        case Optimizer::DT_BYTE:
            oa_genint(Optimizer::chargen, v->i);
            break;
        case Optimizer::DT_USHORT:
            oa_genint(Optimizer::shortgen, v->i);
            break;
        case Optimizer::DT_UINT:
            oa_genint(Optimizer::intgen, v->i);
            break;
        case Optimizer::DT_ULONG:
            oa_genint(Optimizer::longgen, v->i);
            break;
        case Optimizer::DT_ULONGLONG:
            oa_genint(Optimizer::longlonggen, v->i);
            break;
        case Optimizer::DT_16:
            oa_genint(Optimizer::u16gen, v->i);
            break;
        case Optimizer::DT_32:
            oa_genint(Optimizer::u32gen, v->i);
            break;
        case Optimizer::DT_ENUM:
            oa_genint(Optimizer::intgen, v->i);
            break;
        case Optimizer::DT_FLOAT:
            oa_genfloat(Optimizer::floatgen, &v->f);
            break;
        case Optimizer::DT_DOUBLE:
            oa_genfloat(Optimizer::doublegen, &v->f);
            break;
        case Optimizer::DT_LDOUBLE:
            oa_genfloat(Optimizer::longdoublegen, &v->f);
            break;
        case Optimizer::DT_CFLOAT:
            oa_genfloat(Optimizer::floatgen, &v->c.r);
            oa_genfloat(Optimizer::floatgen, &v->c.i);
            break;
        case Optimizer::DT_CDOUBLE:
            oa_genfloat(Optimizer::doublegen, &v->c.r);
            oa_genfloat(Optimizer::doublegen, &v->c.i);
            break;
        case Optimizer::DT_CLONGDOUBLE:
            oa_genfloat(Optimizer::longdoublegen, &v->c.r);
            oa_genfloat(Optimizer::longdoublegen, &v->c.i);
            break;
        case Optimizer::DT_ADDRESS:
            oa_genaddress(v->i);
            break;
        case Optimizer::DT_VIRTUAL:
            oa_gen_virtual(v->symbol.sym, v->symbol.i);
            if (v->symbol.sym->isexport)
                oa_put_expfunc(v->symbol.sym);
            break;
        case Optimizer::DT_ENDVIRTUAL:
            oa_gen_endvirtual(v->symbol.sym);
            break;
        case Optimizer::DT_ALIGN:
            oa_align(v->i);
            break;
        case Optimizer::DT_VTT:
            oa_gen_vtt(v->symbol.i, v->symbol.sym);
            break;
        case Optimizer::DT_IMPORTTHUNK:
            oa_gen_importThunk(v->symbol.sym);
            break;
        case Optimizer::DT_VC1:
            oa_gen_vc1(v->symbol.sym);
            break;
        case Optimizer::DT_AUTOREF:
            oa_genint(Optimizer::intgen, v->symbol.sym->offset + v->symbol.i);
            break;
            break;
    }
}
bool ProcessData(const char* name)
{
    if (Optimizer::cparams.prm_asmfile)
    {
        char buf[260];
        outputfile(buf, name, Optimizer::assemblerFileExtension.c_str(), true);
        InsertExternalFile(buf, false);
        Optimizer::outputFile = fopen(buf, "w");
        if (!Optimizer::outputFile)
            return false;
        oa_header(buf, "OCC Version " STRING_VERSION);
        oa_setalign(2, Optimizer::dataAlign, Optimizer::bssAlign, Optimizer::constAlign);
    }
    for (auto v : Optimizer::baseData)
    {
        if (v->type == Optimizer::DT_FUNC)
        {
            lastFunc = v->funcData;
            //            temporarySymbols = v->funcData->temporarySymbols;
            //            functionVariables = v->funcData->variables;
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
            Optimizer::SetUsesESP(currentFunction->usesEsp);
            generate_instructions(Optimizer::intermed_head);
            flush_peep(currentFunction, nullptr);
        }
        else
        {
            ProcessData(v);
        }
    }
    if (Optimizer::cparams.prm_asmfile)
    {
        oa_end_generation();
        for (auto v : Optimizer::externals)
        {
            if (v)
            {
                oa_put_extern(v, 0);
                if (v->isimport)
                {
                    oa_put_impfunc(v, v->importfile);
                }
            }
        }
        oa_trailer();
        fclose(Optimizer::outputFile);
        Optimizer::outputFile = nullptr;
    }
    return true;
}

bool LoadFile(SharedMemory* optimizerMem)
{
    Optimizer::SymbolManager::clear();
    Optimizer::InitIntermediate();
    bool rv = Optimizer::InputIntermediate(optimizerMem);
    Optimizer::SelectBackendData();
    dbginit();
    outcode_file_init();
    Optimizer::oinit();
    omfInit();
    Optimizer::SelectBackendData();
    return rv;
}
bool SaveFile(const char* name)
{
    if (!Optimizer::cparams.prm_asmfile)
    {
        strcpy(infile, name);
        outputfile(Parser::outFile, name, Optimizer::chosenAssembler->objext, true);
        InsertExternalFile(Parser::outFile, false);
        Optimizer::outputFile = fopen(Parser::outFile, "wb");
        if (!Optimizer::outputFile)
            return false;
        if (Optimizer::cparams.prm_browse)
        {
            outputfile(Parser::outFile, name, ".cbr", true);
            // have to readd the extension in case /o was specified
            Utils::StripExt(Parser::outFile);
            Utils::AddExt(Parser::outFile, ".cbr");
            Optimizer::browseFile = fopen(Parser::outFile, "wb");
            if (!Optimizer::browseFile)
                return false;
        }
        oa_end_generation();
        for (auto v : Optimizer::externals)
        {
            if (v)
            {
                oa_put_extern(v, 0);
                if (v->isimport)
                {
                    oa_put_impfunc(v, v->importfile);
                }
            }
        }
        oa_setalign(2, Optimizer::dataAlign, Optimizer::bssAlign, Optimizer::constAlign);
        output_obj_file();
        fclose(Optimizer::outputFile);
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
        Utils::ReplaceAll(curArg, "\"", "\\\"");
        args += std::string("\"") + curArg + "\"";
    }

    return Utils::ToolInvoke("occparse", occ_verbosity, "-! --architecture \"x86;%s\" %s", parserMem->Name().c_str(), args.c_str());
}
int InvokeOptimizer(SharedMemory* parserMem, SharedMemory* optimizerMem)
{
    return Utils::ToolInvoke("occopt", occ_verbosity, "-! -S %s %s", parserMem->Name().c_str(), optimizerMem->Name().c_str());
}
}  // namespace occx86
int main(int argc, char* argv[])
{
    using namespace occx86;
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
    bool syntaxOnly = false;

    if (!Utils::HasLocalExe("occopt") || !Utils::HasLocalExe("occparse"))
    {
        Utils::fatal("cannot find 'occopt.exe' or 'occparse.exe'");
    }
    for (auto p = argv; *p; p++)
    {
        if (strstr(*p, "/y") || strstr(*p, "-y"))
            occ_verbosity = "";
        if (strstr(*p, "/fsyntax-only") || strstr(*p, "-fsyntax-only"))
            syntaxOnly = true;
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
    if (!rv && !syntaxOnly)
    {
        if (!LoadFile(optimizerMem))
        {
            Utils::fatal("internal error: could not load intermediate file");
        }
        if (Optimizer::cparams.prm_displaytiming)
        {
            startTime = clock();
        }
        for (auto v : Optimizer::toolArgs)
        {
            InsertOption(v.c_str());
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
        for (auto p : files)
        {
            if (!LoadFile(optimizerMem))
                Utils::fatal("internal error: could not load intermediate file");
            if (!ProcessData(p.c_str()))
                Utils::fatal("File I/O error");
            if (!SaveFile(p.c_str()))
                Utils::fatal("Cannot open '%s' for write", Parser::outFile);
        }
        if (Optimizer::cparams.prm_displaytiming)
        {
            stopTime = clock();
            printf("occ timing: %d.%03d\n", (stopTime - startTime) / 1000, (stopTime - startTime) % 1000);
        }
        rv = RunExternalFiles();
    }
    delete optimizerMem;
    if (rv == 255)  // means don't run the optimizer or backend
        rv = 0;
    return rv;
}
