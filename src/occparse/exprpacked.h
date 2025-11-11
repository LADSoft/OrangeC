/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#pragma once

namespace Parser
{
extern int unpackingTemplate;

void packed_init();
void PushPackIndex();
void PopPackIndex();
int GetPackIndex();
void SetPackIndex(int);
void EnterPackedContext();
void LeavePackedContext();
void EnterPackedSequence();
void LeavePackedSequence();
void ClearPackedSequence();
void AddPackedEntityToSequence(SYMBOL* name, TEMPLATEPARAM* packed);
TEMPLATEPARAM* LookupPackedInstance(TEMPLATEPARAMPAIR& packed);

bool IsPacking();
bool hasPackedExpression(EXPRESSION* exp, bool useAuto);
void GatherPackedTypes(int* count, SYMBOL** arg, Type* tp);
void GatherPackedVars(int* count, SYMBOL** arg, EXPRESSION* packedExp);
int CountPacks(std::list<TEMPLATEPARAMPAIR>* packs);
void expandPackedInitList(std::list<Argument*>** lptr, SYMBOL* funcsp, LexToken::iterator& start, EXPRESSION* packedExp);
void checkPackedType(SYMBOL* sym);
void checkPackedExpression(EXPRESSION* exp);
// void checkUnpackedExpression(EXPRESSION* exp);
void expandPackedBaseClasses(SYMBOL* cls, SYMBOL* funcsp, std::list<CONSTRUCTORINITIALIZER*>::iterator& init,
                             std::list<CONSTRUCTORINITIALIZER*>::iterator& initend, std::list<CONSTRUCTORINITIALIZER*>* mi,
                             std::list<BASECLASS*>* bc, std::list<VBASEENTRY*>* vbase);
void ExpandPackedConstructorInitializers(SYMBOL* cls, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* templatePack,
                                    std::list<CONSTRUCTORINITIALIZER*>** p, LexToken* start, std::list<Argument*>* list);
std::list<Argument*>* ExpandTemplateArguments(EXPRESSION* exp);
std::list<TEMPLATEPARAMPAIR>** ExpandTemplateArguments(std::list<TEMPLATEPARAMPAIR>** lst, SYMBOL* funcsp,
                                                       std::list<TEMPLATEPARAMPAIR>* select);
std::list<TEMPLATEPARAMPAIR>** ExpandTemplateArguments(std::list<TEMPLATEPARAMPAIR>** lst, LexToken::iterator& start, SYMBOL* funcsp,
                                                       std::list<TEMPLATEPARAMPAIR>* select);
void ExpandTemplateArguments(std::list<TEMPLATEPARAMPAIR>** lst, LexToken::iterator& start, SYMBOL* name, SYMBOL* first, SYMBOL* funcsp,
                             Type** tp, EXPRESSION** exp);
int GetPackCount();
}  // namespace Parser
