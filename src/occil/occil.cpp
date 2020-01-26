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
#include "DotNetPELib.h"

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
extern PELib* peLib;
extern FILE* outputFile;
extern std::list<std::string> prm_Using;
extern std::list<MsilProperty> msilProperties;

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

void regInit() { }
int usingEsp;

static const int MAX_SHARED_REGION = 240 * 1024 * 1024;

bool InputIntermediate(SharedMemory* mem);

void flush_peep()
{

}
void Import()
{

}
void diag(const char*fmt, ...)
{

}

void ResolveMSILExterns()
{
    for (auto&& sp : externals)
    {
        if (sp->msil)
        {
            if (sp->tp->type == st_func)
            {
                std::vector<SimpleSymbol*> params;
                for (auto v = sp->syms; v != nullptr; v = v->next)
                {
                    SimpleSymbol *sym = (SimpleSymbol*)v->data;
                    if (!sym->thisPtr && sym->tp->type != st_void)
                        params.push_back(sym);
                }
                std::map<SimpleSymbol*, Param*, byName> paramList;
                LoadParams(sp, params, paramList);
                std::vector<Type*> types;
                for (auto v : paramList)
                {
                    types.push_back(v.second->GetType());
                }
                Method* rv = nullptr;
                peLib->Find(sp->msil, &rv, types, nullptr, true);
                if (rv)
                    sp->msil = (const char *)rv;
                else
                    sp->msil = nullptr; // error!!!!!!!
            }
            else // field
            {
                void* rv = nullptr;
                if (peLib->Find(sp->msil, &rv, nullptr) == PELib::s_field)
                {
                    sp->msil = (const char *)rv;
                }
                else
                {
                    sp->msil = nullptr; ////   errror!!!!!!!
                }

            }
        }
    }
    for (auto it = externals.begin(); it != externals.end();)
    {
        if ((*it)->msil)
            it = externals.erase(it);
        else
            ++it;
    }
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
    else if (buf[0] == 0) // no output file specified, put the output wherever the input was...
    {
        strcpy(buf, name);
        char *p = strrchr(buf, '\\');
        char *q = strrchr(buf, '/');
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


void global(SimpleSymbol* sym, int flags)
{
}
void ProcessData(BaseData* v)
{
    switch (v->type)
    {
    case DT_SEG:
        msil_oa_enterseg((e_sg)v->i);
        break;
    case DT_SEGEXIT:
        break;
    case DT_DEFINITION:
        if (v->symbol.sym->tp->type == st_func)
            currentFunction = v->symbol.sym;
        msil_oa_gen_strlab(v->symbol.sym);
        global(v->symbol.sym, v->symbol.i);
        if (v->symbol.sym->tp->type == st_func)
            currentFunction = nullptr;
        break;
    case DT_LABELDEFINITION:
        msil_oa_put_string_label(v->i, 0);
        break;
    case DT_RESERVE:
        //msil_oa_genstorage(v->i);
        break;
    case DT_SYM:
        //msil_oa_genref(v->symbol.sym, v->symbol.i);
        break;
    case DT_SRREF:
        msil_oa_gensrref(v->symbol.sym, v->symbol.i, 0);
        break;
    case DT_PCREF:
        //msil_oa_genpcref(v->symbol.sym, v->symbol.i);
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
        msil_oa_genstring(v->astring.str, v->astring.i);
        break;
    case DT_BIT:
        break;
    case DT_BOOL:
//        msil_oa_genint(chargen, v->i);
        break;
    case DT_BYTE:
//        msil_oa_genint(chargen, v->i);
        break;
    case DT_USHORT:
//        msil_oa_genint(shortgen, v->i);
        break;
    case DT_UINT:
//        msil_oa_genint(intgen, v->i);
        break;
    case DT_ULONG:
//        msil_oa_genint(longgen, v->i);
        break;
    case DT_ULONGLONG:
//        msil_oa_genint(longlonggen, v->i);
        break;
    case DT_16:
//        msil_oa_genint(u16gen, v->i);
        break;
    case DT_32:
 //       msil_oa_genint(u32gen, v->i);
        break;
    case DT_ENUM:
//        msil_oa_genint(intgen, v->i);
        break;
    case DT_FLOAT:
//        msil_oa_genfloat(floatgen, &v->f);
        break;
    case DT_DOUBLE:
//        msil_oa_genfloat(doublegen, &v->f);
        break;
    case DT_LDOUBLE:
//        msil_oa_genfloat(longdoublegen, &v->f);
        break;
    case DT_CFLOAT:
//        msil_oa_genfloat(floatgen, &v->c.r);
//        msil_oa_genfloat(floatgen, &v->c.i);
        break;
    case DT_CDOUBLE:
  //      msil_oa_genfloat(doublegen, &v->c.r);
//        msil_oa_genfloat(doublegen, &v->c.i);
        break;
    case DT_CLONGDOUBLE:
//        msil_oa_genfloat(longdoublegen, &v->c.r);
//        msil_oa_genfloat(longdoublegen, &v->c.i);
        break;
    case DT_ADDRESS:
        genaddress(v->i);
        break;
    case DT_VIRTUAL:
//        msil_oa_gen_virtual(v->symbol.sym, v->symbol.i);
        break;
    case DT_ENDVIRTUAL:
//        msil_oa_gen_endvirtual(v->symbol.sym);
        break;
    case DT_ALIGN:
//        msil_oa_align(v->i);
        break;
    case DT_VTT:
        msil_oa_gen_vtt(v->symbol.i, v->symbol.sym);
        break;
    case DT_IMPORTTHUNK:
        msil_oa_gen_importThunk(v->symbol.sym);
        break;
    case DT_VC1:
        msil_oa_gen_vc1(v->symbol.sym);
        break;
    case DT_AUTOREF:
//        msil_oa_gen_int(0);
        break;
    }
}
bool ProcessData(const char *name)
{
    for (auto v : baseData)
    {
        if (v->type == DT_FUNC)
        {
            temporarySymbols = v->funcData->temporarySymbols;
            functionVariables = v->funcData->variables;
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
    InitIntermediate();
    bool rv = InputIntermediate(parserMem);
    SelectBackendData();
    oinit();
    SelectBackendData();
    if (fileName.empty())
    {
        for (auto&& s : prm_Using)
            _add_global_using(s.c_str());
    }
    if (fileName.empty() && inputFiles.size())
    {
        if (!cparams.prm_compileonly || cparams.prm_asmfile)
        {
            if (!outputFileName.empty())
                outputfile(outFile, outputFileName.c_str(), chosenAssembler->objext);
            else
                outputfile(outFile, inputFiles.front().c_str(), chosenAssembler->objext);
            InsertExternalFile(outFile, false);
        }
        fileName = inputFiles.front();
    }
    msil_main_preprocess((char *)fileName.c_str());
    ResolveMSILExterns();
    return rv;
}
bool SaveFile(const char *name)
{
    if (cparams.prm_compileonly && !cparams.prm_asmfile)
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
                msil_oa_put_extern(v, 0);
            }
        }
        //        msil_oa_setalign(2, dataAlign, bssAlign, constAlign);
        msil_end_generation(outFile);
        fclose(outputFile);
        outputFile = nullptr;
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
        if (!LoadFile(optimizerMem, ""))
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
        if (cparams.prm_compileonly && !cparams.prm_asmfile)
        {
            for (auto p : files)
            {
                if (!LoadFile(optimizerMem, p))
                    Utils::fatal("internal error: could not load intermediate file");
                if (!ProcessData(p.c_str()) || !SaveFile(p.c_str()))
                    Utils::fatal("File I/O error");
            }
        }
        if (!cparams.prm_compileonly)
        {
            rv = RunExternalFiles();
        }
        if (!rv)
            msil_main_postprocess(false);
    }
    return rv;
}
