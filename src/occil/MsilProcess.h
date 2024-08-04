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

namespace occmsil
{
extern BoxedType* boxedType(int isz);

void parse_pragma(const char* kw, const char* tag);
const char* GetName(const DataContainer* container, std::string name = "");
MethodSignature* LookupArrayMethod(Type* tp, std::string name);
bool qualifiedStruct(Optimizer::SimpleSymbol* sp);
bool IsPointedStruct(Optimizer::SimpleType* tp);
Field* GetField(Optimizer::SimpleSymbol* sp);
MethodSignature* GetMethodSignature(Optimizer::SimpleType* tp, bool pinvoke);
MethodSignature* FindPInvokeWithVarargs(std::string name, std::list<Param*>::iterator vbegin, std::list<Param*>::iterator vend,
                                        size_t size);
MethodSignature* GetMethodSignature(Optimizer::SimpleSymbol* sp);
std::string GetArrayName(Optimizer::SimpleType* tp);
Value* GetStructField(Optimizer::SimpleSymbol* sp);
void msil_create_property(Optimizer::SimpleSymbol* property, Optimizer::SimpleSymbol* getter, Optimizer::SimpleSymbol* setter);
Type* GetType(Optimizer::SimpleType* tp, bool commit, bool funcarg = false, bool pinvoke = false);
void msil_oa_enter_type(Optimizer::SimpleSymbol* sp);
void CacheExtern(Optimizer::SimpleSymbol* sp);
void CacheGlobal(Optimizer::SimpleSymbol* sp);
void CacheStatic(Optimizer::SimpleSymbol* sp);
Value* GetParamData(std::string name);
Value* GetLocalData(Optimizer::SimpleSymbol* sp);
Value* GetFieldData(Optimizer::SimpleSymbol* sp);
void LoadLocals(std::vector<Optimizer::SimpleSymbol*>& vars);
void LoadParams(Optimizer::SimpleSymbol* funcsp, std::vector<Optimizer::SimpleSymbol*>& vars,
                std::map<Optimizer::SimpleSymbol*, Param*, byName>& paramList);
void msil_flush_peep(Optimizer::SimpleSymbol* funcsp, Optimizer::QUAD* list);
void CreateFunction(MethodSignature* sig, Optimizer::SimpleSymbol* sp);
void ReplaceName(std::map<std::string, Value*>& list, Value* v, char* name);
void msil_oa_gensrref(Optimizer::SimpleSymbol* sp, int val, int type);
void msil_oa_put_extern(Optimizer::SimpleSymbol* sp, int code);
void msil_oa_gen_strlab(Optimizer::SimpleSymbol* sp);
Type* GetStringType(int type);
Value* GetStringFieldData(int lab, int type);
void msil_oa_put_string_label(int lab, int type);
void msil_oa_genbyte(int bt);
void msil_oa_genstring(char* str, int len);
void msil_oa_enterseg(Optimizer::e_sg segnum);
void CreateStringFunction();
void msil_main_postprocess(bool errors);
}  // namespace occmsil