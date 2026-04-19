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

#include "compiler.h"
#include <cstdarg>
#include "PreProcessor.h"
#include "Utils.h"
#include "CmdSwitch.h"
#include <csetjmp>
#include <ctime>
#include "../version.h"
#include "winmode.h"
#ifndef ORANGE_NO_INASM
#    include "InstructionParser.h"
#endif
#include "SharedMemory.h"
#include "MakeStubs.h"

#ifndef ORANGE_NO_INASM
#    include "x64Operand.h"
#    include "x64Parser.h"
#endif
#include "ccerr.h"
#include "config.h"
#include "declare.h"
#include "templatedecl.h"
#include "templateinst.h"
#include "templateutil.h"
#include "templatededuce.h"
#include "stmt.h"
#include "inasm.h"
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
#include "expr.h"
#include "constopt.h"
#include "browse.h"
#include "ilstream.h"
#include "OptUtils.h"
#include "istmt.h"
#include "irc.h"
#ifndef ORANGE_NO_MSIL
#    include "DotNetPELib.h"
#endif
#include "constexpr.h"
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "types.h"
#include "exprpacked.h"
#include <cstdlib>
#include <cstdio>
#include "sha1.h"
#include "templatehash.h"

#include "newoperator.h"

#ifndef ORANGE_NO_MSIL
using namespace DotNetPELib;
PELib* peLib;
NetCore* netCoreInstance;
#endif

void regInit() {}
int usingEsp;
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif

bool IsSymbolCharRoutine(const char* data, bool startOnly)
{
    diag("IsSymbolCharRoutine called");
    abort();
}
#ifndef ORANGE_NO_MSIL
void InstructionParser::Split(const std::string& line, std::vector<std::string>& splt)
{
    {
        diag("InstructionParser::Split called");
        abort();
    }
}
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

void diag(const char* fmt, ...)
{
    using namespace Parser;
    if (!templateDefinitionLevel)
    {
        if (Optimizer::cparams.prm_diag)
        {
            va_list argptr;

            va_start(argptr, fmt);
            printf("Diagnostic: ");
            vprintf(fmt, argptr);
            if (theCurrentFunc)
                printf(":%s", theCurrentFunc->sb->decoratedName);
            printf("\n");
            va_end(argptr);
        }
        diagcount++;
    }
}

namespace Parser
{
#ifdef TARGET_OS_WINDOWS
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

#ifndef ORANGE_NO_INASM
InstructionParser* instructionParser;
#endif
static int stoponerr = 0;

Optimizer::COMPILER_PARAMS cparams_default = {
    25, /* int  prm_maxerr;*/
    0,  /* prm_stackalign */
    ~0, /* optimizer modules */
    0,  /* icd flags */
    0,  /* verbosity */
    Dialect::c11,
    Dialect::cpp17,
    true,  /* optimize_for_speed */
    false, /* optimize_for_size */
    false, /* optimize_for_float_access */
    false, /* char prm_quieterrors;*/
    true,  /* char prm_warning;*/
    false, /* char prm_extwarning;*/
    false, /* char prm_diag;*/
    false, /* char prm_ansi;*/
    true,  /* char prm_cmangle;*/
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
    false,       /* char makelib; */
    0,           /* int prm_stackprotect */
    0,           /* .net core version to compile against.   0 = none, NetCore::DummyNeedsLatest = latest*/
    false,       /* export all symbols */
    false        /* compile via assembly */
};  // namespace Parser

int usingEsp;

bool IsCompiler()
{
#ifdef PARSER_ONLY
    return false;
#else
    return true;
#endif
}

static void debug_dumptypedefs(std::list<NAMESPACEVALUEDATA*>* nameSpace)
{
    for (auto sym : *nameSpace->front()->syms)
    {
        if (sym->sb->storage_class == StorageClass::namespace_)
        {
            debug_dumptypedefs(sym->sb->nameSpaceValues);
        }
        else if (sym->sb->storage_class == StorageClass::typedef_)
        {
            Type* tp = sym->tp;
            while (tp->IsPtr() || tp->IsRef())
            {
                tp = tp->BaseType()->btp;
            }
            if (!tp->IsStructured() || !tp->BaseType()->sp->sb->templateLevel || tp->BaseType()->sp->sb->instantiated)
                Optimizer::typedefs.push_back(Optimizer::SymbolManager::Get(sym));
        }
    }
}
void compile(bool global)
{
    fileIndex++;
    SET_GLOBAL(true, 1);
    Optimizer::SymbolManager::clear();
    ListFactoryInit();
    helpinit();
    mangleInit();
    errorinit();
    Optimizer::constoptinit();
    constexprinit();
    declare_init();
    init_init();
    syminit();
    namespaceinit();
    inlineinit();
    lambda_init();
    rtti_init();
    expr_init();
    packed_init();
    libcxx_init();
    statement_ini(global);
    lexini();
    setglbdefs();
    templateInit();
    templateHashInit();

    if (Optimizer::architecture != ARCHITECTURE_MSIL)
    {
        Optimizer::nextLabel = 1;
    }
    iexpr_init();
    iinlineInit();
    genstmtini();
    ParseBuiltins();
    //    intrinsicInit();
#ifndef ORANGE_NO_INASM
    inlineAsmInit();
#endif
    // outcodeInit();
    //    debuggerInit();
    //    browsdataInit();
    browse_init();
    browse_startfile(infile, 0);
    if (IsCompiler())
    {
        static bool first = true;
#ifndef ORANGE_NO_MSIL
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
            if (first || (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile))
            {
                occmsil::msil_compile_start((char*)clist.front().c_str());
            }
#endif
        first = false;
    }
    EnterPackedContext();
    if (Optimizer::cparams.prm_assemble)
    {
        getsym();
        if (currentLex)
        {
            FunctionBlock bd;
            memset(&bd, 0, sizeof(bd));
            bd.type = Keyword::begin_;
            std::list<FunctionBlock*> block{&bd};
            StatementGenerator sg(nullptr);
            while (sg.ParseAsm(block))
                ;
            if (IsCompiler())
            {
                genASM(bd.statements);
            }
        }
    }
    else
    {
        getsym();
        if (currentLex)
        {
            while (declare(nullptr, nullptr, StorageClass::global_, Linkage::none_, emptyBlockdata, true, false, false,
                           AccessLevel::public_))
            {
                if (MATCHKW(Keyword::end_))
                {
                    getsym();
                    if (!currentLex)
                        break;
                }
            }
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
        dumpInlines();
        if (IsCompiler())
        {
            dumpLits();
            WeedExterns();
        }
        if (!Optimizer::cparams.prm_assemble && Optimizer::cparams.prm_debug)
            debug_dumptypedefs(globalNameSpace);
        Optimizer::libIncludes = preProcessor->GetIncludeLibs();
        Optimizer::dseg();  // so we can tack various data on in the optimizer...
    }
    findUnusedStatics(globalNameSpace);
    dumperrs(stdout);
    LeavePackedContext();
    RELEASE_GLOBAL(1);
}
/*-------------------------------------------------------------------------*/

}  // namespace Parser
int main(int argc, char* argv[]) MAINTRY
{
    using namespace Parser;
    Optimizer::cparams = cparams_default;
    Optimizer::compilerName = std::string("occ v") + STRING_VERSION + " " + __DATE__;
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
        return 255;  // some sort of noop operation such as a display occurred

    if (Optimizer::cparams.prm_displaytiming)
    {
        startTime = clock();
    }
    /* loop through and preprocess all the fles on the file list */
    if (clist.size() > 1)
        multipleFiles = true;
    std::string firstFile = clist.size() ? clist.front() : "temp";
    SharedMemory* parserMem = nullptr;
    if (!IsCompiler())
    {
        char buffer[260];
        Utils::StrCpy(buffer, firstFile.c_str());
        Utils::StrCpy(realOutFile, prm_output.GetValue().c_str());
        outputfile(realOutFile, sizeof(realOutFile), buffer, ".ods");
        if (!CompletionCompiler::ccDBOpen(realOutFile))
            Utils::Fatal("Cannot open database file %s", realOutFile);
    }
    else
    {
#ifndef ORANGE_NO_INASM
        Parser::instructionParser = new x64Parser();
#endif
        if (bePostFile.size())
        {
            parserMem = new SharedMemory(0, bePostFile.c_str());
            if (!parserMem->Open() || !parserMem->GetMapping())
                Utils::Fatal("internal error: invalid shared memory region");
            if (clist.empty())
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

    bool first = true;
    while (!clist.empty())
    {
        int index = clist.front().find_last_of('#');
        std::string as = clist.front().substr(index + 1);
        std::string fileToCompile = clist.front().substr(0, index);
        std::string stem = fileToCompile;
        if (IsCompilerSource(stem.c_str()))
        {
            index = stem.find_last_of('.');
            stem = stem.substr(0, index);
        }
        identityValue = Utils::CRC32((const unsigned char*)fileToCompile.c_str(), fileToCompile.size());
        if (IsCompiler())
        {
#ifndef ORANGE_NO_MSIL
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                if (first || (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile))
                    occmsil::msil_main_preprocess((char*)fileToCompile.c_str());
            }
#endif
        }
        first = false;
        Errors::Reset();
        Optimizer::cparams.prm_cplusplus = false;
        if (IsCompiler())
        {
            if (fileToCompile == "-")
                fileToCompile = "a.c";
            if (!MakeStubsContinue.GetValue() && !MakeStubsContinueUser.GetValue())
                Utils::StrCpy(realOutFile, prm_output.GetValue().c_str());
            else
                Utils::StrCpy(realOutFile, "");
            outputfile(realOutFile, sizeof(realOutFile), fileToCompile.c_str(), ".icf");
        }
        else
        {
            CompletionCompiler::ccNewFile((char*)fileToCompile.c_str(), true);
        }
        if (prm_std.GetExists())
        {
            if (prm_std.GetValue() == "c89")
            {
                Optimizer::cparams.c_dialect = Dialect::c89;
            }
            else if (prm_std.GetValue() == "c99")
            {
                Optimizer::cparams.c_dialect = Dialect::c99;
            }
            else if (prm_std.GetValue() == "c11" || prm_std.GetValue() == "c1x")
            {
                Optimizer::cparams.c_dialect = Dialect::c11;
            }
            else if (prm_std.GetValue() == "c23")
            {
                Optimizer::cparams.c_dialect = Dialect::c23;
            }
            else if (prm_std.GetValue() == "c++11")
            {
                Optimizer::cparams.cpp_dialect = Dialect::cpp11;
                Optimizer::cparams.prm_cplusplus = true;
                Optimizer::cparams.c_dialect = Dialect::c89;
            }
            else if (prm_std.GetValue() == "c++14")
            {
                Optimizer::cparams.cpp_dialect = Dialect::cpp14;
                Optimizer::cparams.prm_cplusplus = true;
                Optimizer::cparams.c_dialect = Dialect::c89;
            }
            else if (prm_std.GetValue() == "c++17")
            {
                Optimizer::cparams.cpp_dialect = Dialect::cpp17;
                Optimizer::cparams.prm_cplusplus = true;
                Optimizer::cparams.c_dialect = Dialect::c89;
            }
            else
            {
                Utils::Fatal("value given for 'std' argument unknown: %s", prm_std.GetValue().c_str());
            }
        }
        else if (prm_c23.GetExists())
        {
            Optimizer::cparams.c_dialect = Dialect::c23;
        }
        else if (prm_c11.GetExists())
        {
            Optimizer::cparams.c_dialect = Dialect::c11;
        }
        else if (prm_c99.GetExists())
        {
            Optimizer::cparams.c_dialect = Dialect::c99;
        }
        else if (prm_c89.GetExists())
        {
            Optimizer::cparams.c_dialect = Dialect::c89;
        }
        else
        {
            Optimizer::cparams.c_dialect = Dialect::c11;
        }
        if (Utils::iequal(as, ".c"))
        {
            Optimizer::cparams.prm_cplusplus = false;
        }
        else
        {
            Optimizer::cparams.prm_cplusplus = true;
            Optimizer::cparams.c_dialect = Dialect::c89;
        }
        if (Optimizer::cparams.prm_cplusplus && (Optimizer::architecture == ARCHITECTURE_MSIL))
            Utils::Fatal("MSIL compiler does not compile C++ files at this time");
        preProcessor = new PreProcessor(
            fileToCompile.c_str(), prm_cinclude.GetValue(),
            Optimizer::cparams.prm_cplusplus ? prm_CPPsysinclude.GetValue() : prm_Csysinclude.GetValue(), true,
            Optimizer::cparams.prm_trigraph, '#', Optimizer::cparams.prm_charisunsigned,
            Optimizer::cparams.prm_cplusplus ? Dialect::c23 : Optimizer::cparams.c_dialect, !Optimizer::cparams.prm_ansi,
            (MakeStubsOption.GetValue() || MakeStubsUser.GetValue()) && MakeStubsMissingHeaders.GetValue(),
            prm_pipe.GetValue() != "+" ? prm_pipe.GetValue() : "");

        if (!preProcessor->IsOpen())
            exit(1);
        preProcessor->SetExpressionHandler(ParseExpression);
        preProcessor->SetPragmaCatchall([](const std::string& kw, const std::string& tag) { Optimizer::bePragma[kw] = tag; });

        Utils::StrCpy(infile, fileToCompile.c_str());
        if (!Optimizer::cparams.prm_makestubs || (MakeStubsContinue.GetValue() || MakeStubsContinueUser.GetValue()) &&
                                                     (!prm_error.GetExists() || !prm_error.GetValue().empty()))
        {
            if (Optimizer::cparams.prm_cppfile)
            {
                if (prm_output.GetExists())
                {
                    Utils::StrCpy(cppfile, prm_output.GetValue().c_str());
                }
                else
                {
                    Utils::StrCpy(cppfile, (stem + ".i").c_str());
                }

                cppFile = fopen(cppfile, "w");
                if (!cppFile)
                {
                    delete preProcessor;
                    Utils::Fatal("Cannot open preprocessor output file %s", cppfile);
                }
            }
            if (Optimizer::cparams.prm_errfile)
            {
                std::string fileName = (stem + ".err");
                errFile = fopen(fileName.c_str(), "w");
                if (!errFile)
                {
                    delete preProcessor;
                    fclose(cppFile);
                    Utils::Fatal("Cannot open error file %s", fileName.c_str());
                }
            }
            if (Optimizer::cparams.prm_icdfile)
            {
                std::string fileName = (stem + ".icd");
                Optimizer::icdFile = fopen(fileName.c_str(), "w");
                if (!Optimizer::icdFile)
                {
                    fclose(errFile);
                    delete preProcessor;
                    fclose(cppFile);
                    Utils::Fatal("Cannot open icd file %s", fileName.c_str());
                }
                setvbuf(Optimizer::icdFile, 0, _IOFBF, 32768);
            }

            if (multipleFiles && !Optimizer::cparams.prm_quiet)
                printf("%s\n", fileToCompile.c_str());

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
#ifdef ORANGE_COMPILE_SINGLE_ICF_FILE
                if (compileToFile)
                {
                    std::string fileName = (stem + ".icf");
                    // compile to file
                    int size = Optimizer::GetOutputSize();
                    FILE* fil = fopen(fileName.c_str(), "wb");
                    if (!fil)
                        Utils::Fatal("Cannot open '%s' for write", fileName.c_str());
                    Optimizer::WriteMappingFile(parserMem, fil);
                    fclose(fil);
                }
#endif
            }
        }
        else
        {
            errorinit();
            syminit();
            namespaceinit();
            lexini();
            setglbdefs();
            statement_ini(true);
            packed_init();
            EnterPackedContext();
            while (getsym(), currentLex)
                ;
        }
        if (Optimizer::cparams.prm_makestubs)
        {
            std::string inFile;
            inFile = fileToCompile;
            ;
            int end = inFile.find_last_of('/');
            if (end == std::string::npos)
                end = -1;
            int end1 = inFile.find_last_of('\\');
            if (end1 == std::string::npos)
                end1 = -1;
            if (end < end1)
                end = end1;
            inFile = inFile.substr(end + 1);

            std::string outFile;
            if (!MakeStubsOutputFile.GetValue().empty())
            {
                outFile = MakeStubsOutputFile.GetValue();
            }
            else if (MakeStubsOption.GetValue() || MakeStubsUser.GetValue())
            {
                outFile = "-";
            }
            else if (!prm_output.GetValue().empty())
            {
                outFile = prm_output.GetValue();
            }
            else
            {
                outFile = inFile;
                end = outFile.find_last_of('.');
                if (end != std::string::npos)
                    outFile = outFile.substr(0, end);
                outFile += ".d";
            }
            MakeStubs stubber(*preProcessor, MakeStubsUser.GetValue() || MakeStubsContinueUser.GetValue(),
                              MakeStubsPhonyTargets.GetValue(), inFile, outFile, MakeStubsTargets.GetValue(),
                              MakeStubsQuotedTargets.GetValue());
            stubber.Run(MakeStubsOption.GetValue() || MakeStubsUser.GetValue() ? &std::cout : nullptr);
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
#ifndef ORANGE_NO_MSIL
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
                if (Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_asmfile)
                    occmsil::msil_end_generation(nullptr);
#endif
        }
        clist.pop_front();
    }

    if (IsCompiler())
    {
#ifndef ORANGE_NO_MSIL
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
            if (!Optimizer::cparams.prm_compileonly || Optimizer::cparams.prm_asmfile)
            {
                occmsil::msil_end_generation(nullptr);
                Optimizer::OutputIntermediate(parserMem);
            }
#endif
    }
#ifndef ORANGE_COMPILE_SINGLE_ICF_FILE
    if (compileToFile)
    {
        int index = firstFile.find_last_of('#');
        std::string as = firstFile.substr(index + 1);
        std::string fileToCompile = firstFile.substr(0, index);
        std::string stem = fileToCompile;
        if (IsCompilerSource(stem.c_str()))
        {
            index = stem.find_last_of('.');
            stem = stem.substr(0, index);
        }
        // compile to file
        if (Optimizer::outputFileName.empty())
            Utils::StrCpy(realOutFile, stem.c_str());
        else
        {
            Utils::StrCpy(realOutFile, Optimizer::outputFileName.c_str());
            Utils::StripExt(realOutFile);
        }
        Utils::AddExt(realOutFile, ".icf");
        int size = Optimizer::GetOutputSize();
        FILE* fil = fopen(realOutFile, "wb");
        if (!fil)
            Utils::Fatal("Cannot open '%s' for write", realOutFile);
        Optimizer::WriteMappingFile(parserMem, fil);
        fclose(fil);
    }
#endif
    oFree();
    globalFree();
    delete parserMem;
    if (Optimizer::cparams.prm_displaytiming)
    {
        stopTime = clock();
        printf("occparse timing: %d.%03d\n", (stopTime - startTime) / 1000, (stopTime - startTime) % 1000);
    }
    if (!Optimizer::cparams.prm_cppfile &&
        (!Optimizer::cparams.prm_makestubs || (MakeStubsContinue.GetValue() || MakeStubsContinueUser.GetValue()) &&
                                                  (!prm_error.GetExists() || !prm_error.GetValue().empty())))
        rv = IsCompiler() ? !!stoponerr : 0;
    else
        rv = 255;
    return rv;
}
MAINCATCH