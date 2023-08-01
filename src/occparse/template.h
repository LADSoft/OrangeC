#pragma once
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
extern int dontRegisterTemplate;
extern int instantiatingTemplate;
extern int inTemplateBody;
extern int templateNestingCount;
extern int templateHeaderCount;
extern int inTemplateSpecialization;
extern int inDeduceArgs;
extern bool parsingSpecializationDeclaration;
extern bool inTemplateType;
extern int inTemplateHeader;
extern int noTypeNameError;
extern SYMBOL* instantiatingMemberFuncClass;
extern struct templateListData* currents;
extern int instantiatingFunction;
extern int instantiatingClass;
extern int parsingDefaultTemplateArgs;
extern int inTemplateArgs;
extern std::unordered_map<std::string, SYMBOL*, StringHash> classTemplateMap2;

void templateInit(void);
EXPRESSION* GetSymRef(EXPRESSION* n);
bool equalTemplateIntNode(EXPRESSION* exp1, EXPRESSION* exp2);
bool templatecompareexpressions(EXPRESSION* exp1, EXPRESSION* exp2);
bool templateselectorcompare(std::vector<TEMPLATESELECTOR>* tsin1,
                             std::vector<TEMPLATESELECTOR>* tsin2);
bool templatecomparetypes(TYPE* tp1, TYPE* tp2, bool exact, bool sameType = true);
void TemplateGetDeferred(SYMBOL* sym);
TEMPLATEPARAMPAIR* TemplateLookupSpecializationParam(const char* name);
std::list<TEMPLATEPARAMPAIR>* TemplateGetParams(SYMBOL* sym);
void TemplateRegisterDeferred(LEXLIST* lex);
bool exactMatchOnTemplateParams(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym);
bool exactMatchOnTemplateArgs(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym);
bool matchTemplateSpecializationToParams(std::list<TEMPLATEPARAMPAIR>* param, std::list<TEMPLATEPARAMPAIR>* special, SYMBOL* sp);
std::list<TEMPLATEPARAMPAIR>* TemplateMatching(LEXLIST* lex, std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym, SYMBOL* sp, bool definition);
bool typeHasTemplateArg(TYPE* t);
void TemplateValidateSpecialization(std::list<TEMPLATEPARAMPAIR>* arg);
std::list<TEMPLATEPARAMPAIR>** expandTemplateSelector(std::list<TEMPLATEPARAMPAIR>** lst, std::list<TEMPLATEPARAMPAIR>* orig, TYPE* tp);
bool constructedInt(LEXLIST* lex, SYMBOL* funcsp);
LEXLIST* GetTemplateArguments(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* templ, std::list<TEMPLATEPARAMPAIR>** lst);
bool exactMatchOnTemplateSpecialization(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym);
SYMBOL* FindSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* templateParams);
SYMBOL* LookupSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* templateParams);
SYMBOL* LookupFunctionSpecialization(SYMBOL* overloads, SYMBOL* sp);
LEXLIST* TemplateArgGetDefault(LEXLIST** lex, bool isExpression);
bool TemplateIntroduceArgs(std::list<TEMPLATEPARAMPAIR>* sym, std::list<TEMPLATEPARAMPAIR>* args);
TYPE* SolidifyType(TYPE* tp);
std::list<TEMPLATEPARAMPAIR>* SolidifyTemplateParams(std::list<TEMPLATEPARAMPAIR>* in);
std::list<TEMPLATEPARAMPAIR>* ResolveTemplateSelectors(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, bool byVal);
TYPE* ResolveTemplateSelectors(SYMBOL* sp, TYPE* tp);
void SynthesizeQuals(TYPE*** last, TYPE** qual, TYPE*** lastQual);
EXPRESSION* copy_expression(EXPRESSION* head);
TYPE* LookupTypeFromExpression(EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt);
TYPE* TemplateLookupTypeFromDeclType(TYPE* tp);
TYPE* SynthesizeType(TYPE* tp, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt);
SYMBOL* SynthesizeResult(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params);
void NormalizePacked(TYPE* tpo);
void ScrubTemplateValues(SYMBOL* func);
void PushPopTemplateArgs(SYMBOL* func, bool push);
SYMBOL* TemplateDeduceArgsFromArgs(SYMBOL* sym, FUNCTIONCALL* args);
SYMBOL* TemplateDeduceWithoutArgs(SYMBOL* sym);
SYMBOL* TemplateDeduceArgsFromType(SYMBOL* sym, TYPE* tp);
int TemplatePartialDeduceFromType(TYPE* orig, TYPE* sym, bool byClass);
int TemplatePartialDeduce(TYPE* origl, TYPE* origr, TYPE* syml, TYPE* symr, bool byClass);
int TemplatePartialDeduceArgsFromType(SYMBOL* syml, SYMBOL* symr, TYPE* tpl, TYPE* tpr, FUNCTIONCALL* fcall);
void TemplatePartialOrdering(SYMBOL** table, int count, FUNCTIONCALL* funcparams, TYPE* atype, bool asClass, bool save);
bool TemplateInstantiationMatch(SYMBOL* orig, SYMBOL* sym);
void SetTemplateNamespace(SYMBOL* sym);
int PushTemplateNamespace(SYMBOL* sym);
void PopTemplateNamespace(int n);
void PushPopDefaults(std::deque<TYPE*>& defaults, std::list<TEMPLATEPARAMPAIR>* tpx, bool dflt, bool push);
void SwapMainTemplateArgs(SYMBOL* cls);
std::list<TEMPLATEPARAMPAIR>* copyParams(std::list<TEMPLATEPARAMPAIR>* t, bool alsoSpecializations);
bool TemplateParseDefaultArgs(SYMBOL* declareSym, std::list<TEMPLATEPARAMPAIR>* args, std::list<TEMPLATEPARAMPAIR>* dest, std::list<TEMPLATEPARAMPAIR>* src,
                              std::list<TEMPLATEPARAMPAIR>* enclosing);
SYMBOL* TemplateClassInstantiateInternal(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool isExtern);
SYMBOL* TemplateClassInstantiate(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool isExtern, StorageClass storage_class);
void TemplateDataInstantiate(SYMBOL* sym, bool warning, bool isExtern);
SYMBOL* TemplateFunctionInstantiate(SYMBOL* sym, bool warning);
bool allTemplateArgsSpecified(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool checkDeduced = false, bool checkDeclaring = false);
void DuplicateTemplateParamList(std::list<TEMPLATEPARAMPAIR>** pptr);
SYMBOL* TemplateByValLookup(SYMBOL* parent, SYMBOL* test, std::string& argumentName);
SYMBOL* GetClassTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, bool noErr);
SYMBOL* GetVariableTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args);
int pushContext(SYMBOL* cls, bool all);
TYPE* CloneUsingSyms(TYPE* sp);
SYMBOL* GetTypeAliasSpecialization(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args);
void DoInstantiateTemplateFunction(TYPE* tp, SYMBOL** sp, std::list<NAMESPACEVALUEDATA*>* nsv, SYMBOL* strSym,
    std::list<TEMPLATEPARAMPAIR>* templateParams, bool isExtern);
bool TemplateFullySpecialized(SYMBOL* sp);
void propagateTemplateDefinition(SYMBOL* sym);
bool inCurrentTemplate(const char* name);
bool definedInTemplate(const char* name);
LEXLIST* TemplateDeclaration(LEXLIST* lex, SYMBOL* funcsp, AccessLevel access, StorageClass storage_class, bool isExtern);
}  // namespace Parser