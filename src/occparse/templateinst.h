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
extern int templateDeclarationLevel;
extern SYMBOL* defaultParsingContext;

Type* SolidifyType(Type* tp);
std::list<TEMPLATEPARAMPAIR>* SolidifyTemplateParams(std::list<TEMPLATEPARAMPAIR>* in);
std::list<TEMPLATEPARAMPAIR>* copyParams(std::list<TEMPLATEPARAMPAIR>* t, bool alsoSpecializations, bool unique = false);
void SynthesizeQuals(Type*** last, Type** qual, Type*** lastQual);
static EXPRESSION* copy_expression_data(EXPRESSION* exp);
EXPRESSION* copy_expression(EXPRESSION* head);
static std::list<TEMPLATEPARAMPAIR>* paramsToDefault(std::list<TEMPLATEPARAMPAIR>* templateParams);
static std::list<TEMPLATEPARAMPAIR>** addStructParam(std::list<TEMPLATEPARAMPAIR>** pt, TEMPLATEPARAMPAIR& search,
                                                     std::list<TEMPLATEPARAMPAIR>* enclosing);
static Type* SynthesizeStructure(Type* tp_in, std::list<TEMPLATEPARAMPAIR>* enclosing);
Type* SynthesizeType(Type* tp, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt, bool arg = false);
static bool hasPack(Type* tp);
static SYMBOL* SynthesizeParentClass(SYMBOL* sym);
SYMBOL* SynthesizeResult(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params);
static int eval(EXPRESSION* exp);
static bool ValidExp(EXPRESSION** exp_in);
static bool ValidArg(Type* tp);
static bool valFromDefault(std::list<TEMPLATEPARAMPAIR>* params, bool usesParams, std::list<Argument*>* args);
static void FillNontypeExpressionDefaults(EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* enclosing);
static void FillNontypeTypeDefaults(Type* tp, std::list<TEMPLATEPARAMPAIR>* enclosing);
static bool SetTemplateParamValue(TEMPLATEPARAMPAIR* p, std::list<TEMPLATEPARAMPAIR>* enclosing);
static void FillNontypeTemplateParamDefaults(std::list<TEMPLATEPARAMPAIR>* fills, std::list<TEMPLATEPARAMPAIR>* enclosing);
static bool checkNonTypeTypes(std::list<TEMPLATEPARAMPAIR>* params, std::list<TEMPLATEPARAMPAIR>* enclosing);
SYMBOL* ValidateArgsSpecified(std::list<TEMPLATEPARAMPAIR>* params, SYMBOL* func, std::list<Argument*>* args,
                              std::list<TEMPLATEPARAMPAIR>* nparams);
bool TemplateParseDefaultArgs(SYMBOL* declareSym, std::list<TEMPLATEPARAMPAIR>* args, std::list<TEMPLATEPARAMPAIR>* dest,
                              std::list<TEMPLATEPARAMPAIR>* src, std::list<TEMPLATEPARAMPAIR>* enclosing);
static void SetTemplateArgAccess(SYMBOL* sym, bool accessible);
static void SetAccessibleTemplateArgs(std::list<TEMPLATEPARAMPAIR>* args, bool accessible);
void SwapMainTemplateArgs(SYMBOL* cls);
SYMBOL* TemplateClassInstantiateInternal(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool isExtern);
SYMBOL* TemplateClassInstantiate(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool isExtern, StorageClass storage_class);
void TemplateDataInstantiate(SYMBOL* sym, bool warning, bool isExtern);
SYMBOL* TemplateFunctionInstantiate(SYMBOL* sym, bool warning);
static bool CheckConstCorrectness(Type* P, Type* A, bool byClass);
static void TemplateConstOrdering(SYMBOL** spList, int n, std::list<TEMPLATEPARAMPAIR>* params);
static bool TemplateConstMatchingInternal(std::list<TEMPLATEPARAMPAIR>* P);
static void TemplateConstMatching(SYMBOL** spList, int n, std::list<TEMPLATEPARAMPAIR>* params);
void TransferClassTemplates(std::list<TEMPLATEPARAMPAIR>* dflt, std::list<TEMPLATEPARAMPAIR>* val,
                            std::list<TEMPLATEPARAMPAIR>* params);
static SYMBOL* ValidateClassTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* unspecialized, std::list<TEMPLATEPARAMPAIR>* args);
static bool checkArgType(Type* tp, bool checkDeduced, bool checkDeclaring);
static bool checkArgSpecified(TEMPLATEPARAMPAIR* arg, bool checkDeduced, bool checkDeclaring);
bool allTemplateArgsSpecified(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool checkDeduced = false,
                              bool checkDeclaring = false);
void TemplateArgsAdd(TEMPLATEPARAMPAIR* current, TEMPLATEPARAMPAIR* dflt, std::list<TEMPLATEPARAMPAIR>* basetpl);
void TemplateArgsTemplateAdd(TEMPLATEPARAMPAIR* current, TEMPLATEPARAMPAIR* special, std::list<TEMPLATEPARAMPAIR>* base);
void TemplateArgsScan(std::list<TEMPLATEPARAMPAIR>* currents, std::list<TEMPLATEPARAMPAIR>* base);
void TemplateArgsCopy(std::list<TEMPLATEPARAMPAIR>* base);
void DuplicateTemplateParamList(std::list<TEMPLATEPARAMPAIR>** pptr);
static bool constOnly(SYMBOL** spList, SYMBOL** origList, int n);
static int SpecializationComplexity(std::list<TEMPLATEPARAMPAIR>* tpx);
static int MoreSpecialized(SYMBOL* left, SYMBOL* right);
static bool LessParams(SYMBOL* left, SYMBOL* right);
static void ChooseMoreSpecialized(SYMBOL** list, int n);
SYMBOL* GetClassTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, bool noErr);
SYMBOL* GetVariableTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args);
bool ReplaceIntAliasParams(EXPRESSION** exp, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                           std::list<TEMPLATEPARAMPAIR>* origUsing);
void SearchAlias(const char* name, TEMPLATEPARAMPAIR* x, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                 std::list<TEMPLATEPARAMPAIR>* origUsing);
static Type* ReplaceTemplateParam(Type* in);
void SpecifyTemplateSelector(std::vector<TEMPLATESELECTOR>** rvs, std::vector<TEMPLATESELECTOR>* old, bool expression, SYMBOL* sym,
                             std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
static EXPRESSION* SpecifyArgInt(SYMBOL* sym, EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* orig,
                                 std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
static Type* SpecifyArgType(SYMBOL* sym, Type* tp, TEMPLATEPARAM* tpt, std::list<TEMPLATEPARAMPAIR>* orig,
                            std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
static void SpecifyOneArg(SYMBOL* sym, TEMPLATEPARAMPAIR* temp, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                          std::list<TEMPLATEPARAMPAIR>* origUsing);
static bool ParseTypeAliasDefaults(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                                   std::list<TEMPLATEPARAMPAIR>* origUsing);
std::list<TEMPLATEPARAMPAIR>* GetTypeAliasArgs(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args,
                                               std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
static std::list<TEMPLATEPARAMPAIR>* TypeAliasAdjustArgs(std::list<TEMPLATEPARAMPAIR>* tpx, std::list<TEMPLATEPARAMPAIR>* args);
SYMBOL* ParseLibcxxAliases(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args);
SYMBOL* GetTypeAliasSpecialization(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args);
}  // namespace Parser