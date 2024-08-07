/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include "optmodules.h"
#include "ioptimizer.h"
#include "ildata.h"
#include "config.h"
#include "optmain.h"
#include "issa.h"
#include "ialias.h"
#include "ilive.h"
#include "ilazy.h"
#include "iloop.h"
#include "iflow.h"
#include "istren.h"
#include "iinvar.h"
#include "iblock.h"
#include "ilocal.h"
#include "irc.h"
#include "iconst.h"
#include "Utils.h"
namespace Optimizer
{
struct OptimizerModule
{
    void (*func)();
    const char* friendlyName;
    int optMask;
    int denyMask;
    bool needsSpeed;
    bool needsSize;
};
// Precolor(true);
// RearrangePrecolors();
//// printf("ssa\n");
// TranslateToSSA();

//// printf("const\n");
// if (optflags & OPT_CONSTANT)
//{
//    //ConstantFlow(); /* propagate constants */
//    RemoveInfiniteThunks();
//    //			RemoveCriticalThunks();
//    doms_only(false);
//}
////		if (optflags & OPT_RESHAPE)
////			Reshape();		/* loop expression reshaping */
//// printf("stren\n");
// if (!(chosenAssembler->arch->denyopts & DO_NOGLOBAL))
//{
//    if ((cparams.prm_optimize_for_speed) && (optflags & OPT_LSTRENGTH))
//        ReduceLoopStrength(); /* loop index variable strength reduction */
//                              // printf("invar\n");
//    if ((cparams.prm_optimize_for_speed) && (optflags & OPT_INVARIANT))
//        MoveLoopInvariants(); /* move loop invariants out of loops */
//}
// if ((optflags & OPT_GCSE) && !(chosenAssembler->arch->denyopts & DO_NOGCSE))
//{
//    // printf("alias\n");
//    AliasPass1();
//}
//// printf("ssa out\n");
// TranslateFromSSA(false);
// removeDead(blockArray[0]);
////		RemoveCriticalThunks();
// if ((optflags & OPT_GLOBAL) && !(chosenAssembler->arch->denyopts & DO_NOGLOBAL))
//{
//    // printf("alias 2\n");
//    SetGlobalTerms();
//    AliasPass2();
//    // printf("global\n");
//    GlobalOptimization(); /* partial redundancy, code motion */
//    AliasRundown();
//}
// nextTemp = tempBottom;
//// printf("end opt\n");
// RemoveCriticalThunks();
// removeDead(blockArray[0]);
// RemoveInfiniteThunks();

void OptimizePrecolor() { Precolor(true); }
void SSAIn() { TranslateToSSA(); }
void SSAOut() { TranslateFromSSA(false); }
void RemoveDead() { removeDead(blockArray[0]); }
void ResetTempBottom() { nextTemp = tempBottom; }
void RedoDoms() { doms_only(false); }
OptimizerModule Modules[]{
    {OptimizePrecolor, nullptr, ~0, 0, false, false},  // Precolor(true);
    {RearrangePrecolors, nullptr, ~0, 0, false, false},
    {SSAIn, nullptr, ~0, 0, false, false},
    {ConstantFlow, "Constant Optimization", OPT_CONSTANT, DO_NOCONST, false, false},
    {RemoveInfiniteThunks, nullptr, OPT_CONSTANT, 0, false, false},
    ////    { RemoveCriticalThunks, nullptr, OPT_CONSTANT, 0, false, false },
    {RedoDoms, nullptr, OPT_CONSTANT, 0, false, false},
    ////    { Reshape, "Loop reshaping", OPT_RESHAPE, 0, false, false },
    {ReduceLoopStrength, "Reduce Loop Strength", OPT_LSTRENGTH, DO_NOGCSE, true, false},
    {MoveLoopInvariants, "Move Loop Invariants", OPT_INVARIANT, DO_NOGCSE, true, false},
    {AliasPass1, nullptr, ~0, DO_NOALIAS, false, false},
    {SSAOut, nullptr, ~0, 0, false, false},
    {RemoveDead, nullptr, ~0, 0, false, false},
    {SetGlobalTerms, nullptr, ~0, 0, false, false},
    {AliasPass2, nullptr, ~0, DO_NOALIAS, false, false},
    {RemoveCriticalThunks, nullptr, ~0, 0, false, false},
    {GlobalOptimization, "Lazy global optimization", OPT_GCSE, DO_NOGCSE, false, false},
    {AliasRundown, nullptr, ~0, DO_NOALIAS, false, false},
    {ResetTempBottom, nullptr, ~0, 0, false, false},
    {RemoveDead, nullptr, ~0, 0, false, false},
    {RemoveInfiniteThunks, nullptr, ~0, 0, false, false},
};

void OptimizerStats()
{
    if (Optimizer::cparams.verbosity >= 4)
    {
        printf("Optimizing ");
        if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size))
        {
            printf("enabled\n");
            for (auto m : Modules)
            {
                if (m.friendlyName)
                {
                    bool running = !!(Optimizer::cparams.optimizer_modules & m.optMask);
                    bool denied = !!(chosenAssembler->arch->denyopts & m.denyMask);
                    bool hasSpeed = !m.needsSpeed || Optimizer::cparams.prm_optimize_for_speed;
                    bool hasSize = !m.needsSize || Optimizer::cparams.prm_optimize_for_size;
                    std::string error;
                    if (denied)
                        error += "(Optimization denied by backend)";
                    else if (!running)
                        error += "(optimization not selected)";
                    else if (!hasSpeed)
                        error += "(needs speed optimization)";
                    else if (!hasSize)
                        error += "(needs size optimization)";
                    else
                        error += "optimization will run";
                    printf("  %s: %s\n", m.friendlyName, error.c_str());
                }
            }
        }
        else
        {
            printf("disabled\n");
        }
    }
}

void RunOptimizerModules()
{
    if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size))
    {
        if (!functionHasAssembly)
        {
            if (Optimizer::cparams.verbosity >= 5)
                printf("Optimizing: %s\n", currentFunction->name);
            for (auto m : Modules)
            {
                bool running = !!(Optimizer::cparams.optimizer_modules & m.optMask);
                bool denied = !!(chosenAssembler->arch->denyopts & m.denyMask);
                bool hasSpeed = !m.needsSpeed || Optimizer::cparams.prm_optimize_for_speed;
                bool hasSize = !m.needsSize || Optimizer::cparams.prm_optimize_for_size;
                if (running && !denied && hasSpeed && hasSize)
                {
                    if (m.friendlyName && Optimizer::cparams.verbosity >= 5)
                    {
                        printf("Running: %s\n", m.friendlyName);
                    }
                    m.func();
                }
            }
        }
        else if (Optimizer::cparams.verbosity >= 5)
        {
            printf("Not optimizing: %s (function has assembler instructions)\n", currentFunction->name);
        }
    }
}
}  // namespace Optimizer
