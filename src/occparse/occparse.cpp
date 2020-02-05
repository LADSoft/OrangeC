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

#include "compiler.h"
#include "PreProcessor.h"
#include "Utils.h"
#include "CmdSwitch.h"
#include <setjmp.h>
#include "../version.h"
#include "winmode.h"
#include "InstructionParser.h"
#include "SharedMemory.h"

#ifndef PARSER_ONLY
#include "x64Operand.h"
#include "x64Parser.h"
#endif
extern COMPILER_PARAMS cparams;
extern ARCH_DEBUG* chosenDebugger;
extern ARCH_ASM* chosenAssembler;
extern NAMESPACEVALUELIST* globalNameSpace;
extern LIST* clist;
extern int optflags;
extern CmdSwitchCombineString prm_include;
extern CmdSwitchCombineString prm_sysinclude;
extern CmdSwitchCombineString prm_libpath;
extern CmdSwitchString prm_pipe;
extern CmdSwitchCombineString prm_output;
extern std::list<std::string> inputFiles;
extern std::list<std::string> libIncludes;
extern FILE* icdFile;
extern std::string compilerName;
extern std::vector<SimpleSymbol*> typedefs;
extern std::map<std::string, std::string> bePragma;
extern std::string outputFileName;
extern std::string bePostFile;

char outFile[260];

long long ParseExpression(std::string&line);

#ifdef _WIN32
extern "C"
{
    char* __stdcall GetModuleFileNameA(int handle, char* buf, int size);
}
#endif

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif

#ifdef PARSER_ONLY
void ccDumpSymbols(void);
std::string ccNewFile(char* fileName, bool main);
void ccCloseFile(FILE* handle);
int ccDBOpen(const char* name);
namespace DotNetPELib
{
    class PELib;
}
DotNetPELib::PELib* peLib;
#endif


int maxBlocks, maxTemps;
char cppfile[256];
FILE *cppFile;
char infile[256];

FILE *errFile;
PreProcessor *preProcessor;

char realOutFile[260];

static int stoponerr = 0;

InstructionParser* instructionParser;

COMPILER_PARAMS cparams_default = {
    25,    /* int  prm_maxerr;*/
    0,     /* prm_stackalign */
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
    false,  /* char prm_asmfile;*/
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
    false, /* char compile_under_dos;*/
    true,  /* char prm_bepeep;*/
    false,  /* char prm_crtdll;*/
    false,  /* char prm_lscrtdll;*/
    false,  /* char prm_msvcrt;*/
    (asmTypes)0,  /* char prm_assembler;*/
    false,  /* char prm_flat;*/
    false,  /* char prm_nodos;*/
    true,  /* char prm_useesp;*/
    false, /* char managed_library;*/
    false, /* char no_default_libs;*/
    false, /*char replacePInvoke;*/
    true, /* char msilAllowExtensions;*/
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

void OutputIntermediate(SharedMemory* mem);
void regInit() { }
int usingEsp;


#ifdef PARSER_ONLY
int natural_size(EXPRESSION* exp) { return ISZ_UINT; }
#endif

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
                    TYPE *tp = sym->tp;
                    while (ispointer(tp) || isref(tp))
                    {
                        tp = basetype(tp)->btp;
                    }
                    if (!isstructured(tp) || !basetype(tp)->sp->sb->templateLevel || basetype(tp)->sp->sb->instantiated)
                        typedefs.push_back(SymbolManager::Get(sym));
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
    SET_GLOBAL(true, 1);
    LEXEME* lex = nullptr;
    SymbolManager::clear();
    helpinit();
    mangleInit();
    errorinit();
    constoptinit();
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
    if (architecture != ARCHITECTURE_MSIL)
    {
        nextLabel = 1;
    }
#ifndef PARSER_ONLY
    iexpr_init();
    iinlineInit();
    genstmtini();
#endif
    ParseBuiltins();
//    intrinsicInit();
    inlineAsmInit();
//outcodeInit();
//    debuggerInit();
//    browsdataInit();
    browse_init();
    browse_startfile(infile, 0);
#ifndef PARSER_ONLY
    static bool first = true;
    if (architecture == ARCHITECTURE_MSIL)
        if (first || (cparams.prm_compileonly && !cparams.prm_asmfile))
        {
            msil_compile_start((char*)clist->data);
        }
    first = false;
#endif
    if (cparams.prm_assemble)
    {
        lex = getsym();
        if (lex)
        {
            BLOCKDATA block;
            memset(&block, 0, sizeof(block));
            block.type = begin;
            while ((lex = statement_asm(lex, nullptr, &block)) != nullptr)
                ;
#ifndef PARSER_ONLY
            genASM(block.head);
#endif
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
#ifdef PARSER_ONLY
    ccDumpSymbols();
#endif
    if (!TotalErrors())
    {
        dumpInlines();
        dumpInitializers();
        dumpInlines();
        dumpImportThunks();
        dumpStartups();
#ifndef PARSER_ONLY
        dumpLits();
        WeedExterns();
#endif
        /*        rewrite_icode(); */
//        if (chosenAssembler->gen->finalGen)
//            chosenAssembler->gen->finalGen();
        if (!cparams.prm_assemble && cparams.prm_debug)
            debug_dumptypedefs(globalNameSpace);
//        if (!cparams.prm_asmfile)
//            outputObjFile();
        libIncludes = preProcessor->GetIncludeLibs();
    }
    findUnusedStatics(globalNameSpace);
    dumperrs(stdout);
    RELEASE_GLOBAL(1);
}
/*-------------------------------------------------------------------------*/

void enter_filename(const char *name)
{
    inputFiles.push_back(name);
}
int main(int argc, char* argv[])
{
    cparams = cparams_default;
    compilerName = std::string("occ v") + STRING_VERSION + " " + __DATE__;
    char buffer[256];
    char* p;
    bool multipleFiles = false;
    int rv;
    bool compileToFile = false;

    srand(time(0));

    /*   signal(SIGSEGV,internalError) ;*/
    /*   signal(SIGFPE, internalError) ;*/

    /*
    if (chosenAssembler->Args)
    {
        CMDLIST* newArgs = (CMDLIST*)calloc(sizeof(Args) + sizeof(Args[0]) * chosenAssembler->ArgCount, 1);
        if (newArgs)
        {
            memcpy(&newArgs[0], chosenAssembler->Args, chosenAssembler->ArgCount * sizeof(Args[0]));
            memcpy(&newArgs[chosenAssembler->ArgCount], &Args[0], sizeof(Args));
            ArgList = newArgs;
        }
    }
    */
    /* parse environment variables, command lines, and config files  */
    ccinit(argc, argv);

    /* loop through and preprocess all the files on the file list */
    if (clist && clist->next)
        multipleFiles = true;
#ifdef PARSER_ONLY
    strcpy(buffer, (char*)clist->data);
    strcpy(realOutFile, prm_output.GetValue().c_str());
    outputfile(realOutFile, buffer, ".ods");
    if (!ccDBOpen(realOutFile))
        Utils::fatal("Cannot open database file %s", realOutFile);
#else
#ifndef ISPARSER
    BitInit();
    regInit();
#endif
#endif
#ifndef PARSER_ONLY
    instructionParser = new x64Parser();
    SharedMemory* parserMem = nullptr;
    if (bePostFile.size())
    {
        parserMem = new SharedMemory(0, bePostFile.c_str());
        if (!parserMem->Open() || !parserMem->GetMapping())
            Utils::fatal("internal error: invalid shared memory region");
        if (!clist)
        {
            OutputIntermediate(parserMem);
        }
    }
    else // so we can do compiles without the output going anywhere...
    {
        parserMem = new SharedMemory(240*1024*1024);
        parserMem->Create();
        parserMem->GetMapping();
        compileToFile = true;
    }

#endif
    for (auto c = clist; c; c = c->next)
    {
        enter_filename((const char *)c->data);
    }
//    mainPreprocess();
    bool first = true;
    while (clist)
    {
#ifndef PARSER_ONLY
        if (architecture == ARCHITECTURE_MSIL)
        {
            if (first || (cparams.prm_compileonly && !cparams.prm_asmfile))
                msil_main_preprocess((char *)clist->data);
        }
#endif
        first = false;
        Errors::Reset();
        cparams.prm_cplusplus = false;
        strcpy(buffer, (char*)clist->data);
#ifndef PARSER_ONLY
        if (buffer[0] == '-')
            strcpy(buffer, "a.c");
        strcpy(realOutFile, prm_output.GetValue().c_str());
        outputfile(realOutFile, buffer, ".icf");
#else
        ccNewFile(buffer, true);
#endif
        Utils::AddExt(buffer, ".C");
        static const std::list<std::string> cppExtensions = { ".h", ".cpp", ".cxx", ".cc", ".c++" };
        for (auto & str : cppExtensions)
        {
            if (Utils::HasExt(buffer, str.c_str()))
            {
                cparams.prm_cplusplus = true;
                cparams.prm_c99 = cparams.prm_c1x = false;
                break;
            }
        }
        if (cparams.prm_cplusplus && (architecture == ARCHITECTURE_MSIL))
            Utils::fatal("MSIL compiler does not compile C++ files at this time");
        preProcessor = new PreProcessor(buffer, prm_include.GetValue(), prm_sysinclude.GetValue(), true, cparams.prm_trigraph, '#', cparams.prm_charisunsigned,
            !cparams.prm_c99 && !cparams.prm_c1x && !cparams.prm_cplusplus, !cparams.prm_ansi, prm_pipe.GetValue() != "+" ? prm_pipe.GetValue() : "");

        if (!preProcessor->IsOpen())
            exit(1);
        preProcessor->SetExpressionHandler(ParseExpression);
        preProcessor->SetPragmaCatchall([](const std::string&kw, const std::string&tag) {
            bePragma[kw] = tag;
        });

        strcpy(infile, buffer);
        if (cparams.prm_makestubs)
        {
            enter_filename((char*)clist->data);
            MakeStubs();
        }
        else
        {
            if (cparams.prm_cppfile)
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
            if (cparams.prm_errfile)
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
            if (cparams.prm_icdfile)
            {
                Utils::StripExt(buffer);
                Utils::AddExt(buffer, ".icd");
                icdFile = fopen(buffer, "w");
                if (!icdFile)
                {
                    fclose(errFile);
                    delete preProcessor;
                    fclose(cppFile);
                    Utils::fatal("Cannot open error file %s", buffer);
                }
                setvbuf(icdFile, 0, _IOFBF, 32768);
            }

            if (multipleFiles && !cparams.prm_quiet)
                printf("%s\n", (char *)clist->data);
                
            compile(false);
#ifndef PARSER_ONLY

            if (architecture != ARCHITECTURE_MSIL || (cparams.prm_compileonly && !cparams.prm_asmfile))
            {
                OutputIntermediate(parserMem);
                oFree();
            }
            if (cparams.prm_icdfile)
                OutputIcdFile();
            InitIntermediate();
#endif
        }
#ifdef PARSER_ONLY
        localFree();
#endif
        if (architecture != ARCHITECTURE_MSIL || (cparams.prm_compileonly && !cparams.prm_asmfile))
            globalFree();
        if (cparams.prm_diag)
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
        if (icdFile)
            fclose(icdFile);

        /* Flag to stop if there are any errors */
        stoponerr |= TotalErrors();
#ifndef PARSER_ONLY
        if (architecture == ARCHITECTURE_MSIL)
            if (cparams.prm_compileonly && !cparams.prm_asmfile)
                msil_end_generation(nullptr);
#endif
        clist = clist->next;
    }

#ifndef PARSER_ONLY
    if (architecture == ARCHITECTURE_MSIL)
        if (!cparams.prm_compileonly || cparams.prm_asmfile)
        {
            msil_end_generation(nullptr);
            OutputIntermediate(parserMem);
        }
    oFree();
    globalFree();
    if (compileToFile)
    {
        //compile to file
        strcpy(realOutFile, outputFileName.c_str());
        Utils::StripExt(realOutFile);
        Utils::AddExt(realOutFile, ".icf");
        int size = GetOutputSize();
        void *p = parserMem->GetMapping();
        FILE *fil = fopen(realOutFile, "wb");
        if (!fil)
            Utils::fatal("could not open output file");
        fwrite(p, size, 1, fil);
        fclose(fil);
    }
    delete parserMem;
#endif
    rv = !!stoponerr;
#ifdef PARSER_ONLY
// to make testing of error cases possible
    rv = 0;
#endif
    return rv;
}
