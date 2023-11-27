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

namespace Parser
{
void ClearArgValues(std::list<TEMPLATEPARAMPAIR>* params, bool specialized);
static void PushPopValues(std::list<TEMPLATEPARAMPAIR>* params, bool push);
static bool DeduceFromTemplates(TYPE* P, TYPE* A, bool change, bool byClass);
static bool DeduceFromBaseTemplates(TYPE* P, SYMBOL* A, bool change, bool byClass);
static bool DeduceFromMemberPointer(TYPE* P, TYPE* A, bool change, bool byClass);
static bool TemplateConstExpr(TYPE* tp, EXPRESSION* exp);
bool DeduceTemplateParam(TEMPLATEPARAMPAIR* Pt, TYPE* P, TYPE* A, EXPRESSION* exp, bool change);
bool Deduce(TYPE* P, TYPE* A, EXPRESSION* exp, bool change, bool byClass, bool allowSelectors, bool baseClasses);
static TYPE* GetForwardType(TYPE* P, TYPE* A, EXPRESSION* exp);
static bool TemplateDeduceFromArg(TYPE* orig, TYPE* sym, EXPRESSION* exp, bool allowSelectors, bool baseClasses);
void NormalizePacked(TYPE* tpo);
static bool TemplateDeduceArgList(SymbolTable<SYMBOL>::iterator funcArgs, SymbolTable<SYMBOL>::iterator funcArgsEnd,
                                  SymbolTable<SYMBOL>::iterator templateArgs, SymbolTable<SYMBOL>::iterator templateArgsEnd,
                                  std::list<INITLIST*>::iterator its, std::list<INITLIST*>::iterator itse, bool allowSelectors,
                                  bool baseClasses);
void ScrubTemplateValues(SYMBOL* func);
void PushPopTemplateArgs(SYMBOL* func, bool push);
SYMBOL* TemplateDeduceArgsFromArgs(SYMBOL* sym, FUNCTIONCALL* args);
static bool TemplateDeduceFromType(TYPE* P, TYPE* A);
SYMBOL* TemplateDeduceWithoutArgs(SYMBOL* sym);
static bool TemplateDeduceFromConversionType(TYPE* orig, TYPE* tp);
SYMBOL* TemplateDeduceArgsFromType(SYMBOL* sym, TYPE* tp);
int TemplatePartialDeduceFromType(TYPE* orig, TYPE* sym, bool byClass);
int TemplatePartialDeduce(TYPE* origl, TYPE* origr, TYPE* syml, TYPE* symr, bool byClass);
int TemplatePartialDeduceArgsFromType(SYMBOL* syml, SYMBOL* symr, TYPE* tpx, TYPE* tpr, FUNCTIONCALL* fcall);
static SYMBOL* SynthesizeTemplate(TYPE* tp, SYMBOL* rvt, sym::_symbody* rvs, TYPE* tpt);
void TemplatePartialOrdering(SYMBOL** table, int count, FUNCTIONCALL* funcparams, TYPE* atype, bool asClass, bool save);
}  // namespace Parser