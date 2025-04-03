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

#include <map>
#include <vector>

extern PELib* peLib;
extern NetCore* netCoreInstance;

namespace occmsil
{
extern int uniqueId;
extern Optimizer::SimpleSymbol retblocksym;
extern int errCount;
extern Method* mainSym;
extern int hasEntryPoint;

extern MethodSignature* argsCtor;
extern MethodSignature* argsNextArg;
extern MethodSignature* argsUnmanaged;
extern MethodSignature* ptrBox;
extern MethodSignature* ptrUnbox;
extern MethodSignature* concatStr;
extern MethodSignature* concatObj;
extern MethodSignature* toStr;
extern MethodSignature* delegateInvoker;
extern MethodSignature* delegateAllocator;
extern MethodSignature* delegateFreer;
extern Class* multicastDelegate;
extern Type* systemObject;
extern Type* intPtr;
extern Method* currentMethod;
extern DataContainer* mainContainer;
extern Optimizer::LIST *initializersHead, *initializersTail;
extern Optimizer::LIST *deinitializersHead, *deinitializersTail;

extern std::map<Optimizer::SimpleSymbol*, Value*, byName> externalMethods;
extern std::map<Optimizer::SimpleSymbol*, Value*, byName> externalList;
extern std::map<Optimizer::SimpleSymbol*, Value*, byName> globalMethods;
extern std::map<Optimizer::SimpleSymbol*, Value*, byName> globalList;
extern std::map<Optimizer::SimpleSymbol*, Value*, byLabel> staticMethods;
extern std::map<Optimizer::SimpleSymbol*, Value*, byLabel> staticList;
extern std::map<Optimizer::SimpleSymbol*, MethodSignature*, byName> pinvokeInstances;
extern std::map<Optimizer::SimpleSymbol*, Param*, byName> paramList;
extern std::multimap<std::string, MethodSignature*> pInvokeReferences;

extern std::map<std::string, Value*> startups, rundowns, tlsstartups, tlsrundowns;

extern std::map<std::string, Type*> typeList;
extern std::map<Optimizer::SimpleSymbol*, Value*, byField> fieldList;
extern std::map<std::string, MethodSignature*> arrayMethods;

extern std::vector<Local*> localList;

bool LoadAssembly(const char* name);

MethodSignature* FindMethodSignature(const char* name);
Type* FindType(const char* name, bool toErr);
int msil_main_preprocess(char* fileName);
void msil_end_generation(char* fileName);
void msil_compile_start(char* name);
}  // namespace occmsil