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
extern int noTypeNameError;
extern int inTemplateHeader;
extern SYMBOL* instantiatingMemberFuncClass;
extern int instantiatingFunction;
extern int instantiatingClass;
extern int parsingDefaultTemplateArgs;
extern int count1;
extern int inTemplateArgs;
extern bool fullySpecialized;

extern int templateNameTag;
extern std::unordered_map<SYMBOL*, std::unordered_map<std::string, SYMBOL*, StringHash>> classTemplateMap;
extern std::unordered_map<std::string, SYMBOL*, StringHash> classTemplateMap2;
extern std::unordered_map<std::string, SYMBOL*, StringHash> classInstantiationMap;

extern struct templateListData* currents;

void templateInit(void);
void TemplateGetDeferred(SYMBOL* sym);
TEMPLATEPARAMPAIR* TemplateLookupSpecializationParam(const char* name);
std::list<TEMPLATEPARAMPAIR>* TemplateGetParams(SYMBOL* sym);
void TemplateRegisterDeferred(LEXLIST* lex);
static std::list<TEMPLATEPARAMPAIR>** expandArgs(std::list<TEMPLATEPARAMPAIR>** lst, LEXLIST* start, SYMBOL* funcsp,
                                                 std::list<TEMPLATEPARAMPAIR>* select, bool packable);
void UnrollTemplatePacks(std::list<TEMPLATEPARAMPAIR>* tplx);
static std::list<TEMPLATEPARAMPAIR>* nextExpand(std::list<TEMPLATEPARAMPAIR>* inx, int n);
std::list<TEMPLATEPARAMPAIR>** expandTemplateSelector(std::list<TEMPLATEPARAMPAIR>** lst, std::list<TEMPLATEPARAMPAIR>* orig,
                                                      TYPE* tp);

bool constructedInt(LEXLIST* lex, SYMBOL* funcsp);
LEXLIST* GetTemplateArguments(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* templ, std::list<TEMPLATEPARAMPAIR>** lst);
static bool sameTemplateSpecialization(TYPE* P, TYPE* A);
bool exactMatchOnTemplateSpecialization(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym);
SYMBOL* FindSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* templateParams);
SYMBOL* LookupSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* templateParams);
static bool matchTemplatedType(TYPE* old, TYPE* sym, bool strict);
SYMBOL* LookupFunctionSpecialization(SYMBOL* overloads, SYMBOL* sp);
LEXLIST* TemplateArgGetDefault(LEXLIST** lex, bool isExpression);
static SYMBOL* templateParamId(TYPE* tp, const char* name, int tag);
static LEXLIST* TemplateHeader(LEXLIST* lex, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* args);
static LEXLIST* TemplateArg(LEXLIST* lex, SYMBOL* funcsp, TEMPLATEPARAMPAIR& arg, std::list<TEMPLATEPARAMPAIR>** lst,
                            bool templateParam);
static bool matchArg(TEMPLATEPARAMPAIR& param, TEMPLATEPARAMPAIR& arg);
bool TemplateIntroduceArgs(std::list<TEMPLATEPARAMPAIR>* sym, std::list<TEMPLATEPARAMPAIR>* args);
static bool comparePointerTypes(TYPE* tpo, TYPE* tps);
static bool TemplateInstantiationMatchInternal(std::list<TEMPLATEPARAMPAIR>* porig, std::list<TEMPLATEPARAMPAIR>* psym, bool dflt,
                                               bool bySpecialization);
bool TemplateInstantiationMatch(SYMBOL* orig, SYMBOL* sym, bool bySpecialization);
void TemplateTransferClassDeferred(SYMBOL* newCls, SYMBOL* tmpl);
static bool ValidSpecialization(std::list<TEMPLATEPARAMPAIR>* special, std::list<TEMPLATEPARAMPAIR>* args, bool templateMatch);
SYMBOL* MatchSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args);
void DoInstantiateTemplateFunction(TYPE* tp, SYMBOL** sp, std::list<NAMESPACEVALUEDATA*>* nsv, SYMBOL* strSym,
                                   std::list<TEMPLATEPARAMPAIR>* templateParams, bool isExtern);
static void referenceInstanceMembers(SYMBOL* cls, bool excludeFromExplicitInstantiation);
static void dontInstantiateInstanceMembers(SYMBOL* cls, bool excludeFromExplicitInstantiation);
static bool IsFullySpecialized(TEMPLATEPARAMPAIR* tpx);
bool TemplateFullySpecialized(SYMBOL* sp);
void propagateTemplateDefinition(SYMBOL* sym);
static void MarkDllLinkage(SYMBOL* sp, Linkage linkage);
static void DoInstantiate(SYMBOL* strSym, SYMBOL* sym, TYPE* tp, std::list<NAMESPACEVALUEDATA*>* nsv, bool isExtern);
bool inCurrentTemplate(const char* name);
bool definedInTemplate(const char* name);
LEXLIST* TemplateDeclaration(LEXLIST* lex, SYMBOL* funcsp, AccessLevel access, StorageClass storage_class, bool isExtern);
}  // namespace Parser