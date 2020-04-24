/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
#include "ildata.h"
#include "SharedMemory.h"
#include <sstream>
#include <iostream>
#include "../version.h"
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif
extern int architecture;
extern std::list<std::string> toolArgs;
extern std::list<std::string> backendFiles;
extern std::vector<SimpleSymbol*> temporarySymbols;
extern std::vector<SimpleSymbol*> functionVariables;
extern int tempCount;
extern int blockCount;
extern int exitBlock;
extern QUAD* intermed_head, *intermed_tail;
extern std::list<std::string> inputFiles;
extern FILE* icdFile;
extern std::deque<BaseData*> baseData;
extern int nextTemp;
extern int tempBottom;
extern BLOCK **blockArray;
extern ARCH_ASM* chosenAssembler;
extern SimpleExpression* objectArray_exp;
extern std::vector<SimpleSymbol*> externals;
extern int usingEsp;
extern int dataAlign;
extern int bssAlign;
extern int constAlign;
extern std::string outputFileName;
extern SimpleExpression* fltexp;
extern int fastcallAlias;
extern FILE* outputFile;
extern FILE* browseFile;
extern bool assembling;

extern bool IsSymbolCharRoutine(const char *, bool);
bool (*Tokenizer::IsSymbolChar)(const char*, bool) = IsSymbolCharRoutine;

char outFile[260];
char infile[260];
int usingEsp;

InstructionParser* instructionParser;

SimpleSymbol* currentFunction;

static const char *verbosity = nullptr;
static FunctionData* lastFunc;

bool InputIntermediate(SharedMemory* mem);

void regInit() { }

static const int MAX_SHARED_REGION = 240 * 1024 * 1024;

void diag(const char*, ...)
{

}

/*-------------------------------------------------------------------------*/

void outputfile(char* buf, const char* name, const char* ext, bool obj)
{
    strcpy(buf, outputFileName.c_str());
    if (buf[strlen(buf) - 1] == '\\')
    {
        // output file is a path specification rather than a file name
        // just add our name and ext
        strcat(buf, name);
        Utils::StripExt(buf);
        Utils::AddExt(buf, ext);
    }
    else if (buf[0] == 0 || obj && !cparams.prm_compileonly && !assembling)// no output file specified, put the output wherever the input was...
    {
        strcpy(buf, name);
        char *p = strrchr(buf, '\\');
        char *q = strrchr(buf, '/');
        if (q > p)
            p = q;
        if (p)
            strcpy(buf, p+1);
        Utils::StripExt(buf);
        Utils::AddExt(buf, ext);
    }
    else
    {
        // use specified output file name
        if (obj)
            Utils::AddExt(buf, ext);
    }
}


void global(SimpleSymbol* sym, int flags)
{
    omf_globaldef(sym);
    if (cparams.prm_asmfile)
    {
        if (sym->storage_class != scc_localstatic && sym->storage_class != scc_static)
            bePrintf("[global %s]\n", sym->outputName);
    }
    if (flags & BaseData::DF_EXPORT)
    {
        oa_put_expfunc(sym);
    }
}
void ProcessData(BaseData* v)
{
    switch (v->type)
    {
    case DT_SEG:
        oa_enterseg((e_sg)v->i);
        break;
    case DT_SEGEXIT:
        break;
    case DT_DEFINITION:
        global(v->symbol.sym, v->symbol.i);
        oa_gen_strlab(v->symbol.sym);
        break;
    case DT_LABELDEFINITION:
        oa_put_label(v->i);
        break;
    case DT_RESERVE:
        oa_genstorage(v->i);
        break;
    case DT_SYM:
        oa_genref(v->symbol.sym, v->symbol.i);
        break;
    case DT_SRREF:
        oa_gensrref(v->symbol.sym, v->symbol.i, 0);
        break;
    case DT_PCREF:
        oa_genpcref(v->symbol.sym, v->symbol.i);
        break;
    case DT_FUNCREF:
//        global(v->symbol.sym, v->symbol.i);
        break;
    case DT_LABEL:
        oa_gen_labref(v->i);
        break;
    case DT_LABDIFFREF:
        outcode_gen_labdifref(v->diff.l1, v->diff.l2);
        break;
    case DT_STRING:
        oa_genstring(v->astring.str, v->astring.i);
        break;
    case DT_BIT:
        break;
    case DT_BOOL:
        oa_genint(chargen, v->i);
        break;
    case DT_BYTE:
        oa_genint(chargen, v->i);
        break;
    case DT_USHORT:
        oa_genint(shortgen, v->i);
        break;
    case DT_UINT:
        oa_genint(intgen, v->i);
        break;
    case DT_ULONG:
        oa_genint(longgen, v->i);
        break;
    case DT_ULONGLONG:
        oa_genint(longlonggen, v->i);
        break;
    case DT_16:
        oa_genint(u16gen, v->i);
        break;
    case DT_32:
        oa_genint(u32gen, v->i);
        break;
    case DT_ENUM:
        oa_genint(intgen, v->i);
        break;
    case DT_FLOAT:
        oa_genfloat(floatgen, &v->f);
        break;
    case DT_DOUBLE:
        oa_genfloat(doublegen, &v->f);
        break;
    case DT_LDOUBLE:
        oa_genfloat(longdoublegen, &v->f);
        break;
    case DT_CFLOAT:
        oa_genfloat(floatgen, &v->c.r);
        oa_genfloat(floatgen, &v->c.i);
        break;
    case DT_CDOUBLE:
        oa_genfloat(doublegen, &v->c.r);
        oa_genfloat(doublegen, &v->c.i);
        break;
    case DT_CLONGDOUBLE:
        oa_genfloat(longdoublegen, &v->c.r);
        oa_genfloat(longdoublegen, &v->c.i);
        break;
    case DT_ADDRESS:
        oa_genaddress(v->i);
        break;
    case DT_VIRTUAL:
        oa_gen_virtual(v->symbol.sym, v->symbol.i);
        if (v->symbol.sym->isexport)
            oa_put_expfunc(v->symbol.sym);
        break;
    case DT_ENDVIRTUAL:
        oa_gen_endvirtual(v->symbol.sym);
        break;
    case DT_ALIGN:
        oa_align(v->i);
        break;
    case DT_VTT:
        oa_gen_vtt(v->symbol.i, v->symbol.sym);
        break;
    case DT_IMPORTTHUNK:
        oa_gen_importThunk(v->symbol.sym);
        break;
    case DT_VC1:
        oa_gen_vc1(v->symbol.sym);
        break;
    case DT_AUTOREF:
        oa_genint(intgen, v->symbol.sym->offset + v->symbol.i);
        break;
    break;
    }
}
bool ProcessData(const char *name)
{
    if (cparams.prm_asmfile)
    {
        char buf[260];
        outputfile(buf, name, chosenAssembler->asmext, true);
        InsertExternalFile(buf, false);
        outputFile = fopen(buf, "w");
        if (!outputFile)
            return false;
        oa_header(buf, "OCC Version " STRING_VERSION);
        oa_setalign(2, dataAlign, bssAlign, constAlign);

    }
    for (auto v : baseData)
    {
        if (v->type == DT_FUNC)
        {
            lastFunc = v->funcData;
//            temporarySymbols = v->funcData->temporarySymbols;
//            functionVariables = v->funcData->variables;
//            blockCount = v->funcData->blockCount;
//            exitBlock = v->funcData->exitBlock;
//            tempCount = v->funcData->tempCount;
//            functionHasAssembly = v->funcData->hasAssembly;
            intermed_head = v->funcData->instructionList;
            intermed_tail = intermed_head;
            while (intermed_tail && intermed_tail->fwd)
                intermed_tail = intermed_tail->fwd;
            objectArray_exp = v->funcData->objectArray_exp;
            fltexp = v->funcData->fltexp;
            fastcallAlias = v->funcData->fastcallAlias;
            currentFunction = v->funcData->name;
            SetUsesESP(currentFunction->usesEsp);
            generate_instructions(intermed_head);
            flush_peep(currentFunction, nullptr);
        }
        else
        {
            ProcessData(v);
        }
    }
    if (cparams.prm_asmfile)
    {
        oa_end_generation();
        for (auto v : externals)
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
        fclose(outputFile);
        outputFile = nullptr;
    }
    return true;
}

bool LoadFile(SharedMemory* optimizerMem)
{
    InitIntermediate();
    bool rv = InputIntermediate(optimizerMem);
    SelectBackendData();
    dbginit();
    outcode_file_init();
    oinit();
    omfInit();
    SelectBackendData();
    return rv;
}
bool SaveFile(const char *name)
{
    if (!cparams.prm_asmfile)
    {
        strcpy(infile, name);
        outputfile(outFile, name, chosenAssembler->objext, true);
        InsertExternalFile(outFile, false);
        outputFile = fopen(outFile, "wb");
        if (!outputFile)
            return false;
        if (cparams.prm_browse)
        {
            outputfile(outFile, name, ".cbr", true);
            // have to readd the extension in case /o was specified
            Utils::StripExt(outFile);
            Utils::AddExt(outFile, ".cbr");
            browseFile = fopen(outFile, "wb");
            if (!browseFile)
                return false;
        }
        oa_end_generation();
        for (auto v : externals)
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
        oa_setalign(2, dataAlign, bssAlign, constAlign);
        output_obj_file();
        fclose(outputFile);
    }
    return true;
}
int InvokeParser(int argc, char**argv, SharedMemory* parserMem)
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

    return Utils::ToolInvoke("occparse", verbosity, "-! --architecture \"x86;%s\" %s", parserMem->Name().c_str(), args.c_str());
}
int InvokeOptimizer(SharedMemory* parserMem, SharedMemory* optimizerMem)
{
    return Utils::ToolInvoke("occopt", verbosity, "-! %s %s", parserMem->Name().c_str(), optimizerMem->Name().c_str());
}

int main(int argc, char* argv[])
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");

    if (!Utils::HasLocalExe("occopt") || !Utils::HasLocalExe("occparse"))
    {
        Utils::fatal("cannot find 'occopt.exe' or 'occparse.exe'");
    }
    for (auto p = argv; *p; p++)
    {
        if (strstr(*p, "/y") || strstr(*p, "-y"))
            verbosity = "";
    }
    auto optimizerMem = new SharedMemory(MAX_SHARED_REGION);
    optimizerMem->Create();
    int rv = 0;
    if (argc == 2 && Utils::HasExt(argv[1], ".icf"))
    {
        FILE* fil = fopen(argv[1], "rb");
        if (fil)
        {
            fseek(fil, 0, SEEK_END);
            long size = ftell(fil);
            fseek(fil, 0, SEEK_SET);
            optimizerMem->GetMapping();
            optimizerMem->EnsureCommitted(size);
            fread(optimizerMem->GetMapping(), 1, size, fil);
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
        rv = InvokeParser(argc, argv, parserMem) || InvokeOptimizer(parserMem, optimizerMem);
        delete parserMem;
    }
    if (!rv)
    {
        if (!LoadFile(optimizerMem))
        {
            Utils::fatal("internal error: could not load intermediate file");
        }
        for (auto v : toolArgs)
        {
            InsertOption(v.c_str());
        }
        for (auto f : backendFiles)
        {
            InsertExternalFile(f.c_str(), false);

        }
        std::list<std::string> files = inputFiles;
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
            if (!ProcessData(p.c_str()) || !SaveFile(p.c_str()))
                Utils::fatal("File I/O error");
        }
        rv = RunExternalFiles();
    }
    delete optimizerMem;
    return rv ;
}
