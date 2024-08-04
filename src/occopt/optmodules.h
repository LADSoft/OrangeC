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
 */
#pragma once

#include <string>

#define OPT_RESHAPE 1
#define OPT_LSTRENGTH 2
#define OPT_GCSE 4
#define OPT_CONSTANT 8
#define OPT_INVARIANT 0x10

#define ICD_QUITEARLY 0x80000000
#define ICD_OCP (1 | ICD_QUITEARLY)
#define ICD_STAYSSA (2 | ICD_QUITEARLY)

#define OPT_BYTECOMPARE 0x10000
#define OPT_REVERSESTORE 0x20000
#define OPT_REVERSEPARAM 0x40000
#define OPT_ARGSTRUCTREF 0x80000
#define OPT_EXPANDSWITCH 0x100000
#define OPT_THUNKRETVAL 0x200000

#define OPTMODULES_DESCRIPTION                                                          \
    "  -fopt-{no}constant             turn on or off constant optimizations\n"          \
    "  -fopt-{no}loop-strength        turn on or off loop strength optimization\n"      \
    "  -fopt-{no}move-invariants      turn on or off loop invariant code motion\n"      \
    "  -fopt-{no}gcse                 turn on or off global subexpression evaluation\n" \
    "  -ficd-{no}gcse                 turn on or off gcse diagnostics in the icd file\n"

#define OPTIMIZATION_DESCRIPTION                               \
    "  -O-, -O0                       disable optimizations\n" \
    "  -O1                            optimize for size\n"     \
    "  -O2                            optimize for speed\n"    \
    "  -Of                            optimize floating point accesses\n"

namespace Optimizer
{
void optimize_setup(char select, const char* string);
std::string ParseOptimizerParams(std::string in);
void OptimizerStats();
void RunOptimizerModules();
}  // namespace Optimizer