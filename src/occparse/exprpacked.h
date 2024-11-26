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
    extern int packIndex;

void checkPackedType(SYMBOL* sym);
void checkPackedExpression(EXPRESSION* exp);
void checkUnpackedExpression(EXPRESSION* exp);
void expandPackedBaseClasses(SYMBOL* cls, SYMBOL* funcsp, std::list<MEMBERINITIALIZERS*>::iterator& init,
    std::list<MEMBERINITIALIZERS*>::iterator& initend, std::list<MEMBERINITIALIZERS*>* mi,
    std::list<BASECLASS*>* bc, std::list<VBASEENTRY*>* vbase);
void ExpandExpression(LexList* start, SYMBOL* funcsp, EXPRESSION** exp);
void expandPackedMemberInitializers(SYMBOL* cls, SYMBOL* funcsp,
    std::list<TEMPLATEPARAMPAIR>* templatePack,
    std::list<MEMBERINITIALIZERS*>** p,
    LexList* start, std::list<Argument*>* list);
std::list<Argument*>* ExpandTemplateArguments(EXPRESSION* exp);
std::list<TEMPLATEPARAMPAIR>** ExpandTemplateArguments(std::list<TEMPLATEPARAMPAIR>** lst, LexList* start, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* select, bool packable);
void ExpandTemplateArguments(std::list<TEMPLATEPARAMPAIR>** lst, LexList* start, SYMBOL* name, SYMBOL* first, SYMBOL* funcsp, Type** tp, EXPRESSION** exp);
}  // namespace Parser
