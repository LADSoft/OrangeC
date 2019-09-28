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

#include "compiler.h"
#include "PreProcessor.h"
#include "Utils.h"
#include "CmdSwitch.h"
#include <setjmp.h>
#include "../../version.h"
extern ARCH_DEBUG* chosenDebugger;
extern ARCH_ASM* chosenAssembler;
extern NAMESPACEVALUELIST* globalNameSpace;
extern LIST* clist;
extern FILE* outputFile;
/*extern int prm_peepopt; */
extern FILE* listFile;
extern int optflags;
extern CmdSwitchCombineString prm_include;
extern CmdSwitchCombineString prm_sysinclude;
extern CmdSwitchCombineString prm_libpath;
extern CmdSwitchString prm_pipe;
extern CmdSwitchCombineString prm_output;

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
#endif

PreProcessor *preProcessor;
int verbosity = 0;

int maxBlocks, maxTemps, maxAllocationSpills, maxAllocationPasses, maxAllocationAccesses;
char cppfile[256];
FILE *cppFile, *browseFile;

FILE *errFile, *icdFile;
char infile[256];

static char tempOutFile[260];
char realOutFile[260];
static char oldOutFile[260];

static int stoponerr = 0;

COMPILER_PARAMS cparams = {
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
    true,  /* char prm_asmfile;*/
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
};

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
                if (sym->storage_class == sc_namespace)
                {
                    debug_dumptypedefs(sym->nameSpaceValues);
                }
                else if (istype(sym))
                    chosenDebugger->outputtypedef(sym);
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
    LEXEME* lex = nullptr;
    SetGlobalFlag(true);
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
#ifndef PARSER_ONLY
    SSAInit();
    outcodeini();
    conflictini();
    iexpr_init();
    iinlineInit();
    flow_init();
    genstmtini();
#endif
    ParseBuiltins();
    if (chosenAssembler->intrinsicInit)
        chosenAssembler->intrinsicInit();
    if (chosenAssembler->inlineAsmInit)
        chosenAssembler->inlineAsmInit();
    if (chosenAssembler->outcode_init)
        chosenAssembler->outcode_init();
    if (chosenAssembler->enter_filename)
        chosenAssembler->enter_filename((char*)clist->data);
    if (cparams.prm_debug && chosenDebugger && chosenDebugger->init)
        chosenDebugger->init();
    if (cparams.prm_browse && chosenDebugger && chosenDebugger->init_browsedata)
        chosenDebugger->init_browsedata((char*)clist->data);
    browse_init();
    browse_startfile(infile, 0);
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
#ifndef PARSER_ONLY
        asm_header((char*)clist->data, STRING_VERSION);
#endif
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
        dumpStartups();
#ifndef PARSER_ONLY
        dumpLits();
#endif
        /*        rewrite_icode(); */
        if (chosenAssembler->gen->finalGen)
            chosenAssembler->gen->finalGen();
        if (!cparams.prm_assemble && cparams.prm_debug)
            if (chosenDebugger && chosenDebugger->outputtypedef)
                debug_dumptypedefs(globalNameSpace);
#ifndef PARSER_ONLY
        putexterns();
#endif
        if (!cparams.prm_asmfile)
            if (chosenAssembler->output_obj_file)
                chosenAssembler->output_obj_file();
    }
    findUnusedStatics(globalNameSpace);
    dumperrs(stdout);
    if (cparams.prm_debug && chosenDebugger && chosenDebugger->rundown)
        chosenDebugger->rundown();
    if (cparams.prm_browse && chosenDebugger && chosenDebugger->rundown_browsedata)
        chosenDebugger->rundown_browsedata();
#ifndef PARSER_ONLY
    if (!cparams.prm_assemble)
        asm_trailer();
#endif
}
/*-------------------------------------------------------------------------*/

void Cleanup()
{
    if (outputFile)
        fclose(outputFile);
    unlink(realOutFile);
    unlink(tempOutFile);
    rename(oldOutFile, realOutFile);
}
int main(int argc, char* argv[])
{
    char buffer[256];
    char* p;
    bool multipleFiles = false;
    bool openOutput = true;
    int rv;

    srand(time(0));

    /*   signal(SIGSEGV,internalError) ;*/
    /*   signal(SIGFPE, internalError) ;*/

    /* initialize back end */
    if (!init_backend(&argc, argv))
        Utils::fatal("Could not initialize back end");

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
    BitInit();
    regInit();
#endif
    if (chosenAssembler->main_preprocess)
        openOutput = chosenAssembler->main_preprocess();
    while (clist)
    {
        cparams.prm_cplusplus = false;
        strcpy(buffer, (char*)clist->data);
#ifndef PARSER_ONLY
        if (buffer[0] == '-')
            strcpy(buffer, "a.c");
        strcpy(realOutFile, prm_output.GetValue().c_str());
        strcpy(tempOutFile, realOutFile);
        outputfile(tempOutFile, buffer, ".oo");
        if (cparams.prm_asmfile)
        {
            outputfile(realOutFile, buffer, chosenAssembler->asmext);
        }
        else
        {
            outputfile(realOutFile, buffer, chosenAssembler->objext);
        }
        strcpy(oldOutFile, realOutFile);
        StripExt(oldOutFile);
        AddExt(oldOutFile, ".tmp");
#else
        ccNewFile(buffer, true);
#endif
        AddExt(buffer, ".C");
        p = strrchr(buffer, '.');
        if (*(p - 1) != '.')
        {
            if (p[1] == 'h' || p[1] == 'H')  // compile H files as C++ for the IDE
                cparams.prm_cplusplus = true;
            if (p[1] == 'c' || p[1] == 'C')
                if (p[2] == 'p' || p[2] == 'P')
                {
                    if (p[3] == 'p' || p[3] == 'P')
                        cparams.prm_cplusplus = true;
                }
                else
                {
                    if (p[2] == 'x' || p[2] == 'X')
                    {
                        if (p[3] == 'x' || p[3] == 'X')
                            cparams.prm_cplusplus = true;
                    }
                }
            else if ((p[2] == 'c' || p[2] == 'C') && !p[3])
            {
                cparams.prm_cplusplus = true;
            }
            else
            {
                if (p[2] == '+')
                {
                    if (p[3] == '+')
                        cparams.prm_cplusplus = true;
                }
            }
        }
        if (cparams.prm_cplusplus)
            cparams.prm_c99 = cparams.prm_c1x = false;
        if (cparams.prm_cplusplus && chosenAssembler->msil)
            Utils::fatal("MSIL compiler does not compile C++ files at this time");
        preProcessor = new PreProcessor(buffer, prm_include.GetValue(), prm_sysinclude.GetValue(), true, cparams.prm_trigraph, '#', cparams.prm_charisunsigned,
            !cparams.prm_c99 && !cparams.prm_c1x && !cparams.prm_cplusplus, !cparams.prm_ansi, prm_pipe.GetValue());

        if (!preProcessor->IsOpen())
            exit(1);
        if (chosenAssembler->doPragma)
        {
            preProcessor->SetPragmaCatchall([](const std::string&kw, const std::string&tag) {
                chosenAssembler->doPragma(kw.c_str(), tag.c_str());
            });
        }

        strcpy(infile, buffer);
        if (cparams.prm_makestubs)
        {
            if (chosenAssembler->enter_filename)
                chosenAssembler->enter_filename((char*)clist->data);
            MakeStubs();
        }
        else
        {
#ifndef PARSER_ONLY
            if (openOutput)
            {
                unlink(oldOutFile);
                rename(realOutFile, oldOutFile);
                outputFile = fopen(tempOutFile, cparams.prm_asmfile ? "w" : "wb");
                if (!outputFile)
                {
                    delete preProcessor;
                    Utils::fatal("Cannot open output file %s", tempOutFile);
                }
                setvbuf(outputFile, 0, _IOFBF, 32768);
            }
#endif
            if (cparams.prm_cppfile)
            {
                StripExt(buffer);
                AddExt(buffer, ".i");
                strcpy(cppfile, buffer);
                cppFile = fopen(buffer, "w");
                if (!cppFile)
                {
                    delete preProcessor;
                    fclose(outputFile);
                    Utils::fatal("Cannot open preprocessor output file %s", buffer);
                }
            }
            if (cparams.prm_listfile)
            {
                StripExt(buffer);
                AddExt(buffer, ".lst");
                listFile = fopen(buffer, "w");
                if (!listFile)
                {
                    delete preProcessor;
                    fclose(cppFile);
                    fclose(outputFile);
                    Utils::fatal("Cannot open list file %s", buffer);
                }
            }
            if (cparams.prm_errfile)
            {
                StripExt(buffer);
                AddExt(buffer, ".err");
                errFile = fopen(buffer, "w");
                if (!errFile)
                {
                    delete preProcessor;
                    fclose(cppFile);
                    fclose(listFile);
                    fclose(outputFile);
                    Utils::fatal("Cannot open error file %s", buffer);
                }
            }
            if (cparams.prm_browse)
            {
                char name[260];
                strcpy(name, realOutFile);
                StripExt(name);
                AddExt(name, ".cbr");
                browseFile = fopen(name, "wb");
                if (!browseFile)
                {
                    fclose(errFile);
                    delete preProcessor;
                    fclose(cppFile);
                    fclose(listFile);
                    fclose(outputFile);
                    Utils::fatal("Cannot open browse file %s", buffer);
                }
                setvbuf(browseFile, 0, _IOFBF, 32768);
            }
            if (cparams.prm_icdfile)
            {
                StripExt(buffer);
                AddExt(buffer, ".icd");
                icdFile = fopen(buffer, "w");
                if (!icdFile)
                {
                    fclose(browseFile);
                    fclose(errFile);
                    delete preProcessor;
                    fclose(cppFile);
                    fclose(listFile);
                    fclose(outputFile);
                    Utils::fatal("Cannot open error file %s", buffer);
                }
                setvbuf(icdFile, 0, _IOFBF, 32768);
            }

            if (multipleFiles && !cparams.prm_quiet)
                printf("%s\n", (char *)clist->data);

            compile(!openOutput);
        }
#ifdef PARSER_ONLY
        localFree();
#endif
        globalFree();
        if (cparams.prm_diag)
        {
            mem_summary();
            printf("Intermediate stats:\n");
            printf("  Block peak:          %d\n", maxBlocks);
            printf("  Temp peak:           %d\n", maxTemps);
            printf("  Allocation Spills:   %d\n", maxAllocationSpills);
            printf("  Allocation Passes:   %d\n", maxAllocationPasses);
            printf("  Allocation Accesses: %d\n", maxAllocationAccesses);
        }
        maxBlocks = maxTemps = maxAllocationSpills = maxAllocationPasses = maxAllocationAccesses = 0;
        delete preProcessor;
        if (outputFile && openOutput)
            fclose(outputFile);
        outputFile = nullptr;
        if (cppFile)
            fclose(cppFile);
        if (listFile)
            fclose(listFile);
        if (errFile)
            fclose(errFile);
        if (browseFile)
            fclose(browseFile);
        if (icdFile)
            fclose(icdFile);

        if (openOutput)
        {
            if (TotalErrors())
            {
                Cleanup();
            }
            else
            {
                unlink(oldOutFile);
                rename(tempOutFile, realOutFile);
            }
        }
        /* Flag to stop if there are any errors */
        stoponerr |= TotalErrors();

        clist = clist->next;
    }
    if (chosenAssembler->main_postprocess)
        chosenAssembler->main_postprocess(stoponerr);
    rv = !!stoponerr;
    if (!cparams.prm_makestubs)
    {
        if (!stoponerr)
        {
            rv = 0;
            if (chosenAssembler->compiler_postprocess)
            {
#ifdef _WIN32
                GetModuleFileNameA(NULL, buffer, sizeof(buffer));
#else
                strcpy(buffer, argv[0]);
#endif
                rv = chosenAssembler->compiler_postprocess(buffer);
            }
        }
        if (chosenAssembler->rundown)
            chosenAssembler->rundown();
    }
#ifdef PARSER_ONLY
// to make testing of error cases possible
    rv = 0;
#endif
    return rv;
}
