#pragma once
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
extern SYMBOL* instantiatingMemberFuncClass;
extern bool reflectUsingType;
extern struct templateListData* currents;

void templateInit(void);
EXPRESSION* GetSymRef(EXPRESSION* n);
bool equalTemplateIntNode(EXPRESSION* exp1, EXPRESSION* exp2);
bool templatecompareexpressions(EXPRESSION* exp1, EXPRESSION* exp2);
bool templateselectorcompare(TEMPLATESELECTOR* tsin1, TEMPLATESELECTOR* tsin2);
bool templatecomparetypes(TYPE* tp1, TYPE* tp2, bool exact);
void TemplateGetDeferred(SYMBOL* sym);
TEMPLATEPARAMLIST* TemplateLookupSpecializationParam(const char* name);
TEMPLATEPARAMLIST* TemplateGetParams(SYMBOL* sym);
void TemplateRegisterDeferred(LEXEME* lex);
bool exactMatchOnTemplateParams(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym);
bool exactMatchOnTemplateArgs(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym);
bool matchTemplateSpecializationToParams(TEMPLATEPARAMLIST* param, TEMPLATEPARAMLIST* special, SYMBOL* sp);
TEMPLATEPARAMLIST* TemplateMatching(LEXEME* lex, TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym, SYMBOL* sp, bool definition);
bool typeHasTemplateArg(TYPE* t);
void TemplateValidateSpecialization(TEMPLATEPARAMLIST* arg);
TEMPLATEPARAMLIST** expandArgs(TEMPLATEPARAMLIST** lst, LEXEME* start, SYMBOL* funcsp, TEMPLATEPARAMLIST* select, bool packable);
TEMPLATEPARAMLIST** expandTemplateSelector(TEMPLATEPARAMLIST** lst, TEMPLATEPARAMLIST* orig, TYPE* tp);
bool constructedInt(LEXEME* lex, SYMBOL* funcsp);
LEXEME* GetTemplateArguments(LEXEME* lex, SYMBOL* funcsp, SYMBOL* templ, TEMPLATEPARAMLIST** lst);
bool exactMatchOnTemplateSpecialization(TEMPLATEPARAMLIST* old, TEMPLATEPARAMLIST* sym);
SYMBOL* FindSpecialization(SYMBOL* sym, TEMPLATEPARAMLIST* templateParams);
SYMBOL* LookupSpecialization(SYMBOL* sym, TEMPLATEPARAMLIST* templateParams);
SYMBOL* LookupFunctionSpecialization(SYMBOL* overloads, SYMBOL* sp);
LEXEME* TemplateArgGetDefault(LEXEME** lex, bool isExpression);
bool TemplateIntroduceArgs(TEMPLATEPARAMLIST* sym, TEMPLATEPARAMLIST* args);
TYPE* SolidifyType(TYPE* tp);
TEMPLATEPARAMLIST* SolidifyTemplateParams(TEMPLATEPARAMLIST* in);
void SynthesizeQuals(TYPE*** last, TYPE** qual, TYPE*** lastQual);
EXPRESSION* copy_expression(EXPRESSION* head);
TYPE* LookupTypeFromExpression(EXPRESSION* exp, TEMPLATEPARAMLIST* enclosing, bool alt);
TYPE* TemplateLookupTypeFromDeclType(TYPE* tp);
TYPE* SynthesizeType(TYPE* tp, TEMPLATEPARAMLIST* enclosing, bool alt);
SYMBOL* SynthesizeResult(SYMBOL* sym, TEMPLATEPARAMLIST* params);
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
void SwapMainTemplateArgs(SYMBOL* cls);
TEMPLATEPARAMLIST* copyParams(TEMPLATEPARAMLIST* t, bool alsoSpecializations);
bool TemplateParseDefaultArgs(SYMBOL* declareSym, TEMPLATEPARAMLIST* dest, TEMPLATEPARAMLIST* src,
                                     TEMPLATEPARAMLIST* enclosing);
SYMBOL* TemplateClassInstantiateInternal(SYMBOL* sym, TEMPLATEPARAMLIST* args, bool isExtern);
SYMBOL* TemplateClassInstantiate(SYMBOL* sym, TEMPLATEPARAMLIST* args, bool isExtern, enum e_sc storage_class);
void TemplateDataInstantiate(SYMBOL* sym, bool warning, bool isExtern);
SYMBOL* TemplateFunctionInstantiate(SYMBOL* sym, bool warning, bool isExtern);
bool allTemplateArgsSpecified(SYMBOL* sym, TEMPLATEPARAMLIST* args);
void DuplicateTemplateParamList(TEMPLATEPARAMLIST** pptr);
SYMBOL* GetClassTemplate(SYMBOL* sp, TEMPLATEPARAMLIST* args, bool noErr);
SYMBOL* GetVariableTemplate(SYMBOL* sp, TEMPLATEPARAMLIST* args);
int pushContext(SYMBOL* cls, bool all);
TYPE *CloneUsingSyms(TYPE *sp);
SYMBOL* GetTypeAliasSpecialization(SYMBOL* sp, TEMPLATEPARAMLIST* args);
void DoInstantiateTemplateFunction(TYPE* tp, SYMBOL** sp, NAMESPACEVALUELIST* nsv, SYMBOL* strSym,
                                   TEMPLATEPARAMLIST* templateParams, bool isExtern);
bool TemplateFullySpecialized(SYMBOL* sp);
void propagateTemplateDefinition(SYMBOL* sym);
LEXEME* TemplateDeclaration(LEXEME* lex, SYMBOL* funcsp, enum e_ac access, enum e_sc storage_class, bool isExtern);
}  // namespace Parser