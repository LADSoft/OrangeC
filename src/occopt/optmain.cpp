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

#include "ioptimizer.h"
#include "beinterfdefs.h"
#include <cstdio>
#include <ctime>
#include "CmdSwitch.h"
#include "Utils.h"
#include "config.h"
#include "ildata.h"
#include "SharedMemory.h"
#include "ildata.h"
#include "OptUtils.h"
#include "iblock.h"
#include "ilocal.h"
#include "rewritemsil.h"
#include "rewritex86.h"
#include "issa.h"
#include "memory.h"
#include "ipeep.h"
#include "configx86.h"
#include "configmsil.h"
#include "ilive.h"
#include "iflow.h"
#include "irc.h"
#include "irewrite.h"
#include "iout.h"
#include "output.h"
#include "ilstream.h"
#include "ilunstream.h"
#include "iconst.h"
#include "ioptutil.h"
#include "ipinning.h"
#include "optmodules.h"
#include "ilazy.h"
#include "iloop.h"

int usingEsp;

Optimizer::SimpleSymbol* currentFunction;
namespace Parser
{
int anonymousNotAlloc;
bool IsCompiler() { return true; }
}  // namespace Parser

namespace Optimizer
{
CmdSwitchParser SwitchParser;
CmdSwitchBool single(SwitchParser, 's', false, {"single"});
CmdSwitchBool WriteIcdFile(SwitchParser, 'Y', false);
CmdSwitchCombineString output(SwitchParser, 'o');
CmdSwitchBool displayTiming(SwitchParser, 't');
CmdSwitchCombineString prm_flags(SwitchParser, 'f', ';');
CmdSwitchBool useSharedMemory(SwitchParser, 'S');
CmdSwitchString prm_verbosity(SwitchParser, 'y');
CmdSwitchString prm_optimize(SwitchParser, 'O', ';');

const char* usageText =
    "[options] inputfile\n"
    "\n"
    "-f{flag}     set or clear a flag\n"
    "-o{file}     set output file (in file mode)\n"
    "--single     don't open internal file list\n"
    "-t           display timing info\n"
    "-y[...]      set verbosity\n"
    "Ox           optimization control\n"
    "-S use shared memory\n"
    "-Y output icd file\n"
    "\nOptimization control:\n" OPTIMIZATION_DESCRIPTION "\nFlags:\n" OPTMODULES_DESCRIPTION "\nTime: " __TIME__
    "  Date: " __DATE__;

bool InputIntermediate(SharedMemory* mem);
void OutputIntermediate(SharedMemory* mem);

static const int MAX_SHARED_REGION = 500 * 1024 * 1024;

void InternalConflict(QUAD* head)
{
    switch (architecture)
    {
        case ARCHITECTURE_MSIL:
            break;
        case ARCHITECTURE_X86:
            x86InternalConflict(head);
            break;
    }
}
void PreColor(QUAD* head)
{
    switch (architecture)
    {
        case ARCHITECTURE_MSIL:
            break;
        case ARCHITECTURE_X86:
            x86PreColor(head);
            break;
    }
}
void FastcallColor(QUAD* head)
{
    switch (architecture)
    {
        case ARCHITECTURE_MSIL:
            break;
        case ARCHITECTURE_X86:
            x86FastcallColor(head);
            break;
    }
}
void examine_icode(QUAD* head)
{
    switch (architecture)
    {
        case ARCHITECTURE_MSIL:
            msil_examine_icode(head);
            break;
        case ARCHITECTURE_X86:
            x86_examine_icode(head);
            break;
    }
}
int PreRegAlloc(QUAD* tail, BRIGGS_SET* globalVars, BRIGGS_SET* eobGlobals, int pass)
{
    switch (architecture)
    {
        case ARCHITECTURE_MSIL:
            break;
        case ARCHITECTURE_X86:
            return x86PreRegAlloc(tail, globalVars, eobGlobals, pass);
    }
    return 1;
}

void CreateTempsAndBlocks(FunctionData* fd)
{
    if (!blockArray || blockCount > blockMax)
    {
        free(blockArray);
        blockMax = (blockCount + 999) /1000 * 1000;
        blockArray = (BLOCK**)calloc(sizeof(BLOCK *), blockMax);
    }
    for (auto im : fd->imodeList)
    {
        if (im->offset)
        {
            switch (im->offset->type)
            {
                case se_const:
                case se_absolute:
                case se_auto:
                case se_global:
                case se_threadlocal:
                case se_pc:
                case se_structelem:
                case se_tempref:
                    switch (im->mode)
                    {
                        case i_immed:
                            im->offset->sp->imaddress = im;
                            break;
                        case i_direct:
                            im->offset->sp->imvalue = im;
                            break;
                        case i_ind: {
                            IMODELIST* iml = Allocate<IMODELIST>();
                            iml->next = im->offset->sp->imind;
                            iml->im = im;
                            im->offset->sp->imind = iml;
                        }
                        break;
                    }
            }
        }
    }
    for (auto q = fd->instructionList; q; q = q->fwd)
    {
        if (q->dc.opcode == i_block)
        {
            blockArray[q->dc.v.label] = q->block;
        }
    }
}
/* coming into this routine we have two major requirements:
 * first, imodes that describe the same thing are the same object
 * second, identical expressions can be identified in that the temps
 * 		involved each time the expression is evaluated are the same
 */

void Optimize(SimpleSymbol* funcsp)
{
    // printf("optimization start\n");
    weed_goto();

    /*
     * icode optimizations goes here.  Note that LCSE is done through
     * DAG construction during the actual construction of the blocks
     * so- it is already done at this point.
     *
     * Order IS important!!!!!!!!! be careful!!!!!
     *
     * note that some of these optimizations make changes to the code,
     * with the exception of the actual global optimization pass we are
     * never really deleting dead code at the time we make changes
     * becase we aren't 100% certain what will really be dead
     * we do separate dead-code passes occasionally to clean it up
     */
    /* Global opts */

    flows_and_doms();
    gatherLocalInfo(functionVariables);

    RunOptimizerModules();

    if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !functionHasAssembly)
    {
        if (cparams.icd_flags & ICD_QUITEARLY)
            return;
    }
    else
    {
        Precolor(false);
        RearrangePrecolors();
        RemoveCriticalThunks();
        RemoveInfiniteThunks();
    }

    /* backend modifies ICODE to improve code generation */
    examine_icode(intermed_head);

    /* register allocation - this first where we go into SSA form and backi s because
     * at this point for global allocation we had to reuse original
     * register names, but the register allocation phase works better
     * when registers are disentangled and have smaller lifetimes
     *
     * while we are back in SSA form we do some improvements to the code that will
     * help in register allocation and code generation.
     */
    definesInfo();
    liveVariables();
    doms_only(true);
    TranslateToSSA();
    CalculateInduction();
    /* lower for backend, e.g. do transformations that will improve the eventual
     * code gen, such as picking scaled indexed modes, moving constants, etc...
     */
    Prealloc(1);
    TranslateFromSSA(!(chosenAssembler->arch->denyopts & DO_NOREGALLOC));
    peep_icode(false); /* peephole optimizations at the ICODE level */
    RemoveCriticalThunks();
    removeDead(blockArray[0]); /* remove dead blocks */

    /* now do the actual allocation */
    if (!(chosenAssembler->arch->denyopts & DO_NOREGALLOC))
    {
        AllocateRegisters(intermed_head);
        /* backend peephole optimization can sometimes benefit by knowing what is live */

        CalculateBackendLives();
    }
    sFree();
    if (pinning)
    {
        RewriteForPinning();
    }
    peep_icode(true); /* we do branche opts last to not interfere with other opts */
}

void ProcessFunction(FunctionData* fd)
{
    SetUsesESP(currentFunction->usesEsp);
    Parser::anonymousNotAlloc = 0;
    CreateTempsAndBlocks(fd);
    Optimize(currentFunction);

    if (!(chosenAssembler->arch->denyopts & DO_NOREGALLOC))
        AllocateStackSpace();
    FillInPrologue(intermed_head, currentFunction);
    // post_function_gen(currentFunction, intermed_head);
    tFree();
    oFree();
}
void ProcessFunctions()
{
    for (auto v : baseData)
    {
        if (v->type == DT_FUNC)
        {
            temporarySymbols = v->funcData->temporarySymbols;
            functionVariables = v->funcData->variables;
            blockCount = v->funcData->blockCount;
            exitBlock = v->funcData->exitBlock;
            fastcallAlias = v->funcData->fastcallAlias;
            tempCount = v->funcData->tempCount;
            functionHasAssembly = v->funcData->hasAssembly;
            intermed_head = v->funcData->instructionList;
            intermed_tail = intermed_head;
            while (intermed_tail && intermed_tail->fwd)
                intermed_tail = intermed_tail->fwd;
            fltexp = v->funcData->fltexp;
            currentFunction = v->funcData->name;
            loadHash = v->funcData->loadHash;
            ProcessFunction(v->funcData);
            v->funcData->temporarySymbols = temporarySymbols;
            v->funcData->variables = functionVariables;
            v->funcData->blockCount = blockCount;
            v->funcData->exitBlock = exitBlock;
            v->funcData->fastcallAlias = fastcallAlias;
            v->funcData->tempCount = tempCount;
            v->funcData->instructionList = intermed_head;
            v->funcData->fltexp = fltexp;
        }
    }
}
bool LoadFile(SharedMemory* parserMem)
{
    bool rv = InputIntermediate(parserMem);
    flow_init();
    BitInit();
    SSAInit();
    oinit();
    constoptinit();
    SelectBackendData();
    return rv;
}
void SaveFile(std::string& name, SharedMemory* optimizerMem)
{
    OutputIntermediate(optimizerMem);
    if (WriteIcdFile.GetValue() || (cparams.prm_icdfile && !name.empty()))
    {
        char buf[260];
        strcpy(buf, name.c_str());
        Utils::StripExt(buf);
        Utils::AddExt(buf, ".icd2");
        icdFile = fopen(buf, "w");
        if (!icdFile)
            return;
        OutputIcdFile();
        fclose(icdFile);
        icdFile = nullptr;
    }
    InitIntermediate();
    localFree();
    globalFree();
}
void ParseParams(char* argv[])
{
    std::vector<std::string> checks = Utils::split(prm_optimize.GetValue());
    for (auto&& v : checks)
    {
        if (v.size() >= 1)
        {
            Optimizer::optimize_setup('O', v.c_str());
        }
    }
    if (ParseOptimizerParams(prm_flags.GetValue()) != "")
        Utils::usage(argv[0], usageText);
    if (prm_verbosity.GetExists())
    {
        cparams.verbosity = 1 + prm_verbosity.GetValue().size();
    }
}
}  // namespace Optimizer
int main(int argc, char* argv[])
{
    using namespace Optimizer;
    unsigned startTime, stopTime;
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");

    if (!SwitchParser.Parse(&argc, argv) || argc < 2 || argc > 3)
    {
        Utils::usage(argv[0], usageText);
    }
    cparams.optimizer_modules = ~0;
    bool fileMode = true;
    if (useSharedMemory.GetValue())
    {
        if (argc != 3)
            Utils::usage(argv[0], usageText);
        fileMode = false;
    }
    else if (argc != 2)
    {
        Utils::usage(argv[0], usageText);
    }
    SharedMemory* parserMem = nullptr;
    SharedMemory* optimizerMem = nullptr;
    std::string outputFile;
    if (fileMode)
    {
        if (output.GetExists())
        {
            outputFile = output.GetValue();
        }
        else
        {
            char buf[260];
            strcpy(buf, argv[1]);
            Utils::StripExt(buf);
            strcat(buf, "_1");
            Utils::AddExt(buf, ".icf");
            outputFile = buf;
        }
        FILE* fil = fopen(argv[1], "rb");
        if (fil)
        {
            parserMem = new SharedMemory(MAX_SHARED_REGION);
            parserMem->Create();
            Optimizer::ReadMappingFile(parserMem, fil);
            fclose(fil);
            optimizerMem = new SharedMemory(MAX_SHARED_REGION);
            optimizerMem->Create();
        }
        else
        {
            Utils::fatal("cannot open input file");
        }
    }
    else
    {
        parserMem = new SharedMemory(0, argv[1]);
        optimizerMem = new SharedMemory(0, argv[2]);
        if (!parserMem->Open() || !optimizerMem->Open())
        {
            Utils::fatal("invalid shared memory specifiers");
        }
    }
    if (!LoadFile(parserMem))
        Utils::fatal("internal error: could not load intermediate file");
    Optimizer::ParseParams(argv);
    Optimizer::OptimizerStats();
    if (Optimizer::cparams.prm_displaytiming || displayTiming.GetValue())
    {
        startTime = clock();
    }
    regInit();
    alloc_init();
    ProcessFunctions();
    std::string aa = inputFiles.size() ? inputFiles.front() : "";
    SaveFile(aa, optimizerMem);
    if (architecture != ARCHITECTURE_MSIL || (cparams.prm_compileonly && !cparams.prm_asmfile))
    {
        if (!single.GetValue() && inputFiles.size())
        {
            std::list<std::string> files = inputFiles;
            files.pop_front();
            for (auto p : files)
            {
                if (!LoadFile(parserMem))
                    Utils::fatal("internal error: could not load intermediate file");
                ProcessFunctions();
                SaveFile(p, optimizerMem);
            }
        }
    }
    if (fileMode)
    {
        // compile to file
        FILE* fil = fopen(outputFile.c_str(), "wb");
        if (!fil)
            Utils::fatal("Cannot open '%s' for write", outputFile.c_str());
        Optimizer::WriteMappingFile(optimizerMem, fil);
        fclose(fil);
    }
    delete parserMem;
    delete optimizerMem;
    if (Optimizer::cparams.prm_displaytiming || displayTiming.GetValue())
    {
        stopTime = clock();
        printf("occopt timing: %d.%03d\n", (stopTime - startTime) / 1000, (stopTime - startTime) % 1000);
    }
    return 0;
}