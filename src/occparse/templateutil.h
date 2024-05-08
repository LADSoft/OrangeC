/* Software License Agreement
 * 
 *     Copyright(C); 1994-2023 David Lindauer, (LADSoft);
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option); any later version.
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
EXPRESSION* GetSymRef(EXPRESSION* n);
bool equalTemplateIntNode(EXPRESSION* exp1, EXPRESSION* exp2);
bool templatecompareexpressions(EXPRESSION* exp1, EXPRESSION* exp2);
bool templateselectorcompare(std::vector<TEMPLATESELECTOR>* tsin1, std::vector<TEMPLATESELECTOR>* tsin2);
bool templateCompareTypes(Type* tp1, Type* tp2, bool exact, bool sameType = true);
bool exactMatchOnTemplateParams(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym);
bool exactMatchOnTemplateArgs(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym);
static std::list<TEMPLATEPARAMPAIR>* mergeTemplateDefaults(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym,
                                                           bool definition);
static void checkTemplateDefaults(std::list<TEMPLATEPARAMPAIR>* args);
bool matchTemplateSpecializationToParams(std::list<TEMPLATEPARAMPAIR>* param, std::list<TEMPLATEPARAMPAIR>* special, SYMBOL* sp);
static void checkMultipleArgs(std::list<TEMPLATEPARAMPAIR>* sym);
std::list<TEMPLATEPARAMPAIR>* TemplateMatching(LexList* lex, std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym,
                                               SYMBOL* sp, bool definition);
static bool structHasTemplateArg(std::list<TEMPLATEPARAMPAIR>* tplx);
bool typeHasTemplateArg(Type* t);
void TemplateValidateSpecialization(std::list<TEMPLATEPARAMPAIR>* arg);
void GetPackedTypes(TEMPLATEPARAMPAIR** packs, int* count, std::list<TEMPLATEPARAMPAIR>* args);
void saveParams(SYMBOL** table, int count);
void restoreParams(SYMBOL** table, int count);
static std::list<Argument*>* ExpandArguments(EXPRESSION* exp);
static void PushPopDefaults(std::deque<Type*>& defaults, EXPRESSION* exp, bool dflt, bool push);
void PushPopDefaults(std::deque<Type*>& defaults, std::list<TEMPLATEPARAMPAIR>* tpx, bool dflt, bool push);
std::list<TEMPLATEPARAMPAIR>* ExpandParams(EXPRESSION* exp);
static Type* LookupUnaryMathFromExpression(EXPRESSION* exp, Keyword kw, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt);
static Type* LookupBinaryMathFromExpression(EXPRESSION* exp, Keyword kw, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt);
Type* LookupTypeFromExpression(EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt);
bool HasUnevaluatedTemplateSelectors(EXPRESSION* exp);
Type* TemplateLookupTypeFromDeclType(Type* tp);
static bool hastemplate(EXPRESSION* exp);
void clearoutDeduction(Type* tp);
void pushContext(SYMBOL* cls, bool all);
void SetTemplateNamespace(SYMBOL* sym);
int PushTemplateNamespace(SYMBOL* sym);
void PopTemplateNamespace(int n);
static SYMBOL* FindTemplateSelector(std::vector<TEMPLATESELECTOR>* tso);
static void FixIntSelectors(EXPRESSION** exp);
static std::list<TEMPLATEPARAMPAIR>* ResolveTemplateSelector(SYMBOL* sp, TEMPLATEPARAMPAIR* arg, bool byVal);
static std::list<TEMPLATEPARAMPAIR>* CopyArgsBack(std::list<TEMPLATEPARAMPAIR>* args, TEMPLATEPARAMPAIR* hold[], int k1);
std::list<TEMPLATEPARAMPAIR>* ResolveTemplateSelectors(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, bool byVal);
Type* ResolveTemplateSelectors(SYMBOL* sp, Type* tp);
std::list<TEMPLATEPARAMPAIR>* ResolveDeclType(SYMBOL* sp, TEMPLATEPARAMPAIR* tpx, bool returnNull = false);
std::list<TEMPLATEPARAMPAIR>* ResolveDeclTypes(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args);
static std::list<TEMPLATEPARAMPAIR>* ResolveConstructor(SYMBOL* sym, TEMPLATEPARAMPAIR* tpx);
TEMPLATEPARAMPAIR* TypeAliasSearch(const char* name, bool toponly);
std::list<TEMPLATEPARAMPAIR>* ResolveClassTemplateArgs(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args);
void copySyms(SYMBOL* found1, SYMBOL* sym);
}  // namespace Parser