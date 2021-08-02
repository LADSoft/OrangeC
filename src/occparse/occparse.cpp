/* Software License Agreement
 * 
 *     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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

#include "compiler.h"
#include "PreProcessor.h"
#include "Utils.h"
#include "CmdSwitch.h"
#include <setjmp.h>
#include <time.h>
#include "../version.h"
#include "winmode.h"
#include "InstructionParser.h"
#include "SharedMemory.h"

#    include "x64Operand.h"
#    include "x64Parser.h"
#include "ccerr.h"
#include "config.h"
#include "declare.h"
#include "template.h"
#include "stmt.h"
#include "inasm.h"
#include "symtab.h"
#include "osutil.h"
#include "ildata.h"
#include "rtti.h"
#include "cppbltin.h"
#include "iout.h"
#include "memory.h"
#include "mangle.h"
#include "libcxx.h"
#include "lex.h"
#include "lambda.h"
#include "inline.h"
#include "init.h"
#include "iinline.h"
#include "iexpr.h"
#include "help.h"
#include "inasm.h"
#include "expr.h"
#include "constopt.h"
#include "browse.h"
#include "ilstream.h"
#include "OptUtils.h"
#include "istmt.h"
#include "irc.h"
#include "DotNetPELib.h"

using namespace DotNetPELib;
PELib* peLib;

void regInit() {}
int usingEsp;
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif
namespace occmsil
{
int msil_main_preprocess(char* fileName);
void msil_end_generation(char* fileName);
void msil_compile_start(char* name);
}  // namespace occmsil
namespace DotNetPELib
{
class PELib;
}
namespace Optimizer
{
    unsigned termCount;
};
namespace CompletionCompiler
{
void ccDumpSymbols(void);
std::string ccNewFile(char* fileName, bool main);
void ccCloseFile(FILE* handle);
int ccDBOpen(const char* name);
}  // namespace CompletionCompiler
namespace Parser
{
#ifdef _WIN32
extern "C"
{
    char* __stdcall GetModuleFileNameA(int handle, char* buf, int size);
}
#endif
char outFile[260];
unsigned identityValue;
int maxBlocks, maxTemps;
char cppfile[256];
FILE* cppFile;
char infile[256];
int fileIndex;

FILE* errFile;
PreProcessor* preProcessor;

char realOutFile[260];

InstructionParser* instructionParser;
static int stoponerr = 0;

Optimizer::COMPILER_PARAMS cparams_default = {
    25,    /* int  prm_maxerr;*/
    0,     /* prm_stackalign */
    ~0,    /* optimizer modules */
    0,     /* icd flags */
    0,     /* verbosity */
    true,  /* optimize_for_speed */
    false, /* optimize_for_size */
    false, /* optimize_for_float_access */
    false, /* char prm_quieterrors;*/
    true,  /* char prm_warning;*/
    false, /* char prm_extwarning;*/
    false, /* char prm_diag;*/
    false, /* char prm_ansi;*/
    true,  /* char prm_cmangle;*/
    true,  /* char prm_c99;*/
    true,  /* char prm_c1x;*/
    false, /* char prm_cplusplus;*/
    true,  /* char prm_xcept;*/
    false, /* char prm_icdfile;*/
    false, /* char prm_asmfile;*/
    false, /* char prm_compileonly;*/
    false, /* char prm_debug;*/
    false, /* char prm_listfile;*/
    false, /* char prm_cppfile;*/
    false, /* char prm_errfile;*/
    false, /* char prm_browse;*/
    false, /* char prm_trigraph;*/
    false, /* char prm_oldfor;*/
    false, /* char prm_stackcheck;*/
    true,  /* char prm_allowinline;*/
    false, /* char prm_profiler;*/
    true,  /* char prm_mergstrings;*/
    false, /* char prm_revbits;*/
    true,  /* char prm_lines;*/
    true,  /* char prm_bss;*/
    false, /* char prm_intrinsic;*/
    false, /* char prm_smartframes;*/
    false, /* char prm_farkeyword;*/
    false, /* char prm_linkreg;*/
    false, /* char prm_charisunsigned;*/
    false, /* bool prm_assemble;*/
    false, /* bool prm_makestubs;*/
#ifndef WIN32
    DOS32A, /* char prm_targettype;*/
#else
    CONSOLE, /* char prm_targettype;*/
#endif
    false,       /* char compile_under_dos;*/
    true,        /* char prm_bepeep;*/
    false,       /* char prm_crtdll;*/
    false,       /* char prm_lscrtdll;*/
    false,       /* char prm_msvcrt;*/
    (asmTypes)0, /* char prm_assembler;*/
    false,       /* char prm_flat;*/
    false,       /* char prm_nodos;*/
    true,        /* char prm_useesp;*/
    false,       /* char managed_library;*/
    false,       /* char no_default_libs;*/
    false,       /*char replacePInvoke;*/
    true,        /* char msilAllowExtensions;*/
};

/*
void doPragma(const char *key, const char *tag)
{
    bePragma[key] = tag;
    if (Utils::iequal(key, "netlib"))
    {
        while (isspace(*tag))
            tag++;
        if (*tag)
        {
            std::string temp = tag;
            int npos = temp.find_last_not_of(" \t\v\n");
            if (npos != std::string::npos)
            {
                temp = temp.substr(0, npos + 1);
            }
            peLib->LoadAssembly(temp);
            Import();
        }
    }
}
    */

int usingEsp;

static void debug_dumptypedefs(NAMESPACEVALUELIST* nameSpace)
{
    int i;
    HASHTABLE* syms = nameSpace->valueData->syms;
    for (i = 0; i < syms->size; i++)
    {
        SYMLIST* h = syms->table[i];
        if (h != 0)
        {
            while (h)
            {

                SYMBOL* sym = (SYMBOL*)h->p;
                if (sym->sb->storage_class == sc_namespace)
                {
                    debug_dumptypedefs(sym->sb->nameSpaceValues);
                }
                else if (sym->sb->storage_class == sc_typedef)
                {
                    TYPE* tp = sym->tp;
                    while (ispointer(tp) || isref(tp))
                    {
                        tp = basetype(tp)->btp;
                    }
                    if (!isstructured(tp) || !basetype(tp)->sp->sb->templateLevel || basetype(tp)->sp->sb->instantiated)
                        Optimizer::typedefs.push_back(Optimizer::SymbolManager::Get(sym));
                }
                h = h->next;
            }
        }
    }
}
void MakeStubs(void)
{
    // parse the file, only gets the macro expansions
    errorinit();
    syminit();
    lexini();
    setglbdefs();
    while (getsym() != nullptr)
        ;
    printf("%s:\\\n", infile);
    for (auto&& v : preProcessor->GetUserIncludes())
    {
        printf("    %s \\\n", v.c_str());
    }
    printf("\n");
}
void compile(bool global)
{
    fileIndex++;
    SET_GLOBAL(true, 1);
    LEXLIST* lex = nullptr;
    Optimizer::SymbolManager::clear();
    helpinit();
    mangleInit();
    errorinit();
    Optimizer::constoptinit();
    declare_init();
    init_init();
    inlineinit();
    lambda_init();
    rtti_init();
    expr_init();
    libcxx_init();
    statement_ini(global);
    syminit();
    lexini();
    setglbdefs();
    templateInit();
    if (Optimizer::architecture != ARCHITECTURE_MSIL)
    {
        Optimizer::nextLabel = 1;
    }
    iexpr_init();
    iinlineInit();
    genstmtini();
    ParseBuiltins();
    //    intrinsicInit();
    inlineAsmInit();
    // outcodeInit();
    //    debuggerInit();
    //    browsdataInit();
    browse_init();
    browse_startfile(infile, 0);
    if (IsCompiler())
    {
        static bool first = true;
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
            if (first || (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile))
            {
                occmsil::msil_compile_start((char*)clist->data);
            }
        first = false;
    }
    if (Optimizer::cparams.prm_assemble)
    {
        lex = getsym();
        if (lex)
        {
            BLOCKDATA block;
            memset(&block, 0, sizeof(block));
            block.type = begin;
            while ((lex = statement_asm(lex, nullptr, &block)) != nullptr)
                ;
            if (IsCompiler())
            {
                genASM(block.head);
            }
        }
    }
    else
    {
        lex = getsym();
        if (lex)
        {
            while ((lex = declare(lex, nullptr, nullptr, sc_global, lk_none, nullptr, true, false, false, ac_public)) != nullptr)
                ;
        }
    }
    if (!IsCompiler())
    {
        CompletionCompiler::ccDumpSymbols();
    }
    if (!TotalErrors())
    {
        dumpInlines();
        dumpInitializers();
        dumpInlines();
        dumpImportThunks();
        dumpStartups();
        if (IsCompiler())
        {
            dumpLits();
            WeedExterns();
        }
        if (!Optimizer::cparams.prm_assemble && Optimizer::cparams.prm_debug)
            debug_dumptypedefs(globalNameSpace);
        Optimizer::libIncludes = preProcessor->GetIncludeLibs();
    }
    findUnusedStatics(globalNameSpace);
    dumperrs(stdout);
    RELEASE_GLOBAL(1);
}
/*-------------------------------------------------------------------------*/

void enter_filename(const char* name) { Optimizer::inputFiles.push_back(name); }
}  // namespace Parser
int main(int argc, char* argv[])
{
    using namespace Parser;
    Optimizer::cparams = cparams_default;
    Optimizer::compilerName = std::string("occ v") + STRING_VERSION + " " + __DATE__;
    char buffer[256];
    char* p;
    bool multipleFiles = false;
    int rv;
    bool compileToFile = false;
    unsigned startTime, stopTime;
    /*   signal(SIGSEGV,internalError) ;*/
    /*   signal(SIGFPE, internalError) ;*/

    /*
    if (Optimizer::chosenAssembler->Args)
    {
        CMDLIST* newArgs = (CMDLIST*)calloc(sizeof(Args) + sizeof(Args[0]) * Optimizer::chosenAssembler->ArgCount, 1);
        if (newArgs)
        {
            memcpy(&newArgs[0], Optimizer::chosenAssembler->Args, Optimizer::chosenAssembler->ArgCount * sizeof(Args[0]));
            memcpy(&newArgs[Optimizer::chosenAssembler->ArgCount], &Args[0], sizeof(Args));
            ArgList = newArgs;
        }
    }
    */
    /* parse environment variables, command lines, and config files  */
    if (ccinit(argc, argv))
        return 255; // some sort of noop operation such as a display occurred

    if (Optimizer::cparams.prm_displaytiming)
    {
        startTime = clock();
    }
    /* loop through and preprocess all the files on the file list */
    if (clist && clist->next)
        multipleFiles = true;
    const char* firstFile = clist ? (const char*)clist->data : "temp";
    SharedMemory* parserMem = nullptr;
    if (!IsCompiler())
    {
        strcpy(buffer, (char*)clist->data);
        strcpy(realOutFile, prm_output.GetValue().c_str());
        outputfile(realOutFile, buffer, ".ods");
        if (!CompletionCompiler::ccDBOpen(realOutFile))
            Utils::fatal("Cannot open database file %s", realOutFile);
    }
    else
    {
        Parser::instructionParser = new x64Parser();
        if (bePostFile.size())
        {
            parserMem = new SharedMemory(0, bePostFile.c_str());
            if (!parserMem->Open() || !parserMem->GetMapping())
                Utils::fatal("internal error: invalid shared memory region");
            if (!clist)
            {
                Optimizer::OutputIntermediate(parserMem);
            }
        }
        else  // so we can do compiles without the output going anywhere...
        {
            parserMem = new SharedMemory(240 * 1024 * 1024);
            parserMem->Create();
            compileToFile = true;
        }
    }
    for (auto c = clist; c; c = c->next)
    {
        enter_filename((const char*)c->data);
    }
    //    mainPreprocess();
    bool first = true;
    while (clist)
    {
        identityValue = Utils::CRC32((const unsigned char*)clist->data, strlen((char*)clist->data));
        if (IsCompiler())
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                if (first || (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile))
                    occmsil::msil_main_preprocess((char*)clist->data);
            }
        }
        first = false;
        Errors::Reset();
        Optimizer::cparams.prm_cplusplus = false;
        strcpy(buffer, (char*)clist->data);
        if (IsCompiler())
        {
            if (buffer[0] == '-')
                strcpy(buffer, "a.c");
            strcpy(realOutFile, prm_output.GetValue().c_str());
            outputfile(realOutFile, buffer, ".icf");
        }
        else
        {
            CompletionCompiler::ccNewFile(buffer, true);
        }
        Utils::AddExt(buffer, ".C");
        if (prm_std.GetExists())
        {
            if (prm_std.GetValue() == "c89")
            {
                Optimizer::cparams.prm_c99 = Optimizer::cparams.prm_c1x = false;
            }
            else if (prm_std.GetValue() == "c99")
            {
                Optimizer::cparams.prm_c99 = true;
                Optimizer::cparams.prm_c1x = false;
            }
            else if (prm_std.GetValue() == "c11")
            {
                Optimizer::cparams.prm_c99 = true;
                Optimizer::cparams.prm_c1x = true;
            }
            else if (prm_std.GetValue() == "c++11")
            {
                cplusplusversion = 11;
                Optimizer::cparams.prm_c99 = false;
                Optimizer::cparams.prm_c1x = false;
                Optimizer::cparams.prm_cplusplus = true;
            }
            else if (prm_std.GetValue() == "c++14")
            {
                cplusplusversion = 14;
                Optimizer::cparams.prm_c99 = false;
                Optimizer::cparams.prm_c1x = false;
                Optimizer::cparams.prm_cplusplus = true;
            }
            else if (prm_std.GetValue() == "c++17")
            {
                cplusplusversion = 17;
                Optimizer::cparams.prm_c99 = false;
                Optimizer::cparams.prm_c1x = false;
                Optimizer::cparams.prm_cplusplus = true;
            }
            else 
            {
                Utils::fatal("value given for 'std' argument unknown: %s", prm_std.GetValue().c_str());
            }
        }
        else if (prm_language.GetExists())
        {
            if (prm_language.GetValue() == "c++")
            {
                Optimizer::cparams.prm_cplusplus = true;
                Optimizer::cparams.prm_c99 = Optimizer::cparams.prm_c1x = false;
            }
            else if (prm_language.GetValue() != "c")
            {
                Utils::fatal("Unknown language specifier: %s\n", prm_language.GetValue().c_str());
            }
        }
        else
        {
            static const std::list<std::string> cppExtensions = {".h", ".hh", ".hpp", ".hxx", ".hm", ".cpp", ".cxx", ".cc", ".c++"};
            for (auto& str : cppExtensions)
            {
                if (Utils::HasExt(buffer, str.c_str()))
                {
                    Optimizer::cparams.prm_cplusplus = true;
                    Optimizer::cparams.prm_c99 = Optimizer::cparams.prm_c1x = false;
                    break;
                }
            }
        }
        if (Optimizer::cparams.prm_cplusplus && (Optimizer::architecture == ARCHITECTURE_MSIL))
            Utils::fatal("MSIL compiler does not compile C++ files at this time");
        preProcessor =
            new PreProcessor(buffer, prm_cinclude.GetValue(), Optimizer::cparams.prm_cplusplus ? prm_CPPsysinclude.GetValue() : prm_Csysinclude.GetValue(), 
                             true, Optimizer::cparams.prm_trigraph, '#',
                             Optimizer::cparams.prm_charisunsigned,
                             !Optimizer::cparams.prm_c99 && !Optimizer::cparams.prm_c1x && !Optimizer::cparams.prm_cplusplus,
                             !Optimizer::cparams.prm_ansi, prm_pipe.GetValue() != "+" ? prm_pipe.GetValue() : "");

        if (!preProcessor->IsOpen())
            exit(1);
        preProcessor->SetExpressionHandler(ParseExpression);
        preProcessor->SetPragmaCatchall([](const std::string& kw, const std::string& tag) { Optimizer::bePragma[kw] = tag; });

        strcpy(infile, buffer);
        if (Optimizer::cparams.prm_makestubs)
        {
            enter_filename((char*)clist->data);
            MakeStubs();
            rv = 255;
        }
        else
        {
            if (Optimizer::cparams.prm_cppfile)
            {
                Utils::StripExt(buffer);
                Utils::AddExt(buffer, ".i");
                strcpy(cppfile, buffer);
                cppFile = fopen(buffer, "w");
                if (!cppFile)
                {
                    delete preProcessor;
                    Utils::fatal("Cannot open preprocessor output file %s", buffer);
                }
            }
            if (Optimizer::cparams.prm_errfile)
            {
                Utils::StripExt(buffer);
                Utils::AddExt(buffer, ".err");
                errFile = fopen(buffer, "w");
                if (!errFile)
                {
                    delete preProcessor;
                    fclose(cppFile);
                    Utils::fatal("Cannot open error file %s", buffer);
                }
            }
            if (Optimizer::cparams.prm_icdfile)
            {
                Utils::StripExt(buffer);
                Utils::AddExt(buffer, ".icd");
                Optimizer::icdFile = fopen(buffer, "w");
                if (!Optimizer::icdFile)
                {
                    fclose(errFile);
                    delete preProcessor;
                    fclose(cppFile);
                    Utils::fatal("Cannot open icd file %s", buffer);
                }
                setvbuf(Optimizer::icdFile, 0, _IOFBF, 32768);
            }

            if (multipleFiles && !Optimizer::cparams.prm_quiet)
                printf("%s\n", (char*)clist->data);

            compile(false);
            if (IsCompiler())
            {
                if (Optimizer::architecture != ARCHITECTURE_MSIL ||
                    (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile))
                {
                    Optimizer::OutputIntermediate(parserMem);
                    oFree();
                }
                if (Optimizer::cparams.prm_icdfile)
                    Optimizer::OutputIcdFile();
                Optimizer::InitIntermediate();
            }
        }
        if (!IsCompiler())
        {
            localFree();
        }
        if (Optimizer::architecture != ARCHITECTURE_MSIL || (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile))
            globalFree();
        if (Optimizer::cparams.prm_diag)
        {
            mem_summary();
            printf("Intermediate stats:\n");
            printf("  Block peak:          %d\n", maxBlocks);
            printf("  Temp peak:           %d\n", maxTemps);
        }
        maxBlocks = maxTemps = 0;

        delete preProcessor;
        if (cppFile)
            fclose(cppFile);
        if (errFile)
            fclose(errFile);
        if (Optimizer::icdFile)
            fclose(Optimizer::icdFile);

        /* Flag to stop if there are any errors */
        stoponerr |= TotalErrors();
        if (IsCompiler())
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            if (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile)
                occmsil::msil_end_generation(nullptr);
        }
        clist = clist->next;
    }

    if (IsCompiler())
    {
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
            if (!Optimizer::cparams.prm_compileonly || Optimizer::cparams.prm_asmfile)
            {
                occmsil::msil_end_generation(nullptr);
                Optimizer::OutputIntermediate(parserMem);
            }
    }
    oFree();
    globalFree();
    if (compileToFile)
    {
        // compile to file
        if (Optimizer::outputFileName.empty())
            strcpy(realOutFile, firstFile);
        else
            strcpy(realOutFile, Optimizer::outputFileName.c_str());
        Utils::StripExt(realOutFile);
        Utils::AddExt(realOutFile, ".icf");
        int size = Optimizer::GetOutputSize();
        FILE* fil = fopen(realOutFile, "wb");
        if (!fil)
            Utils::fatal("Cannot open '%s' for write", realOutFile);
        Optimizer::WriteMappingFile(parserMem, fil);
        fclose(fil);
    }
    delete parserMem;
    if (Optimizer::cparams.prm_displaytiming)
    {
        stopTime = clock();
        printf("occparse timing: %d.%03d\n", (stopTime - startTime)/1000, (stopTime - startTime)% 1000); 
    }
    rv = IsCompiler() ? !!stoponerr : 0;
    return rv;
}
