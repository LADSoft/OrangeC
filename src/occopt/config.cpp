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

#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "Utils.h"
#ifndef ORANGE_NO_MSIL
#include "configmsil.h"
#endif
#include "configx86.h"
#include "ildata.h"

namespace Optimizer
{
ARCH_ASM* chosenAssembler;
ARCH_DEBUG* chosenDebugger;

enum asmTypes : int;

ARCH_ASM* assemblerInterfaces[] = {x86AssemblerInterface
                                   #ifndef ORANGE_NO_MSIL
                                   , msilAssemblerInterface
                                   #endif
                                  };

static ARCH_ASM* assemblerInterface;

void SelectBackendData()
{
    switch (architecture)
    {
#ifndef ORANGE_NO_MSIL
        case ARCHITECTURE_MSIL:
            assemblerInterface = msilAssemblerInterface;
            break;
#endif
        case ARCHITECTURE_X86:
            assemblerInterface = x86AssemblerInterface;
            break;
        default:
            return;  // failure
    }
    char assembler[100], debugger[100];
    int i, rv;
    assembler[0] = debugger[0] = 0;
    if (cparams.prm_asmfile)
    {
        const char* p = prm_assemblerSpecifier.c_str();
        char* q = assembler;
        cparams.prm_compileonly = true;
        while (*p && !isspace(*p) && *p != ';')
            *q++ = *p++;
        *q = 0;
        if (*p == ';')
        {
            q = debugger;
            p++;
            while (*p && !isspace(*p))
                *q++ = *p++;
            *q = 0;
        }
    }
    if (assembler[0])
    {
        ARCH_ASM* a = assemblerInterface;
        while (a->name)
        {
            if (!strcmp(a->name, assembler))
            {
                Optimizer::chosenAssembler = a;
                break;
            }
            a++;
        }
        if (!a->name)
            Utils::Fatal("Chosen assembler format '%s' not found", assembler);
    }
    else
    {
        Optimizer::chosenAssembler = assemblerInterface;
    }
    if (Optimizer::chosenAssembler->init)
        Optimizer::chosenAssembler->init(&cparams, Optimizer::chosenAssembler, chosenDebugger);
}
}  // namespace Optimizer
