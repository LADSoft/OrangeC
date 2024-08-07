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
#include "DotNetPELib.h"
#include "beinterfdefs.h"
#include <vector>
/*
 *      code generation structures and constants
 */

/* address mode specifications */

#define MAX_SEGS browseseg + 1

struct asm_details
{
    char* name;
};
enum asmTypes : int
{
    pa_nasm,
    pa_fasm,
    pa_masm,
    pa_tasm
};

using namespace DotNetPELib;

namespace occmsil
{
struct byLabel
{
    bool operator()(const Optimizer::SimpleSymbol* left, const Optimizer::SimpleSymbol* right) const
    {
        if (left->storage_class == Optimizer::scc_localstatic || right->storage_class == Optimizer::scc_localstatic || left->storage_class == Optimizer::scc_constant || right->storage_class == Optimizer::scc_constant)
        {
            if (left->storage_class != right->storage_class)
                return left->storage_class < right->storage_class;
            return left->outputName < right->outputName;
        }
        return left->label < right->label;
    }
};
struct byField
{
    bool operator()(const Optimizer::SimpleSymbol* left, const Optimizer::SimpleSymbol* right) const
    {
        int n = strcmp(left->parentClass->name, right->parentClass->name);
        if (n < 0)
        {
            return true;
        }
        else if (n > 0)
        {
            return false;
        }
        else
        {
            return strcmp(left->name, right->name) < 0;
        }
    }
};

struct byName
{
    bool operator()(const Optimizer::SimpleSymbol* left, const Optimizer::SimpleSymbol* right) const
    {
        return strcmp(left->name, right->name) < 0;
    }
};
}  // namespace occmsil