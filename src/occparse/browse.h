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

namespace Parser
{
void abspath(char* name);
void browse_init(void);
void browsedata(Optimizer::BROWSEINFO* bi);
void browsefile(Optimizer::BROWSEFILE* bf);
void browse_startfunc(SYMBOL* func, int lineno);
void browse_endfunc(SYMBOL* func, int lineno);
void browse_startfile(const char* name, int index);
void browse_variable(SYMBOL* var);
void browse_usage(SYMBOL* var, int file);
void browse_define(char* name, int lineno, int charindex);
void browse_blockstart(int lineno);
void browse_blockend(int lineno);
}  // namespace Parser