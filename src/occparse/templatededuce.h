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

namespace Parser
{
void ClearArgValues(std::list<TEMPLATEPARAMPAIR>* params, bool specialized);
static void PushPopValues(std::list<TEMPLATEPARAMPAIR>* params, bool push);
static bool DeduceFromTemplates(Type* P, Type* A, bool change, bool byClass);
static bool DeduceFromBaseTemplates(Type* P, SYMBOL* A, bool change, bool byClass);
static bool DeduceFromMemberPointer(Type* P, Type* A, bool change, bool byClass);
static bool TemplateConstExpr(Type* tp, EXPRESSION* exp);
bool DeduceTemplateParam(TEMPLATEPARAMPAIR* Pt, Type* P, Type* A, EXPRESSION* exp, bool byClass, bool change);
bool Deduce(Type* P, Type* A, EXPRESSION* exp, bool change, bool byClass, bool allowSelectors, bool baseClasses);
static Type* GetForwardType(Type* P, Type* A, EXPRESSION* exp);
static bool TemplateDeduceFromArg(Type* orig, Type* sym, EXPRESSION* exp, bool allowSelectors, bool baseClasses);
void NormalizePacked(Type* tpo);
static bool TemplateDeduceArgList(SymbolTable<SYMBOL>::iterator funcArgs, SymbolTable<SYMBOL>::iterator funcArgsEnd,
                                  SymbolTable<SYMBOL>::iterator templateArgs, SymbolTable<SYMBOL>::iterator templateArgsEnd,
                                  std::list<Argument*>::iterator its, std::list<Argument*>::iterator itse, bool allowSelectors,
                                  bool baseClasses);
void ScrubTemplateValues(SYMBOL* func);
void PushPopTemplateArgs(SYMBOL* func, bool push);
SYMBOL* TemplateDeduceArgsFromArgs(SYMBOL* sym, CallSite* args);
static bool TemplateDeduceFromType(Type* P, Type* A);
SYMBOL* TemplateDeduceWithoutArgs(SYMBOL* sym);
static bool TemplateDeduceFromConversionType(Type* orig, Type* tp);
SYMBOL* TemplateDeduceArgsFromType(SYMBOL* sym, Type* tp);
int TemplatePartialDeduceFromType(Type* orig, Type* sym, bool byClass);
int TemplatePartialDeduce(Type* origl, Type* origr, Type* syml, Type* symr, bool byClass);
int TemplatePartialDeduceArgsFromType(SYMBOL* syml, SYMBOL* symr, Type* tpx, Type* tpr, CallSite* fcall);
static SYMBOL* SynthesizeTemplate(Type* tp, SYMBOL* rvt, sym::_symbody* rvs, Type* tpt);
void TemplatePartialOrdering(SYMBOL** table, int count, CallSite* funcparams, Type* atype, bool asClass, bool save);
void CTADLookup(SYMBOL* funcsp, EXPRESSION** exp, Type** templateType, CallSite* funcparams, int flags);
}  // namespace Parser