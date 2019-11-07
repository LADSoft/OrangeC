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
#include "iexpr.h"
#include "beinterf.h"
#include <stdio.h>
#include "CmdSwitch.h"
#include "Utils.h"
#include "ildata.h"

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
extern BLOCK** blockArray;
extern ARCH_ASM* chosenAssembler;
extern SimpleExpression* objectArray_exp;
extern TEMP_INFO** tempInfo;
extern int blockMax;
extern int tempMax;

CmdSwitchParser SwitchParser;
CmdSwitchBool single(SwitchParser, 's', false, "single");
CmdSwitchBool WriteIcdFile(SwitchParser, 'Y', false);

const char* usageText =
"[options] inputfile\n"
"\n"
"--single     don't open internal file list\n"
"-Y output icd file\n"
"\nTime: " __TIME__ "  Date: " __DATE__;

FILE* inputFile;

FILE* outputFile;
int anonymousNotAlloc;
SimpleSymbol* currentFunction;
int usingEsp;

static bool functionHasAssembly;

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
    blockArray = (BLOCK**)Alloc(sizeof(BLOCK*) * blockCount);
    blockMax = blockCount;
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
                case i_ind:
                {
                    IMODELIST *iml = (IMODELIST*)Alloc(sizeof(IMODE));
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
    static int optflags = ~0;
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
    if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !functionHasAssembly)
    {
        Precolor();
        RearrangePrecolors();
        // printf("ssa\n");
        TranslateToSSA();

        // printf("const\n");
        if (optflags & OPT_CONSTANT)
        {
            ConstantFlow(); /* propagate constants */
            RemoveInfiniteThunks();
            //			RemoveCriticalThunks();
            doms_only(false);
        }
        //		if (optflags & OPT_RESHAPE)
        //			Reshape();		/* loop expression reshaping */
        // printf("stren\n");
        if (!(chosenAssembler->arch->denyopts & DO_NOGLOBAL))
        {
            if ((cparams.prm_optimize_for_speed) && (optflags & OPT_LSTRENGTH))
                ReduceLoopStrength(); /* loop index variable strength reduction */
                                      // printf("invar\n");
            if ((cparams.prm_optimize_for_speed) && (optflags & OPT_INVARIANT))
                MoveLoopInvariants(); /* move loop invariants out of loops */
        }
        if ((optflags & OPT_GLOBAL) && !(chosenAssembler->arch->denyopts & DO_NOGLOBAL))
        {
            // printf("alias\n");
            AliasPass1();
        }
        // printf("ssa out\n");
        TranslateFromSSA(false);
        removeDead(blockArray[0]);
        //		RemoveCriticalThunks();
        if ((optflags & OPT_GLOBAL) && !(chosenAssembler->arch->denyopts & DO_NOGLOBAL))
        {
            // printf("alias 2\n");
            SetGlobalTerms();
            AliasPass2();
            // printf("global\n");
            GlobalOptimization(); /* partial redundancy, code motion */
            AliasRundown();
        }
        nextTemp = tempBottom;
        // printf("end opt\n");
        RemoveCriticalThunks();
        removeDead(blockArray[0]);
        RemoveInfiniteThunks();
    }
    else
    {
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
    // printf("to ssa\n");
    TranslateToSSA();
    CalculateInduction();
    /* lower for backend, e.g. do transformations that will improve the eventual
     * code gen, such as picking scaled indexed modes, moving constants, etc...
     */
     // printf("prealloc\n");
    Prealloc(1);
    // printf("from ssa\n");
    TranslateFromSSA(!(chosenAssembler->arch->denyopts & DO_NOREGALLOC));
    // printf("peep\n");
    peep_icode(false); /* peephole optimizations at the ICODE level */
    RemoveCriticalThunks();
    removeDead(blockArray[0]); /* remove dead blocks */

    // printf("allocate\n");
    /* now do the actual allocation */
    if (!(chosenAssembler->arch->denyopts & DO_NOREGALLOC))
    {
        AllocateRegisters(intermed_head);
        /* backend peephole optimization can sometimes benefit by knowing what is live */
        // printf("live\n");

        CalculateBackendLives();
    }
    sFree();
    peep_icode(true); /* we do branche opts last to not interfere with other opts */
                      // printf("optimzation done\n");
}

void ProcessFunction(FunctionData* fd)
{
    SetUsesESP(currentFunction->usesEsp);
    anonymousNotAlloc = 0;
    CreateTempsAndBlocks(fd);
    Optimize(currentFunction);

    if (!(chosenAssembler->arch->denyopts & DO_NOREGALLOC))
        AllocateStackSpace();
    FillInPrologue(intermed_head, currentFunction);
    // post_function_gen(currentFunction, intermed_head);
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
            tempCount = v->funcData->tempCount;
            functionHasAssembly = v->funcData->hasAssembly;
            intermed_head = v->funcData->instructionList;
            intermed_tail = intermed_head;
            while (intermed_tail && intermed_tail->fwd)
                intermed_tail = intermed_tail->fwd;
            objectArray_exp = v->funcData->objectArray_exp;
            currentFunction = v->funcData->name;
            ProcessFunction(v->funcData);
            v->funcData->temporarySymbols = temporarySymbols;
            v->funcData->variables = functionVariables;
            v->funcData->blockCount = blockCount;
            v->funcData->exitBlock = exitBlock;
            v->funcData->tempCount = tempCount;
            v->funcData->instructionList = intermed_head;
        }
    }
}
bool LoadFile(const char *name)
{
    char buf[260];
    strcpy(buf, name);
    Utils::StripExt(buf);
    Utils::AddExt(buf, ".icf");
    inputFile = fopen(buf, "rb");
    if (!inputFile)
        return false;
    InitIntermediate();
    bool rv = InputIntermediate();
    fclose(inputFile);
    flow_init();
    BitInit();
    SSAInit();
    oinit();
    SelectBackendData();
    return rv;
}
void SaveFile(const char *name)
{
    char buf[260];
    strcpy(buf, name);
    Utils::StripExt(buf);
    Utils::AddExt(buf, ".icf");
    outputFile = fopen(buf, "wb");
    if (!outputFile)
        return;
    OutputIntermediate();
    fclose(outputFile);
    if (WriteIcdFile.GetValue() || cparams.prm_icdfile)
    {
        char buf[260];
        strcpy(buf, name);
        Utils::StripExt(buf);
        Utils::AddExt(buf, ".icd2");
        icdFile = fopen(buf, "w");
        if (!icdFile)
            return;
        OutputIcdFile();
        fclose(icdFile);
        icdFile = nullptr;
    }
}
bool Matches(const char *arg, const char *cur)
{
    const char *l = strrchr(arg, '\\');
    if (!l)
        l = arg;
    const char *r = strrchr(cur, '\\');
    if (!r)
        r = cur;
    return Utils::iequal(l, r);
}
int main(int argc, char* argv[])
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");

    if (!SwitchParser.Parse(&argc, argv) || argc != 2)
    {
        Utils::usage(argv[0], usageText);
    }
    if (!LoadFile(argv[1]))
        return 1;
    regInit();
    alloc_init();
    ProcessFunctions();
    SaveFile(argv[1]);
    if (!single.GetValue())
    {
        std::list<std::string> files = inputFiles;
        for (auto p : files)
        {
            if (!Matches(argv[1], p.c_str()))
            {
                if (!LoadFile(p.c_str()))
                    return 1;
                ProcessFunctions();
                SaveFile(p.c_str());
            }
        }
    }
    return 0;
}
