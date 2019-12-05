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

 /*
  * C configuration when we are using ICODE code generator output
  */
#include <stdio.h>
#include <string.h>
#include "be.h"
#include "beinterf.h"
#include "Utils.h"
#include "winmode.h"
#include "CmdSwitch.h"
#include "ildata.h"
#include "SharedMemory.h"

extern int architecture;
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
extern std::string outputFileName;
extern std::vector<SimpleSymbol*> externals;
extern std::list<std::string> backendFiles;
extern SimpleExpression* fltexp;
extern int usingEsp;
extern int fastcallAlias;

CmdSwitchParser SwitchParser;
CmdSwitchBool single(SwitchParser, 's', false, "single");

const char* usageText =
"[options] inputfile\n"
"\n"
"--single     don't open internal file list\n"
"\nTime: " __TIME__ "  Date: " __DATE__;

SimpleSymbol* currentFunction;

char infile[260];
char outFile[260];

int dbgblocknum;

static const char* verbosity = nullptr;
extern FILE* outputFile;

void regInit() { }
int usingEsp;

static const int MAX_SHARED_REGION = 500 * 1024 * 1024;

bool InputIntermediate(SharedMemory* mem);

void flush_peep()
{

}
void Import()
{

}
void LoadFuncs()
{

}
void diag(const char*fmt, ...)
{

}

void outputfile(char* buf, const char* name, const char* ext)
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
    else if (buf[0] != 0)
    {
        // output file is a real name, strip the name portion off the path and add our name and ext
        char* p = strrchr(buf, '\\');
        char* p1 = strrchr(buf, '/');
        if (p1 > p)
            p = p1;
        else if (!p)
            p = p1;
        if (!p)
            p = buf;
        else
            p++;
        const char* r = strrchr(name, '\\');
        const char* r1 = strrchr(name, '/');
        if (r1 > r)
            r = r1;
        else if (!r)
            r = r1;
        if (!r)
            r = name;
        else
            r++;
        strcpy(p, r);
        Utils::StripExt(buf);
        Utils::AddExt(buf, ext);
    }
    else // no output file specified, put the output wherever the input was...
    {
        strcpy(buf, name);
        Utils::StripExt(buf);
        Utils::AddExt(buf, ext);
    }
}


void global(SimpleSymbol* sym, int flags)
{
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
        oa_gen_strlab(v->symbol.sym);
        global(v->symbol.sym, v->symbol.i);
        break;
    case DT_LABELDEFINITION:
        oa_put_string_label(v->i, 0);
        break;
    case DT_RESERVE:
        //oa_genstorage(v->i);
        break;
    case DT_SYM:
        //oa_genref(v->symbol.sym, v->symbol.i);
        break;
    case DT_SRREF:
        oa_gensrref(v->symbol.sym, v->symbol.i, 0);
        break;
    case DT_PCREF:
        //oa_genpcref(v->symbol.sym, v->symbol.i);
        break;
    case DT_FUNCREF:
        gen_funcref(v->symbol.sym);
        global(v->symbol.sym, v->symbol.i);
        break;
    case DT_LABEL:
        gen_labref(v->i);
        break;
    case DT_LABDIFFREF:
        gen_labdifref(v->diff.l1, v->diff.l2);
        break;
    case DT_STRING:
        oa_genstring(v->astring.str, v->astring.i);
        break;
    case DT_BIT:
        break;
    case DT_BOOL:
//        oa_genint(chargen, v->i);
        break;
    case DT_BYTE:
//        oa_genint(chargen, v->i);
        break;
    case DT_USHORT:
//        oa_genint(shortgen, v->i);
        break;
    case DT_UINT:
//        oa_genint(intgen, v->i);
        break;
    case DT_ULONG:
//        oa_genint(longgen, v->i);
        break;
    case DT_ULONGLONG:
//        oa_genint(longlonggen, v->i);
        break;
    case DT_16:
//        oa_genint(u16gen, v->i);
        break;
    case DT_32:
 //       oa_genint(u32gen, v->i);
        break;
    case DT_ENUM:
//        oa_genint(intgen, v->i);
        break;
    case DT_FLOAT:
//        oa_genfloat(floatgen, &v->f);
        break;
    case DT_DOUBLE:
//        oa_genfloat(doublegen, &v->f);
        break;
    case DT_LDOUBLE:
//        oa_genfloat(longdoublegen, &v->f);
        break;
    case DT_CFLOAT:
//        oa_genfloat(floatgen, &v->c.r);
//        oa_genfloat(floatgen, &v->c.i);
        break;
    case DT_CDOUBLE:
  //      oa_genfloat(doublegen, &v->c.r);
//        oa_genfloat(doublegen, &v->c.i);
        break;
    case DT_CLONGDOUBLE:
//        oa_genfloat(longdoublegen, &v->c.r);
//        oa_genfloat(longdoublegen, &v->c.i);
        break;
    case DT_ADDRESS:
        genaddress(v->i);
        break;
    case DT_VIRTUAL:
//        oa_gen_virtual(v->symbol.sym, v->symbol.i);
        break;
    case DT_ENDVIRTUAL:
//        oa_gen_endvirtual(v->symbol.sym);
        break;
    case DT_ALIGN:
//        oa_align(v->i);
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
//        oa_gen_int(0);
        break;
    }
}
bool ProcessData(const char *name)
{
    if (cparams.prm_asmfile)
    {
        char buf[260];
        outputfile(buf, name, chosenAssembler->asmext);
        InsertExternalFile(buf, false);
        outputFile = fopen(buf, "w");
        if (!outputFile)
            return false;
//        oa_setalign(2, dataAlign, bssAlign, constAlign);

    }
    for (auto v : baseData)
    {
        if (v->type == DT_FUNC)
        {
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
        for (auto v : externals)
        {
            if (v)
            {
                oa_put_extern(v, 0);
            }
        }
        oa_end_generation();
        fclose(outputFile);
        outputFile = nullptr;
    }
    return true;
}

bool LoadFile(SharedMemory* parserMem)
{
    InitIntermediate();
    bool rv = InputIntermediate(parserMem);
    SelectBackendData();
    oinit();
    SelectBackendData();
    return rv;
}
bool SaveFile(const char *name)
{
    if (!cparams.prm_asmfile)
    {
        strcpy(infile, name);
        outputfile(outFile, name, chosenAssembler->objext);
        InsertExternalFile(outFile, false);
        outputFile = fopen(outFile, "wb");
        if (!outputFile)
            return false;
        for (auto v : externals)
        {
            if (v)
            {
                oa_put_extern(v, 0);
            }
        }
        //        oa_setalign(2, dataAlign, bssAlign, constAlign);
        fclose(outputFile);
        oa_end_generation();
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

    return Utils::ToolInvoke("occparse", verbosity, "-! --architecture \"msil;%s\" %s", parserMem->Name().c_str(), args.c_str());
}
int InvokeOptimizer(SharedMemory* parserMem, SharedMemory* optimizerMem)
{
    return Utils::ToolInvoke("occopt", verbosity, "-! %s %s", parserMem->Name().c_str(), optimizerMem->Name().c_str());
}

int main(int argc, char* argv[])
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");

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
        char fileName[260];
        if (!LoadFile(optimizerMem))
        {
            Utils::fatal("internal error: could not load intermediate file");
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
        if (!cparams.prm_compileonly)
        {
            rv = RunExternalFiles();
        }
        if (!rv)
            oa_main_postprocess(false);
    }
    return rv;
}
