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

#include "DotNetPELib.h"
namespace DotNetPELib
{
const char* PELibError::errorNames[] = {
    "Variable Length Argument parameters already declared",
    "Stack underflow",
    "Mismatched stack at label",
    "Stack not empty at end of function",
    "Duplicate label",
    "Missing label",
    "Short branch out of range",
    "Index out of range",
    "Multiple Entry Points",
    "Missing Entry Point",
    "Expected SEH Try block",
    "Expected SEH Handler",
    "SEH Tag Mismatch",
    "Orphaned SEH Tag",
    "Invalid SEH Filter",
    "Invalid epilogue of SEH section",
};
}