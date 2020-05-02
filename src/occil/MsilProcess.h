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
extern BoxedType* boxedType(int isz);

void parse_pragma(const char* kw, const char* tag);
const char *GetName(const DataContainer* container, std::string name = "");
MethodSignature* LookupArrayMethod(Type* tp, std::string name);
bool qualifiedStruct(SimpleSymbol* sp);
bool IsPointedStruct(SimpleType* tp);
Field* GetField(SimpleSymbol* sp);
MethodSignature* GetMethodSignature(SimpleType* tp, bool pinvoke);
MethodSignature* FindPInvokeWithVarargs(std::string name, std::list<Param*>::iterator vbegin, std::list<Param*>::iterator vend,
    size_t size);
MethodSignature* GetMethodSignature(SimpleSymbol* sp);
std::string GetArrayName(SimpleType* tp);
Value* GetStructField(SimpleSymbol* sp);
void msil_create_property(SimpleSymbol* property, SimpleSymbol* getter, SimpleSymbol* setter);
Type* GetType(SimpleType* tp, bool commit, bool funcarg=false, bool pinvoke=false);
void msil_oa_enter_type(SimpleSymbol* sp);
void CacheExtern(SimpleSymbol* sp);
void CacheGlobal(SimpleSymbol* sp);
void CacheStatic(SimpleSymbol* sp);
Value* GetParamData(std::string name);
Value* GetLocalData(SimpleSymbol* sp);
Value* GetFieldData(SimpleSymbol* sp);
void LoadLocals(std::vector<SimpleSymbol*>& vars);
void LoadParams(SimpleSymbol* funcsp, std::vector<SimpleSymbol*>& vars, std::map<SimpleSymbol*, Param*, byName>& paramList);
void msil_flush_peep(SimpleSymbol* funcsp, QUAD* list);
void CreateFunction(MethodSignature* sig, SimpleSymbol* sp);
void ReplaceName(std::map<std::string, Value*>& list, Value* v, char* name);
void msil_oa_gensrref(SimpleSymbol* sp, int val, int type);
void msil_oa_put_extern(SimpleSymbol* sp, int code);
void msil_oa_gen_strlab(SimpleSymbol* sp);
Type* GetStringType(int type);
Value* GetStringFieldData(int lab, int type);
void msil_oa_put_string_label(int lab, int type);
void msil_oa_genbyte(int bt);
void msil_oa_genstring(char* str, int len);
void msil_oa_enterseg(e_sg segnum);
void msil_main_postprocess(bool errors);
